unique_pointer<AdvancedSettingsManager> advancedSettingsManager;

AdvancedSettingsManager::AdvancedSettingsManager() {
  advancedSettingsManager = this;

  layout.setMargin(5);

  driverLabel.setText("Driver Selection").setFont(Font().setBold());
  videoLabel.setText("Video:");
  videoDriver.onChange([&] { settings["Video/Driver"].setValue(videoDriver.selected().text()); });
  for(auto& driver : Video::availableDrivers()) {
    ComboButtonItem item;
    item.setText(driver);
    videoDriver.append(item);
    if(settings["Video/Driver"].text() == driver) item.setSelected();
  }
  audioLabel.setText("Audio:");
  audioDriver.onChange([&] { settings["Audio/Driver"].setValue(audioDriver.selected().text()); });
  for(auto& driver : Audio::availableDrivers()) {
    ComboButtonItem item;
    item.setText(driver);
    audioDriver.append(item);
    if(settings["Audio/Driver"].text() == driver) item.setSelected();
  }
  inputLabel.setText("Input:");
  inputDriver.onChange([&] { settings["Input/Driver"].setValue(inputDriver.selected().text()); });
  for(auto& driver : Input::availableDrivers()) {
    ComboButtonItem item;
    item.setText(driver);
    inputDriver.append(item);
    if(settings["Input/Driver"].text() == driver) item.setSelected();
  }

  otherLabel.setText("Other").setFont(Font().setBold());
  autoSaveMemory.setText("Auto-Save Memory Periodically").setChecked(settings["Emulation/AutoSaveMemory/Enable"].boolean()).onToggle([&] {
    settings["Emulation/AutoSaveMemory/Enable"].setValue(autoSaveMemory.checked());
  });

  effectLabel.setText("BG Effects").setFont(Font().setBold());
  effectNone.setText("None").onActivate([&] { settings["UI/Effect"].setValue("None"); });
  effectSnow.setText("Snow").onActivate([&] { settings["UI/Effect"].setValue("Snow"); });
  effectWaterA.setText("Water A").onActivate([&] { settings["UI/Effect"].setValue("WaterA"); });
  effectWaterB.setText("Water B").onActivate([&] { settings["UI/Effect"].setValue("WaterB"); });
  effectBurning.setText("Burning").onActivate([&] { settings["UI/Effect"].setValue("Burning"); });
  effectSmoke.setText("Smoke").onActivate([&] { settings["UI/Effect"].setValue("Smoke"); });
  if(settings["UI/Effect"].text() == "None") effectNone.setChecked();
  if(settings["UI/Effect"].text() == "Snow") effectSnow.setChecked();
  if(settings["UI/Effect"].text() == "WaterA") effectWaterA.setChecked();
  if(settings["UI/Effect"].text() == "WaterB") effectWaterB.setChecked();
  if(settings["UI/Effect"].text() == "Burning") effectBurning.setChecked();
  if(settings["UI/Effect"].text() == "Smoke") effectSmoke.setChecked();

  setTitle("Advanced");
  setSize({600, 405});
  setAlignment({0.5, 0.5});
  setDismissable();
}

auto AdvancedSettingsManager::show() -> void {
  setVisible();
  setFocused();
  doSize();
}
