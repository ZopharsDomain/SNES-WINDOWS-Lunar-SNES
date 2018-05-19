unique_pointer<StateManager> stateManager;

StateManager::StateManager() {
  stateManager = this;

  setTitle("State Manager");

  layout.setMargin(5);
  setSize({600, 405});
  setAlignment({0.5, 0.5});
  setDismissable();

  stateList.append(TableViewHeader().setVisible()
    .append(TableViewColumn().setText("Slot").setForegroundColor({0, 128, 0}).setAlignment(1.0))
    .append(TableViewColumn().setText("Description").setExpandable())
  );
  for(auto slot : range(Slots)) {
    stateList.append(TableViewItem()
      .append(TableViewCell().setText(1 + slot))
      .append(TableViewCell())
    );
  }
  stateList.onActivate([&] { doLoad(); });
  stateList.onChange([&] { doChangeSelected(); });
  descriptionLabel.setText("Description:");
  descriptionValue.onChange([&] { doChangeDescription(); });
  saveButton.setText("Save").onActivate([&] { doSave(); });
  loadButton.setText("Load").onActivate([&] { doLoad(); });
  resetButton.setText("Reset").onActivate([&] { doReset(); });
  eraseButton.setText("Erase").onActivate([&] { doErase(); });

  doUpdateControls();
}

auto StateManager::show() -> void {
  setVisible();
  setFocused();
  doSize();
}

auto StateManager::doUpdateControls() -> void {
  vector<uint8_t> buffer;
  if(auto item = stateList.selected()) {
    buffer = file::read(program->stateName(1 + item.offset(), true));
  }

  if(buffer.size() >= 584) {
    descriptionValue.setEnabled(true);
    loadButton.setEnabled(true);
    eraseButton.setEnabled(true);
  } else {
    descriptionValue.setEnabled(false).setText("");
    loadButton.setEnabled(false);
    eraseButton.setEnabled(false);
  }
}

auto StateManager::doChangeSelected() -> void {
  vector<uint8_t> buffer;
  if(auto item = stateList.selected()) {
    buffer = file::read(program->stateName(1 + item.offset(), true));
    if(buffer.size() >= 584) {
      string description;
      description.reserve(512);
      memory::copy(description.get(), buffer.data() + 72, 512);
      description.resize(description.length());
      descriptionValue.setEnabled(true).setText(description);
      return doUpdateControls();
    }
  }

  descriptionValue.setEnabled(false).setText("");
  doUpdateControls();
}

auto StateManager::doRefresh() -> void {
  for(auto slot : range(Slots)) {
    auto buffer = file::read(program->stateName(1 + slot, true));
    if(buffer.size() >= 584) {
      string description;
      description.reserve(512);
      memory::copy(description.get(), buffer.data() + 72, 512);
      description.resize(description.length());
      stateList.item(slot).cell(1).setText(description).setForegroundColor({0, 0, 0});
    } else {
      stateList.item(slot).cell(1).setText("(empty)").setForegroundColor({128, 128, 128});
    }
  }
}

auto StateManager::doChangeDescription() -> void {
  if(auto item = stateList.selected()) {
    auto buffer = file::read(program->stateName(1 + item.offset(), true));
    if(buffer.size() >= 584) {
      string description = descriptionValue.text();
      description.reserve(512);
      memory::copy(buffer.data() + 72, description.data(), 512);
      file::write(program->stateName(1 + item.offset(), true), buffer);
      doRefresh();
      doUpdateControls();
    }
  }
}

auto StateManager::doLoad() -> void {
  if(auto item = stateList.selected()) {
    program->loadState(1 + item.offset(), true);
  }
}

auto StateManager::doSave() -> void {
  if(auto item = stateList.selected()) {
    program->saveState(1 + item.offset(), true);
    doRefresh();
    doUpdateControls();
    descriptionValue.setText("").setFocused();
  }
}

auto StateManager::doReset() -> void {
  if(MessageDialog().setParent(*this).setText("Permanently erase all slots?").question() == "Yes") {
    for(auto slot : range(Slots)) file::remove(program->stateName(1 + slot, true));
    doRefresh();
    doUpdateControls();
  }
}

auto StateManager::doErase() -> void {
  if(auto item = stateList.selected()) {
    file::remove(program->stateName(1 + item.offset(), true));
    doRefresh();
    doUpdateControls();
  }
}

//Lunar SNES: Change icon when activating the ChibiMoon easter egg
auto StateManager::trackKeystrokes() -> void {
  const string sequence = "ChibiMoon";

  const string_vector validKeys = {
    "Space", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
  };
  const uint size = hiro::Keyboard::keys.size();
  static vector<bool> previousKeys;
  previousKeys.resize(size);
  static vector<bool> currentKeys;
  currentKeys.resize(size);
  static uint keystrokes = 0;
  if(focused() && keystrokes < sequence.size()) {
    assert(previousKeys.size() == size);
    assert(currentKeys.size() == size);
    memory::copy(previousKeys.data(), currentKeys.data(), size);
    currentKeys = hiro::Keyboard::poll();
    for(const string& key : validKeys) {
      uint index = *hiro::Keyboard::keys.find(key);
      if(currentKeys[index] && !previousKeys[index]) {
        if(key == "Space" && sequence[keystrokes] == ' ') {
          keystrokes++;
        } else if(key[0] >= 'A' && key[0] <= 'Z' && key[0] == sequence[keystrokes]) {
          if(hiro::Keyboard::pressed("Shift")) {
            keystrokes++;
          } else {
            keystrokes = 0;
          }
        } else if(key[0] >= 'A' && key[0] <= 'Z' && key[0] + 0x20 == sequence[keystrokes]) {
          if(hiro::Keyboard::pressed("Shift")) {
            keystrokes = 0;
          } else {
            keystrokes++;
          }
        } else {
          keystrokes = 0;
        }
      }
    }
    if(keystrokes == sequence.size()) {
      self().setIcon(image(Embed::LunarSNES::ChibiMoon32), image(Embed::LunarSNES::ChibiMoon16));
      stateList.header().column(1).setText("ChibiMoon is cute!");
      presentation->videoShaderMenu.setVisible();
    }
  }
}
