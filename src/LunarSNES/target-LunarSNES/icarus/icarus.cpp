#include "../LunarSNES.hpp"
unique_pointer<Icarus> icarus;

#include <icarus/heuristics/super-famicom.cpp>
#include <icarus/heuristics/game-boy.cpp>
#include <icarus/heuristics/bs-memory.cpp>
#include <icarus/heuristics/sufami-turbo.cpp>

#include "shim.cpp"
#include "core.cpp"
#include <icarus/core/super-famicom.cpp>
#include <icarus/core/game-boy.cpp>
#include <icarus/core/bs-memory.cpp>
#include <icarus/core/sufami-turbo.cpp>

auto Icarus::Output::reset() -> void {
  buffer.reset();
  filenames.reset();
  offsets.reset();
  sizes.reset();
}

Icarus::Icarus() {
  icarus = this;

  sfc.buffer.reserve(16 * 1024 * 1024);
  gb.buffer.reserve(4 * 1024 * 1024);
  bs.buffer.reserve(1 * 1024 * 1024);
  st.buffer.reserve(1 * 1024 * 1024);

  settings = BML::unserialize(
    "icarus\n"
    "  UseDatabase=true\n"
    "  UseHeuristics=true\n"
    "  CreateManifests=true\n"
  );
}

auto Icarus::extension(string extension) -> string {
  #if defined(PLATFORM_WINDOWS)
  extension.downcase();
  #endif
  if(extension == "smc"
  || extension == "swc"
  || extension == "fig"
  || extension == "dx2"
  || extension == "ufo"
  || extension == "gd3"
  || extension == "gd7"
  || extension == "mgd"
  || extension == "mgh"
  || extension == "048"
  || extension == "058"
  || extension == "078"
  || extension == "bin"
  || extension == "usa"
  || extension == "eur"
  || extension == "jap"
  || extension == "aus"
  ) return "sfc";
  return extension;
}

auto Icarus::select(const string& path) -> Output* {
  string raw_path = Location::path(path);
  string type = Location::suffix(raw_path).trimLeft(".");
  if(type == "zip") if(auto index = zipPaths.find(raw_path)) type = zipTypes[*index];
  type = extension(type);
  if(type == "sfc") return &sfc;
  if(type == "gb") return &gb;
  if(type == "bs") return &bs;
  if(type == "st") return &st;
  return nullptr;
}

auto Icarus::filename(const string& path) -> string {
  if(gamepak) return path;
  string raw_name = Location::file(path);
  string raw_path = Location::path(path).trimRight("/");
  if(raw_name == "msu1.rom") return {Location::prefix(raw_path), ".msu"};
  if(raw_name == "program.rom") return raw_path;
  if(raw_name.endsWith(".rom")) return "";
  //if(raw_name.match("track-*.pcm")) return string{raw_path}.replace(".sfc", raw_name.trimLeft("track"));
  if(raw_name == "save.ram") return {::settings["Path/Save"].text(), Location::prefix(raw_path), ".srm"};
  if(raw_name == "upd96050.data.ram") return {::settings["Path/Save"].text(), Location::prefix(raw_path), ".srm"};
  if(raw_name.endsWith("time.rtc")) return {::settings["Path/Save"].text(), Location::prefix(raw_path), ".rtc"};
  if(raw_name == "rtc.ram") return {::settings["Path/Save"].text(), Location::prefix(raw_path), ".rtc"};
  return "";
}

auto Icarus::offset(const string& path) -> uint {
  string raw_name = Location::file(path);
  if(raw_name == "msu1.rom") return 0;
  if(raw_name.endsWith(".rom")) {
    if(auto output = select(path)) {
      if(auto index = output->filenames.find(Location::file(path))) {
        return output->offsets[*index];
      }
    }
    return 0;
  }
  return 0;
}

auto Icarus::size(const string& path) -> uint {
  string raw_name = Location::file(path);
  if(raw_name == "msu1.rom") return file::size(filename(path));
  if(raw_name.endsWith(".rom")) {
    if(auto output = select(path)) {
      if(auto index = output->filenames.find(Location::file(path))) {
        return output->sizes[*index];
      }
    }
    return 0;
  }
  return file::size(filename(path));
}

auto Icarus::reset() -> void {
  sfc.reset();
  gb.reset();
  bs.reset();
  st.reset();
}
