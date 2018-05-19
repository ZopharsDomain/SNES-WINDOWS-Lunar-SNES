auto Icarus::error() const -> string {
  return errorMessage;
}

auto Icarus::missing() const -> string_vector {
  return missingFiles;
}

auto Icarus::success(string location) -> string {
  errorMessage = "";
  return location;
}

auto Icarus::failure(string message) -> string {
  errorMessage = message;
  return {};
}

auto Icarus::manifest(string location) -> string {
  location.transform("\\", "/").trimRight("/").append("/");
  if(!directory_exists(location)) return {};  //change by the libretro team

  auto type = extension(Location::suffix(location).downcase().trimLeft("."));
  if(type == "sfc") return superFamicomManifest(location);
  if(type == "gb") return gameBoyManifest(location);
  if(type == "bs") return bsMemoryManifest(location);
  if(type == "st") return sufamiTurboManifest(location);

  return "";
}

auto Icarus::import(string location) -> string {
  errorMessage = {};
  missingFiles = {};

  location.transform("\\", "/").trimRight("/");
  if(!file::exists(location)) return failure("file does not exist");
  if(!file::readable(location)) return failure("file is unreadable");

  auto name = Location::prefix(location);
  auto type = Location::suffix(location).downcase();
  if(!name || !type) return failure("invalid file name");

  vector<uint8_t> buffer;

  if(type == ".zip") {
    Decode::ZIP zip;
    if(!zip.open(location)) return failure("ZIP archive is invalid");
    if(!zip.file) return failure("ZIP archive is empty");

    name = Location::prefix(zip.file[0].name);
    type = Location::suffix(zip.file[0].name).downcase();
    buffer = zip.extract(zip.file[0]);
    zipPaths.append({location, "/"});
    zipTypes.append(string{type}.trimLeft("."));
  } else {
    buffer = file::read(location);
  }
  if(!buffer) return failure("file is empty");

  if(type == ".sfc" || type == ".smc") {
    sfc.buffer = buffer;
    return superFamicomImport(buffer, location);
  }
  if(type == ".gb") { gb.buffer = buffer; return gameBoyImport(buffer, location); }
  if(type == ".bs") { bs.buffer = buffer; return bsMemoryImport(buffer, location); }
  if(type == ".st") { st.buffer = buffer; return sufamiTurboImport(buffer, location); }

  return failure("unrecognized file extension");
}

auto Icarus::concatenate(vector<uint8_t>& output, string location) -> void {
  if(gamepak && !location.endsWith("program.rom")) return;
  if(auto input = read(location)) {  //change by the libretro team
    auto size = output.size();
    output.resize(size + input.size());
    memory::copy(output.data() + size, input.data(), input.size());
  }
}
