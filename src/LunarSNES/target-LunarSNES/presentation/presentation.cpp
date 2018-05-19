#include "../LunarSNES.hpp"
#include "effects.cpp"
#include "about.cpp"
unique_pointer<AboutWindow> aboutWindow;
unique_pointer<Presentation> presentation;

Presentation::Presentation() {
  presentation = this;

  gameMenu.setText("Game");
  loadMedium.setText("Load").onActivate([=] {
    program->loadMedium(emulator->media[0]);
  });
  resetSystem.setText("Soft Reset").onActivate([&] { program->softReset(); });
  powerSystem.setText("Power Cycle").onActivate([&] { program->powerCycle(); });
  unloadSystem.setText("Unload").onActivate([&] { program->unloadMedium(); });
  saveQuickStateMenu.setText("Save Quick State").setEnabled(false);
  saveSlot1.setText("Slot 1").onActivate([&] { program->saveState(1); });
  saveSlot2.setText("Slot 2").onActivate([&] { program->saveState(2); });
  saveSlot3.setText("Slot 3").onActivate([&] { program->saveState(3); });
  saveSlot4.setText("Slot 4").onActivate([&] { program->saveState(4); });
  saveSlot5.setText("Slot 5").onActivate([&] { program->saveState(5); });
  loadQuickStateMenu.setText("Load Quick State").setEnabled(false);
  loadSlot1.setText("Slot 1").onActivate([&] { program->loadState(1); });
  loadSlot2.setText("Slot 2").onActivate([&] { program->loadState(2); });
  loadSlot3.setText("Slot 3").onActivate([&] { program->loadState(3); });
  loadSlot4.setText("Slot 4").onActivate([&] { program->loadState(4); });
  loadSlot5.setText("Slot 5").onActivate([&] { program->loadState(5); });
  stateManager.setText("State Manager ...").setEnabled(false).onActivate([&] { ::stateManager->show(); });

  configMenu.setText("Config");
  showInputSettings.setText("Input").onActivate([&] { inputSettingsManager->show(); });
  devices.setText("Devices").onActivate([&] { deviceSelector->show(); });
  videoScaleMenu.setText("Video Scale");
  videoScaleSmall.setText("Small").onActivate([&] {
    settings["Video/Windowed/Scale"].setValue("Small");
    resizeViewport();
  });
  videoScaleMedium.setText("Medium").onActivate([&] {
    settings["Video/Windowed/Scale"].setValue("Medium");
    resizeViewport();
  });
  videoScaleLarge.setText("Large").onActivate([&] {
    settings["Video/Windowed/Scale"].setValue("Large");
    resizeViewport();
  });
  videoEmulationMenu.setText("Video Emulation");
  blurEmulation.setText("Blurring").setChecked(settings["Video/BlurEmulation"].boolean()).onToggle([&] {
    settings["Video/BlurEmulation"].setValue(blurEmulation.checked());
    emulator->set("Blur Emulation", blurEmulation.checked());
  });
  colorEmulation.setText("Colors").setChecked(settings["Video/ColorEmulation"].boolean()).onToggle([&] {
    settings["Video/ColorEmulation"].setValue(colorEmulation.checked());
    emulator->set("Color Emulation", colorEmulation.checked());
  });
  scanlineEmulation.setText("Scanlines").setChecked(settings["Video/ScanlineEmulation"].boolean()).setVisible(false).onToggle([&] {
    settings["Video/ScanlineEmulation"].setValue(scanlineEmulation.checked());
    emulator->set("Scanline Emulation", scanlineEmulation.checked());
  });
  videoShaderMenu.setText("Video Shader");
  videoShaderNone.setText("None").onActivate([&] {
    settings["Video/Shader"].setValue("None");
    program->updateVideoShader();
  });
  videoShaderBlur.setText("Blur").onActivate([&] {
    settings["Video/Shader"].setValue("Blur");
    program->updateVideoShader();
  });
  loadShaders();
  synchronizeVideo.setText("Synchronize Video").setChecked(settings["Video/Synchronize"].boolean()).setVisible(false).onToggle([&] {
    settings["Video/Synchronize"].setValue(synchronizeVideo.checked());
    video->setBlocking(synchronizeVideo.checked());
  });
  synchronizeAudio.setText("Synchronize Audio").setChecked(settings["Audio/Synchronize"].boolean()).onToggle([&] {
    settings["Audio/Synchronize"].setValue(synchronizeAudio.checked());
    audio->setBlocking(synchronizeAudio.checked());
  });
  muteAudio.setText("Mute Audio").setChecked(settings["Audio/Mute"].boolean()).onToggle([&] {
    settings["Audio/Mute"].setValue(muteAudio.checked());
    program->updateAudioEffects();
  });
  showStatusBar.setText("Show Status Bar").setChecked(settings["UserInterface/ShowStatusBar"].boolean()).onToggle([&] {
    settings["UserInterface/ShowStatusBar"].setValue(showStatusBar.checked());
    statusBar.setVisible(showStatusBar.checked());
    if(visible()) resizeViewport();
  });
  showVideoSettings.setText("Video ...").onActivate([&] { videoSettingsManager->show(); });
  showAudioSettings.setText("Audio ...").onActivate([&] { audioSettingsManager->show(); });
  showPathSettings.setText("Paths ...").onActivate([&] { pathSettingsManager->show(); });

  cheatMenu.setText("Cheat");
  cheatEditor.setText("Editor ...").setEnabled(false).onActivate([&] { ::cheatEditor->show(); });

  miscMenu.setText("Misc");
  showHotkeySettings.setText("Hotkeys ...").onActivate([&] { hotkeySettingsManager->show(); });
  showAdvancedSettings.setText("Advanced ...").onActivate([&] { advancedSettingsManager->show(); });
  showManifestViewer.setText("Manifest Viewer ...").onActivate([&] { manifestViewer->show(); });
  about.setText("About ...").onActivate([&] {
    aboutWindow->setVisible().setFocused();
  });

  statusBar.setFont(Font().setBold());
  statusBar.setVisible(settings["UserInterface/ShowStatusBar"].boolean());

  viewport.setDroppable().onDrop([&](auto locations) {
    program->mediumQueue.append(locations(0));
    program->loadMedium();
  });

  onSize([&] {
    resizeViewport(false);
  });

  onClose([&] {
    program->quit();
  });

  setTitle({"Lunar SNES v", Emulator::Version});
  setBackgroundColor({0, 0, 0});
  resizeViewport();
  setCentered();

  #if defined(PLATFORM_WINDOWS)
  Application::Windows::onModalChange([](bool modal) { if(modal && audio) audio->clear(); });
  #endif

  #if defined(PLATFORM_MACOS)
  about.setVisible(false);
  Application::Cocoa::onAbout([&] { about.doActivate(); });
  Application::Cocoa::onActivate([&] { setFocused(); });
  Application::Cocoa::onPreferences([&] { showInputSettings.doActivate(); });
  Application::Cocoa::onQuit([&] { doClose(); });
  #endif
}

