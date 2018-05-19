auto Program::stateName(uint slot, bool managed) -> string {
  return {
    settings["Path/State"].text(), Location::prefix(mediumPaths(1)), ".l",
    slot < 10 ? "s" : pad(slot / 10),
    slot == 0 ? "t" : pad(slot % 10)
  };
}

auto Program::loadState(uint slot, bool managed) -> bool {
  if(!emulator->loaded()) return false;
  string type = managed ? "managed" : "quick";
  auto location = stateName(slot, managed);
  auto memory = file::read(location);
  if(memory.size() == 0) return showMessage({"Slot ", slot, " ", type, " state does not exist"}), false;
  serializer s(memory.data(), memory.size());
  if(emulator->unserialize(s) == false) return showMessage({"Slot ", slot, " ", type, " state incompatible"}), false;
  return showMessage({"Loaded ", type, " state from slot ", slot}), true;
}

auto Program::saveState(uint slot, bool managed) -> bool {
  if(!emulator->loaded()) return false;
  string type = managed ? "managed" : "quick";
  auto location = stateName(slot, managed);
  directory::create(Location::dir(location));
  serializer s = emulator->serialize();
  if(s.size() == 0) return showMessage({"Failed to save ", type, " state to slot ", slot}), false;
  directory::create(Location::path(location));
  if(file::write(location, s.data(), s.size()) == false) {
    return showMessage({"Unable to write ", type, " state to slot ", slot}), false;
  }
  return showMessage({"Saved ", type, " state to slot ", slot}), true;
}
