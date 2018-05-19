unique_pointer<HotkeySettingsManager> hotkeySettingsManager;

HotkeySettingsManager::HotkeySettingsManager() {
  hotkeySettingsManager = this;

  layout.setMargin(5);
  statusBar.setFont(Font().setBold());

  mappingList.onActivate([&] { assignMapping(); });
  mappingList.onChange([&] {
    eraseButton.setEnabled((bool)mappingList.selected());
  });
  resetButton.setText("Reset").onActivate([&] {
    if(MessageDialog("Are you sure you want to erase all hotkey mappings?").setParent(*this).question() == "Yes") {
      for(auto& mapping : inputManager->hotkeys) mapping->unbind();
      refreshMappings();
    }
  });
  eraseButton.setText("Erase").onActivate([&] {
    if(auto item = mappingList.selected()) {
      inputManager->hotkeys[item.offset()]->unbind();
      refreshMappings();
    }
  });

  setTitle("Hotkeys");
  setSize({600, 405});
  setAlignment({0.5, 0.5});
  setDismissable();

  reloadMappings();
  refreshMappings();
}

auto HotkeySettingsManager::setVisible(bool visible) -> HotkeySettingsManager& {
  if(visible) {
    refreshMappings();
  }
  Window::setVisible(visible);
  return *this;
}

auto HotkeySettingsManager::show() -> void {
  setVisible();
  setFocused();
  doSize();
}

auto HotkeySettingsManager::reloadMappings() -> void {
  mappingList.reset();
  mappingList.append(TableViewHeader().setVisible()
    .append(TableViewColumn().setText("Name"))
    .append(TableViewColumn().setText("Mapping").setExpandable())
  );
  for(auto& hotkey : inputManager->hotkeys) {
    mappingList.append(TableViewItem()
      .append(TableViewCell().setText(hotkey->name))
      .append(TableViewCell())
    );
  }
  mappingList.resizeColumns();
}

auto HotkeySettingsManager::refreshMappings() -> void {
  uint position = 0;
  for(auto& hotkey : inputManager->hotkeys) {
    mappingList.item(position).cell(1).setText(hotkey->displayName());
    position++;
  }
  mappingList.resizeColumns();
}

auto HotkeySettingsManager::assignMapping() -> void {
  inputManager->poll();  //clear any pending events first

  if(auto item = mappingList.selected()) {
    activeMapping = inputManager->hotkeys[item.offset()];
    layout.setEnabled(false);
    statusBar.setText({"Press a key or button to map [", activeMapping->name, "] ..."});
  }
}

auto HotkeySettingsManager::inputEvent(shared_pointer<HID::Device> device, uint group, uint input, int16 oldValue, int16 newValue) -> void {
  if(!activeMapping) return;
  if(device->isMouse()) return;

  if(activeMapping->bind(device, group, input, oldValue, newValue)) {
    activeMapping = nullptr;
    statusBar.setText("Mapping assigned.");
    refreshMappings();
    timer.onActivate([&] {
      timer.setEnabled(false);
      statusBar.setText();
      layout.setEnabled();
    }).setInterval(200).setEnabled();
  }
}