auto Presentation::updateEmulator() -> void {
  emulator->set("Blur Emulation", blurEmulation.checked());
  emulator->set("Color Emulation", colorEmulation.checked());
  emulator->set("Scanline Emulation", scanlineEmulation.checked());
}

auto Presentation::clearViewport() -> void {
  if(!video) return;

  uint32_t* output;
  uint length;
  uint width = viewport.geometry().width();
  uint height = viewport.geometry().height();
  if(video->lock(output, length, width, height)) {
    for(uint y : range(height)) {
      auto dp = output + y * (length >> 2);
      for(uint x : range(width)) *dp++ = 0xff000000;
    }

    video->unlock();
    video->output();
  }
}

auto Presentation::resizeViewport(bool resizeWindow) -> void {
  //clear video area before resizing to avoid seeing distorted video momentarily
  clearViewport();

  uint viewportWidth = geometry().width();
  uint viewportHeight = geometry().height();

  double emulatorWidth = 320;
  double emulatorHeight = 240;
  double aspectCorrection = 1.0;
  auto information = emulator->videoInformation();
  emulatorWidth = information.width;
  emulatorHeight = information.height;
  aspectCorrection = information.aspectCorrection;
  if(emulator->information.overscan) {
    uint overscanHorizontal = settings["Video/Overscan/Horizontal"].natural();
    uint overscanVertical = settings["Video/Overscan/Vertical"].natural();
    emulatorWidth -= overscanHorizontal * 2;
    emulatorHeight -= overscanVertical * 2;
  }

  if(!fullScreen()) {
    if(settings["Video/Windowed/AspectCorrection"].boolean()) emulatorWidth *= aspectCorrection;

    if(resizeWindow) {
      if(settings["Video/Windowed/Scale"].text() == "Small") viewportHeight = 480;
      if(settings["Video/Windowed/Scale"].text() == "Medium") viewportHeight = 720;
      if(settings["Video/Windowed/Scale"].text() == "Large") viewportHeight = 960;
      viewportWidth = viewportHeight * 4 / 3;
    }

    if(resizeWindow) {
      uint multiplier = min(viewportWidth / emulatorWidth, viewportHeight / emulatorHeight);
      emulatorWidth *= multiplier;
      emulatorHeight *= multiplier;
      setSize({viewportWidth = emulatorWidth, viewportHeight = emulatorHeight});
    } else if(settings["Video/Windowed/IntegralScaling"].boolean()) {
      uint multiplier = min(viewportWidth / emulatorWidth, viewportHeight / emulatorHeight);
      emulatorWidth *= multiplier;
      emulatorHeight *= multiplier;
    } else {
      double multiplier = min(viewportWidth / emulatorWidth, viewportHeight / emulatorHeight);
      emulatorWidth *= multiplier;
      emulatorHeight *= multiplier;
    }
  } else {
    if(settings["Video/Fullscreen/AspectCorrection"].boolean()) emulatorWidth *= aspectCorrection;

    if(settings["Video/Fullscreen/IntegralScaling"].boolean()) {
      uint multiplier = min(viewportWidth / emulatorWidth, viewportHeight / emulatorHeight);
      emulatorWidth *= multiplier;
      emulatorHeight *= multiplier;
    } else {
      double multiplier = min(viewportWidth / emulatorWidth, viewportHeight / emulatorHeight);
      emulatorWidth *= multiplier;
      emulatorHeight *= multiplier;
    }
  }

  viewport.setGeometry({
    (viewportWidth - emulatorWidth) / 2, (viewportHeight - emulatorHeight) / 2,
    emulatorWidth, emulatorHeight
  });

  //clear video area again to ensure entire viewport area has been painted in
  clearViewport();
}

