auto Icarus::create(const string& pathname) -> bool {
  if(gamepak) return directory::create(pathname);
  return true;
}

auto Icarus::read(const string& path) -> vector<uint8_t> {  //change by the libretro team
  if(gamepak) return file::read(path);
  if(path.endsWith(".rom")) {
    if(auto output = select(path)) {
      if(auto index = output->filenames.find(Location::file(path))) {
        uint offset = output->offsets[index()];
        uint size   = output->sizes[index()];
        vector<uint8_t> section;
        section.resize(size);
        memory::copy(section.data(), output->buffer.data() + offset, size);
        return section;
      } else {
        return {};
      }
    }
  }
  return file::read(filename(path));
}

auto Icarus::exists(const string& path) -> bool {
  return file::exists(filename(path));
}

auto Icarus::directory_exists(const string& path) -> bool {  //change by the libretro team
  if(gamepak) return directory::exists(filename(path));
  return true;
}

auto Icarus::copy(const string& source, const string& target) -> bool {
  if(gamepak) return file::copy(source, target);
  return false;  //return file::copy(source, filename(target));
}

auto Icarus::write(const string& path, const uint8_t* data, uint size) -> bool {
  if(gamepak) return file::write(path, data, size);
  if(Location::file(path) == "manifest.bml") {
    if(auto output = select(path)) {
      output->manifest.resize(size);
      memory::copy((uint8_t*)output->manifest.data(), data, size);
    }
  } else if(path.endsWith(".rom")) {
    if(auto output = select(path)) {
      for(uint index : range(output->buffer.size() - size + 1)) {
        if(memory::compare(output->buffer.data() + index, data, size) == 0) {
          output->filenames.append(Location::file(path));
          output->offsets.append(index);
          output->sizes.append(size);
          break;
        }
      }
    }
  }
  return true;
}
