auto Program::loadMedium() -> void {
  if(!mediumQueue) return;

  string location = mediumQueue.left();
  string type = Location::suffix(location).trimLeft(".", 1L);
  if(type == "zip") {
    Decode::ZIP zip;
    if(zip.open(location.trimRight("/"))) {
      type = Location::suffix(zip.file[0].name).trimLeft(".", 1L);
      if(icarus->extension(type) == "sfc") type = "sfc";
    }
  } else {
    if(icarus->extension(type) == "sfc") type = "sfc";
  }
  for(auto& medium : emulator->media) {
    if(medium.type != type) continue;
    return loadMedium(medium);
  }

  mediumQueue.reset();
}

auto Program::loadMedium(const Emulator::Interface::Medium& medium) -> void {
  unloadMedium();

  mediumPaths.append(locate({medium.name, ".sys/"}));

  Emulator::audio.reset(2, audio->frequency());
  if(!emulator->load(medium.id)) {
    mediumPaths.reset();
    return;
  }
  updateAudioDriver();
  updateAudioEffects();
  connectDevices();
  emulator->power();

  presentation->resizeViewport();
  presentation->setTitle(emulator->title());
  presentation->saveQuickStateMenu.setEnabled(true);
  presentation->loadQuickStateMenu.setEnabled(true);
  presentation->stateManager.setEnabled(true);
  presentation->cheatEditor.setEnabled(true);
  presentation->updateEmulator();
  cheatEditor->loadCheats();
  stateManager->doRefresh();
  manifestViewer->doRefresh();
}

auto Program::unloadMedium() -> void {
  presentation->clearViewport();
  cheatEditor->saveCheats();
  emulator->unload();
  mediumPaths.reset();

  presentation->resizeViewport();
  presentation->setTitle({"Lunar SNES v", Emulator::Version});
  presentation->saveQuickStateMenu.setEnabled(false);
  presentation->loadQuickStateMenu.setEnabled(false);
  presentation->stateManager.setEnabled(false);
  presentation->cheatEditor.setEnabled(false);
  cheatEditor->setVisible(false);
  stateManager->setVisible(false);

  icarus->reset();
  presentation->videoBuffer = nullptr;
}
