unique_pointer<PathSettingsManager> pathSettingsManager;

PathSettingsManager::PathSettingsManager() {
  pathSettingsManager = this;

  layout.setMargin(5);

  saveLabel.setText("Saves:");
  saveEdit.setText(settings["Path/Save"].text()).onChange([&] {
    settings["Path/Save"].setValue(saveEdit.text().replace("\\", "/").trimRight("/").append("/"));
  });
  saveBrowse.setText("...").onActivate([&] {
    settings["Path/Save"].setValue(BrowserDialog()
    .setParent(*this)
    .setPath(Location::dir(settings["Path/Save"].value()))
    .setTitle("Browse for Saves Folder")
    .selectFolder()
    );
  });

  stateLabel.setText("States:");
  stateEdit.setText(settings["Path/State"].text()).onChange([&] {
    settings["Path/State"].setValue(stateEdit.text().replace("\\", "/").trimRight("/").append("/"));
  });
  stateBrowse.setText("...").onActivate([&] {
    settings["Path/State"].setValue(BrowserDialog()
    .setParent(*this)
    .setPath(Location::dir(settings["Path/State"].value()))
    .setTitle("Browse for States Folder")
    .selectFolder()
    );
  });

  cheatLabel.setText("Cheats:");
  cheatEdit.setText(settings["Path/Cheat"].text()).onChange([&] {
    settings["Path/Cheat"].setValue(cheatEdit.text().replace("\\", "/").trimRight("/").append("/"));
  });
  cheatBrowse.setText("...").onActivate([&] {
    settings["Path/Cheat"].setValue(BrowserDialog()
    .setParent(*this)
    .setPath(Location::dir(settings["Path/Cheat"].value()))
    .setTitle("Browse for Cheats Folder")
    .selectFolder()
    );
  });

  setTitle("Paths");
  setSize({600, 105});
  setAlignment({0.5, 0.5});
  setDismissable();
}

auto PathSettingsManager::show() -> void {
  setVisible();
  setFocused();
  doSize();
}
