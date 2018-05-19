struct AboutWindow : Window {
  AboutWindow();

  VerticalLayout layout{this};
    Frame frame{&layout, Size{~0, ~0}};
      VerticalLayout frameLayout{&frame};
        Label information{&frameLayout, Size{~0, ~0}};
};

struct Presentation : Window {
  Presentation();
  auto updateEmulator() -> void;
  auto clearViewport() -> void;
  auto resizeViewport(bool resizeWindow = true) -> void;
  auto toggleFullScreen() -> void;
  auto loadShaders() -> void;
  auto effect() -> void;
  //Lunar SNES: track keystrokes for activating the "Chelsea" easter egg
  auto trackKeystrokes() -> void;

  MenuBar menuBar{this};
    Menu gameMenu{&menuBar};
      MenuItem loadMedium{&gameMenu};
      MenuItem resetSystem{&gameMenu};
      MenuItem powerSystem{&gameMenu};
      MenuItem unloadSystem{&gameMenu};
      MenuSeparator gameMenuSeparator{&gameMenu};
      Menu saveQuickStateMenu{&gameMenu};
        MenuItem saveSlot1{&saveQuickStateMenu};
        MenuItem saveSlot2{&saveQuickStateMenu};
        MenuItem saveSlot3{&saveQuickStateMenu};
        MenuItem saveSlot4{&saveQuickStateMenu};
        MenuItem saveSlot5{&saveQuickStateMenu};
      Menu loadQuickStateMenu{&gameMenu};
        MenuItem loadSlot1{&loadQuickStateMenu};
        MenuItem loadSlot2{&loadQuickStateMenu};
        MenuItem loadSlot3{&loadQuickStateMenu};
        MenuItem loadSlot4{&loadQuickStateMenu};
        MenuItem loadSlot5{&loadQuickStateMenu};
      MenuItem stateManager{&gameMenu};
    Menu configMenu{&menuBar};
      MenuItem showInputSettings{&configMenu};
      MenuSeparator configMenuSeparatorPorts{&configMenu};
      MenuItem devices{&configMenu};
      MenuSeparator configMenuSeparatorVideo{&configMenu};
      Menu videoScaleMenu{&configMenu};
        MenuItem videoScaleSmall{&videoScaleMenu};
        MenuItem videoScaleMedium{&videoScaleMenu};
        MenuItem videoScaleLarge{&videoScaleMenu};
      Menu videoEmulationMenu{&configMenu};
        MenuCheckItem blurEmulation{&videoEmulationMenu};
        MenuCheckItem colorEmulation{&videoEmulationMenu};
        MenuCheckItem scanlineEmulation{&videoEmulationMenu};
      Menu videoShaderMenu{&configMenu};
        MenuRadioItem videoShaderNone{&videoShaderMenu};
        MenuRadioItem videoShaderBlur{&videoShaderMenu};
        Group videoShaders{&videoShaderNone, &videoShaderBlur};
      MenuSeparator videoSettingsSeparator{&configMenu};
      MenuCheckItem synchronizeVideo{&configMenu};
      MenuCheckItem synchronizeAudio{&configMenu};
      MenuCheckItem muteAudio{&configMenu};
      MenuCheckItem showStatusBar{&configMenu};
      MenuSeparator settingsSeparator{&configMenu};
      MenuItem showVideoSettings{&configMenu};
      MenuItem showAudioSettings{&configMenu};
      MenuItem showPathSettings{&configMenu};
    Menu cheatMenu{&menuBar};
      MenuItem cheatEditor{&cheatMenu};
    Menu miscMenu{&menuBar};
      MenuItem showHotkeySettings{&miscMenu};
      MenuItem showAdvancedSettings{&miscMenu};
      MenuItem showManifestViewer{&miscMenu};
      MenuSeparator miscMenuSeparator{&miscMenu};
      MenuItem about{&miscMenu};

  FixedLayout layout{this};
    Viewport viewport{&layout, Geometry{0, 0, 1, 1}};

  StatusBar statusBar{this};

  const uint32* videoBuffer = nullptr;

  struct SnowParticle {
    uint16_t posX;
    uint16_t posY;
    uint16_t velX;
    uint16_t velY;
  };
  SnowParticle snowParticles[200];

private:
  enum Effect : unsigned {
    None,
    Snow,
    WaterA,
    WaterB,
    Burning,
    Smoke,
    TVNoise,
  };

  auto updateEffect(Effect effect, uint32_t* output, uint8_t* indexedOutput) -> void;
  auto updateSnow(uint32_t* output) -> void;
  auto updateWater(uint8_t* indexedOutput, bool mode) -> void;
  auto updateBurning(uint8_t* indexedOutput) -> void;
  auto updateSmoke(uint8_t* indexedOutput) -> void;

  auto drawWaterWithLight(uint8_t* indexedOutput, int* ptr, int light) -> void;
  auto calcWater(int* newTable, int* originalTable, int density) -> void;
  auto waterSineBlob(int x, int y, int radius, int height, int* ptr) -> void;
  auto drawSmokeBottom(uint8_t* buffer) -> void;
};

extern unique_pointer<AboutWindow> aboutWindow;
extern unique_pointer<Presentation> presentation;
