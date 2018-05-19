unique_pointer<InputSettingsManager> inputSettingsManager;

InputSettingsTab::InputSettingsTab(TabFrame* parent, uint portID) : TabFrameItem(parent), portID(portID) {
  setIcon(Icon::Device::Joypad);
  setText({"Port ", portID + 1});

  layout.setMargin(5);
  mappingList.onActivate([&] { inputSettingsManager->assignMapping(); });
  mappingList.onChange([&] { inputSettingsManager->updateControls(); });
}

auto InputSettingsTab::activePort() -> InputPort& {
  return inputManager->ports[portID < 1 ? SFC::ID::Port::Controller1 : SFC::ID::Port::Controller2];
}

auto InputSettingsTab::activeDevice() -> InputDevice& {
  for(auto& device : activePort().devices) {
    if(device.id == (portID < 2 ? SFC::ID::Device::Gamepad : SFC::ID::Device::SuperMultitap)) return device;
  }
}

auto InputSettingsTab::reloadMappings() -> void {
  inputSettingsManager->eraseButton.setEnabled(false);
  mappingList.reset();
  mappingList.append(TableViewHeader().setVisible()
    .append(TableViewColumn().setText("Name"))
    .append(TableViewColumn().setText("Mapping").setExpandable())
  );
  for(uint position : range(12)) {
    string name = activeDevice().mappings[position].name.split(" - ").right();
    mappingList.append(TableViewItem()
      .append(TableViewCell().setText(name))
      .append(TableViewCell())
    );
  }
  refreshMappings();
}

auto InputSettingsTab::refreshMappings() -> void {
  for(uint position : range(12)) {
    auto& mapping = activeDevice().mappings[portID < 2 ? position : position + (portID - 1) * 12];
    mappingList.item(position).cell(1).setText(mapping.displayName());
  }
  mappingList.resizeColumns();
}

InputSettingsManager::InputSettingsManager() {
  inputSettingsManager = this;

  layout.setMargin(5);
  statusBar.setFont(Font().setBold());

  focusLabel.setText("When Focus is Lost:");
  pauseEmulation.setText("Pause Emulation").setChecked(settings["Input/FocusLoss/Pause"].boolean()).onToggle([&] {
    settings["Input/FocusLoss/Pause"].setValue(pauseEmulation.checked());
    allowInput.setEnabled(!pauseEmulation.checked());
  }).doToggle();
  allowInput.setText("Allow Input").setChecked(settings["Input/FocusLoss/AllowInput"].boolean()).onToggle([&] {
    settings["Input/FocusLoss/AllowInput"].setValue(allowInput.checked());
  });
  resetButton.setText("Reset").onActivate([&] {
    if(MessageDialog("Are you sure you want to erase all mappings for this device?").setParent(*this).question() == "Yes") {
      for(auto& mapping : activeDevice().mappings) mapping.unbind();
      refreshMappings();
    }
  });
  eraseButton.setText("Erase").onActivate([&] {
    if(auto mapping = activeTab().mappingList.selected()) {
      activeDevice().mappings[mapping.offset()].unbind();
      refreshMappings();
    }
  });

  onSize([&] {
    port1.mappingList.resizeColumns();
    port2.mappingList.resizeColumns();
    port3.mappingList.resizeColumns();
    port4.mappingList.resizeColumns();
    port5.mappingList.resizeColumns();
  });

  setTitle("Input Device");
  setSize({600, 405});
  setAlignment({0.5, 0.5});
  setDismissable();

  reloadMappings();
}

auto InputSettingsManager::setVisible(bool visible) -> InputSettingsManager& {
  if(visible) {
    refreshMappings();
  }
  Window::setVisible(visible);
  return *this;
}

auto InputSettingsManager::show() -> void {
  setVisible();
  setFocused();
  doSize();
}

auto InputSettingsManager::updateControls() -> void {
  eraseButton.setEnabled((bool)activeTab().mappingList.selected());

  if(auto mapping = activeTab().mappingList.selected()) {
    auto& input = activeDevice().mappings[mapping.offset()];
  }
}

auto InputSettingsManager::activeTab() -> InputSettingsTab& {
  if(port1.selected()) return port1;
  if(port2.selected()) return port2;
  if(port3.selected()) return port3;
  if(port4.selected()) return port4;
  if(port5.selected()) return port5;
}

auto InputSettingsManager::activePort() -> InputPort& {
  return activeTab().activePort();
}

auto InputSettingsManager::activeDevice() -> InputDevice& {
  return activeTab().activeDevice();
}

auto InputSettingsManager::reloadMappings() -> void {
  port1.reloadMappings();
  port2.reloadMappings();
  port3.reloadMappings();
  port4.reloadMappings();
  port5.reloadMappings();
}

auto InputSettingsManager::refreshMappings() -> void {
  port1.refreshMappings();
  port2.refreshMappings();
  port3.refreshMappings();
  port4.refreshMappings();
  port5.refreshMappings();
}

auto InputSettingsManager::assignMapping() -> void {
  inputManager->poll();  //clear any pending events first

  if(auto mapping = activeTab().mappingList.selected()) {
    uint portID = activeTab().portID;
    activeMapping = &activeDevice().mappings[portID < 2 ? mapping.offset() : mapping.offset() + (portID - 1) * 12];
    layout.setEnabled(false);
    statusBar.setText({"Press a key or button to map [", activeMapping->name, "] ..."});
  }
}

auto InputSettingsManager::inputEvent(shared_pointer<HID::Device> device, uint group, uint input, int16 oldValue, int16 newValue) -> void {
  if(!activeMapping) return;
  if(device->isMouse()) return;

  if(activeMapping->bind(device, group, input, oldValue, newValue)) {
    activeMapping = nullptr;
    statusBar.setText("Mapping assigned.");
    //Copy port 2 Gamepad mappings to Super Multitap port 2
    for(uint index : range(12)) {
      const auto& gamepad = inputManager->ports[SFC::ID::Port::Controller2].devices[SFC::ID::Device::Gamepad];
      auto& multitap = inputManager->ports[SFC::ID::Port::Controller2].devices[SFC::ID::Device::SuperMultitap];
      multitap.mappings[index].bind(gamepad.mappings[index].assignment);
    }
    refreshMappings();
    timer.onActivate([&] {
      timer.setEnabled(false);
      statusBar.setText();
      layout.setEnabled();
    }).setInterval(200).setEnabled();
  }
}
