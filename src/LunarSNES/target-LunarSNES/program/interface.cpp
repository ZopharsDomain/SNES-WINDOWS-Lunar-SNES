auto Program::path(uint id) -> string {
  return mediumPaths(id);
}

auto Program::open(uint id, string name, vfs::file::mode mode, bool required) -> vfs::shared::file {
  if(id == SFC::ID::System) {
    const vector<uint8_t>* embed = nullptr;
    if(name == "ipl.rom") embed = &Embed::SuperFamicom::iplrom;
    if(name == "manifest.bml") embed = &Embed::SuperFamicom::manifest;
  //if(name == "boards.bml") embed = &Embed::SuperFamicom::boards;
    if(embed) return vfs::memory::file::open(embed->data(), embed->size());
  }

  if(name == "manifest.bml" && id != SFC::ID::System) {
    if(!file::exists({path(id), name})) {
      icarus->gamepak = directory::exists(path(id));
      if(!icarus->gamepak && !file::exists(path(id).trimRight("/"))) return {};
      if(!icarus->gamepak) icarus->import(path(id).trimRight("/"));
      if(auto output = icarus->select(path(id))) {
        return vfs::memory::file::open((uint8_t*)output->manifest.data(), output->manifest.size());
      }
    }
  }

  if(name == "hg51bs169.data.rom" || name == "cx4.data.rom") {
    vector<uint8_t> dataROM;
    #define triad(_value)\
      uint32_t value = _value;\
      dataROM.append(value >>  0);\
      dataROM.append(value >>  8);\
      dataROM.append(value >> 16)
    for(uint divisor : range(256)) {
      triad(divisor == 0 ? 0xffffff : 0x800000 / divisor);
    }
    for(uint square : range(256)) {
      triad(floor(0x100000 * sqrt(square)));
    }
    for(uint angle : range(128)) {
      triad(floor(0x1000000 * sin((angle / 256.0) * Math::Pi)));
    }
    for(uint sine : range(128)) {
      triad(floor(0x1000000 / Math::Pi * asin(sine / 128.0)));
    }
    for(uint angle : range(128)) {
      triad(floor(0x10000 * (angle == 64 ? 1.0 : tan((angle / 256.0) * Math::Pi))));
    }
    for(uint angle : range(128)) {
      triad(min(floor(0x1000000 * cos((angle / 256.0) * Math::Pi)), 0xffffff));
    }
    #undef triad
    return vfs::memory::file::open(dataROM.data(), dataROM.size());
  } else if(name.endsWith(".rom")) {
    if(auto result = icarus->read({path(id), name})) {
      return vfs::memory::file::open(result.data(), result.size());
    }
  } else {
    print(Location::dir(icarus->filename({path(id), name})), "\n");
    if(mode == vfs::file::mode::write) directory::create(Location::dir(icarus->filename({path(id), name})));
    if(auto result = vfs::fs::file::open(icarus->filename({path(id), name}), mode)) return result;
  }

  if(required) {
    MessageDialog()
    .setTitle({"Error"})
    .setText({"Error: missing required file:\n\n", path(id), name})
    .error();
  }

  return {};
}

auto Program::load(uint id, string name, string type, string_vector options) -> Emulator::Platform::Load {
  string location, option;
  if(mediumQueue) {
    auto entry = mediumQueue.takeLeft().split("|", 1L);
    location = entry.right();
    if(entry.size() == 1) option = options(0);
    if(entry.size() == 2) option = entry.left();
  } else {
    BrowserDialog dialog;
    location = dialog
    .setTitle({"Load ", name})
    .setPath(settings["Path/ROM"].text())
    .setFilters({string{name, "|*.", type == "sfc" ? "sfc"
      ":*.smc:*.SMC"
      ":*.swc:*.SWC"
      ":*.fig:*.FIG"
      ":*.mgd:*.MGD"
      ":*.mgh:*.MGH"
      ":*.ufo:*.UFO"
      ":*.bin:*.BIN"
      ":*.gd3:*.GD3"
      ":*.gd7:*.GD7"
      ":*.dx2:*.DX2"
      ":*.usa:*.USA"
      ":*.eur:*.EUR"
      ":*.jap:*.JAP"
      ":*.aus:*.AUS"
      ":*.048"
      ":*.058"
      ":*.078"
    : type, ":*.zip:*.ZIP"}, "All|*.*"})
    .setOptions(options)
    //Lunar SNES: use ROM files instead of cartridge folders.
    //TODO: In target-zenith, this should be changed to accept both.
    .openFile();
    option = dialog.option();
    settings["Path/ROM"].setValue(Location::dir(location));
  }
  if(!location.endsWith("/")) location.append("/");
  if(!directory::exists(location) && !file::exists(string{location}.trimRight("/"))) {
    mediumQueue.reset();
    return {};
  }

  uint pathID = mediumPaths.size();
  mediumPaths.append(location);
  return {pathID, option};
}

auto Program::videoRefresh(const uint32* data, uint pitch, uint width, uint height) -> void {
  presentation->videoBuffer = data;  //allow overlaying of snow/water/burning/smoke effects when paused
  uint32_t* output;
  uint length;

  pitch >>= 2;

  if(emulator->information.overscan) {
    uint overscanHorizontal = settings["Video/Overscan/Horizontal"].natural();
    uint overscanVertical = settings["Video/Overscan/Vertical"].natural();
    auto information = emulator->videoInformation();
    overscanHorizontal *= information.internalWidth / information.width;
    overscanVertical *= information.internalHeight / information.height;
    data += overscanVertical * pitch + overscanHorizontal;
    width -= overscanHorizontal * 2;
    height -= overscanVertical * 2;
  }

  if(video->lock(output, length, width, height)) {
    length >>= 2;

    for(auto y : range(height)) {
      memory::copy(output + y * length, data + y * pitch, width * sizeof(uint32));
    }

    video->unlock();
    video->output();
  }

  static uint frameCounter = 0;
  static uint64 previous, current;
  frameCounter++;

  current = chrono::timestamp();
  if(current != previous) {
    previous = current;
    statusText = {"FPS: ", frameCounter};
    frameCounter = 0;
  }
}

auto Program::audioSample(const double* samples, uint channels) -> void {
  audio->output(samples);
}

auto Program::inputPoll(uint port, uint device, uint input) -> int16 {
  if(focused() || settings["Input/FocusLoss/AllowInput"].boolean()) {
    inputManager->poll();
    if(auto mapping = inputManager->mapping(port, device, input)) {
      return mapping->poll();
    }
  }
  return 0;
}

auto Program::inputRumble(uint port, uint device, uint input, bool enable) -> void {
}

auto Program::dipSettings(Markup::Node node) -> uint {
  return 0;
}

auto Program::notify(string text) -> void {
}