auto Presentation::toggleFullScreen() -> void {
  if(!fullScreen()) {
    statusBar.setVisible(false);
    menuBar.setVisible(false);
    setFullScreen(true);
    video->setExclusive(settings["Video/Fullscreen/Exclusive"].boolean());
    if(video->exclusive()) setVisible(false);
    if(!input->acquired()) input->acquire();
  } else {
    if(input->acquired()) input->release();
    if(video->exclusive()) setVisible(true);
    video->setExclusive(false);
    setFullScreen(false);
    menuBar.setVisible(true);
    statusBar.setVisible(settings["UserInterface/ShowStatusBar"].boolean());
  }
  resizeViewport();
}

auto Presentation::loadShaders() -> void {
  auto pathname = locate("Video Shaders/");

  if(settings["Video/Driver"].text() == "OpenGL") {
    for(auto shader : directory::folders(pathname, "*.shader")) {
      if(videoShaders.objectCount() == 2) videoShaderMenu.append(MenuSeparator());
      MenuRadioItem item{&videoShaderMenu};
      item.setText(string{shader}.trimRight(".shader/", 1L)).onActivate([=] {
        settings["Video/Shader"].setValue({pathname, shader});
        program->updateVideoShader();
      });
      videoShaders.append(item);
    }
  }

  if(settings["Video/Shader"].text() == "None") videoShaderNone.setChecked();
  if(settings["Video/Shader"].text() == "Blur") videoShaderBlur.setChecked();

  for(auto radioItem : videoShaders.objects<MenuRadioItem>()) {
    if(settings["Video/Shader"].text() == string{pathname, radioItem.text(), ".shader/"}) {
      radioItem.setChecked();
    }
  }
}

//Lunar SNES: Change icon when activating the Chelsea easter egg
auto Presentation::trackKeystrokes() -> void {
  const string sequence = "Chelsea";

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

  if(emulator->loaded() && focused() && keystrokes < sequence.size()) {
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
      self().setIcon(image(Embed::LunarSNES::Chelsea32), image(Embed::LunarSNES::Chelsea16));
      program->showMessage("Chelsea is *really* cute!");
    }
  }
}
