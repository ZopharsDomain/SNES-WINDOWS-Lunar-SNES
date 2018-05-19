#include "../LunarSNES.hpp"
#include "interface.cpp"
#include "medium.cpp"
#include "state.cpp"
#include "utility.cpp"
unique_pointer<Program> program;

Program::Program(string_vector args) {
  program = this;

  new Icarus;

  Emulator::platform = this;
  emulator = new SuperFamicom::Interface;

  new Presentation;
  presentation->setVisible();

  if(settings["Crashed"].boolean()) {
    MessageDialog().setText("Driver crash detected. Video/Audio/Input drivers have been disabled.").information();
    settings["Video/Driver"].setValue("None");
    settings["Audio/Driver"].setValue("None");
    settings["Input/Driver"].setValue("None");
  }

  settings["Crashed"].setValue(true);
  settings.save();

  initializeVideoDriver();
  initializeAudioDriver();
  initializeInputDriver();

  settings["Crashed"].setValue(false);
  settings.save();

  new InputManager;
  new VideoSettingsManager;
  new AudioSettingsManager;
  new InputSettingsManager;
  new HotkeySettingsManager;
  new AdvancedSettingsManager;
  new PathSettingsManager;
  new DeviceSelector;
  new CheatEditor;
  new StateManager;
  new ManifestViewer;
  new AboutWindow;

  updateVideoShader();
  updateAudioDriver();
  updateAudioEffects();

  args.takeLeft();  //ignore program location in argument parsing
  for(auto& argument : args) {
    if(argument == "--fullscreen") {
      presentation->toggleFullScreen();
    } else if(inode::exists(argument.split("|", 1L).right())) {
      if(!argument.transform("\\", "/").endsWith("/")) argument.append("/");
      mediumQueue.append(argument);
    }
  }
  loadMedium();

  Application::onMain({&Program::main, this});
}

auto Program::main() -> void {
  updateStatusText();
  inputManager->poll();
  inputManager->pollHotkeys();

  //Lunar SNES: track keystrokes for easter eggs of FuSoYa's waifus
  if(presentation->focused()) presentation->trackKeystrokes();
  if(stateManager->focused()) stateManager->trackKeystrokes();
  if(cheatEditor->focused()) cheatEditor->trackKeystrokes();

  if(!emulator->loaded() || pause || (!focused() && settings["Input/FocusLoss/Pause"].boolean())) {
    audio->clear();
    presentation->effect();
  //usleep(20 * 1000);
    usleep(1'000'000 / 60);
    return;
  }

  emulator->run();
  if(settings["Emulation/AutoSaveMemory/Enable"].boolean()) {
    time_t currentTime = time(nullptr);
    if(currentTime - autoSaveTime >= settings["Emulation/AutoSaveMemory/Interval"].natural()) {
      autoSaveTime = currentTime;
      emulator->save();
    }
  }
}

auto Program::quit() -> void {
  hasQuit = true;
  unloadMedium();
  settings.save();
  inputManager->quit();
  video.reset();
  audio.reset();
  input.reset();
  Application::quit();
}
