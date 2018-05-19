#include "LunarSNES.hpp"
unique_pointer<Video> video;
unique_pointer<Audio> audio;
unique_pointer<Input> input;
Emulator::Interface* emulator = nullptr;

auto locate(string name) -> string {
  return {Path::program(), name};
  /*
  string location = {Path::program(), name};
  if(inode::exists(location)) return location;

  location = {Path::config(), "LunarSNES/", name};
  if(inode::exists(location)) return location;

  directory::create({Path::local(), "LunarSNES/"});
  return {Path::local(), "LunarSNES/", name};
  */
}

#include <nall/main.hpp>
auto nall::main(string_vector args) -> void {
  Application::setName("Lunar SNES");
  new Program(args);
  Application::run();
}
