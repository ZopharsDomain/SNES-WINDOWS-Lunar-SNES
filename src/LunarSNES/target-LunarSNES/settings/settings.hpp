struct VideoSettingsManager : Window {
  VideoSettingsManager();

  VerticalLayout layout{this};
    Label colorAdjustmentLabel{&layout, Size{~0, 0}, 2};
    HorizontalLayout saturationLayout{&layout, Size{~0, 0}};
      Label saturationLabel{&saturationLayout, Size{80, 0}};
      Label saturationValue{&saturationLayout, Size{50, 0}};
      HorizontalSlider saturationSlider{&saturationLayout, Size{~0, 0}};
    HorizontalLayout gammaLayout{&layout, Size{~0, 0}};
      Label gammaLabel{&gammaLayout, Size{80, 0}};
      Label gammaValue{&gammaLayout, Size{50, 0}};
      HorizontalSlider gammaSlider{&gammaLayout, Size{~0, 0}};
    HorizontalLayout luminanceLayout{&layout, Size{~0, 0}};
      Label luminanceLabel{&luminanceLayout, Size{80, 0}};
      Label luminanceValue{&luminanceLayout, Size{50, 0}};
      HorizontalSlider luminanceSlider{&luminanceLayout, Size{~0, 0}};
    Label overscanMaskLabel{&layout, Size{~0, 0}, 2};
    HorizontalLayout horizontalMaskLayout{&layout, Size{~0, 0}};
      Label horizontalMaskLabel{&horizontalMaskLayout, Size{80, 0}};
      Label horizontalMaskValue{&horizontalMaskLayout, Size{50, 0}};
      HorizontalSlider horizontalMaskSlider{&horizontalMaskLayout, Size{~0, 0}};
    HorizontalLayout verticalMaskLayout{&layout, Size{~0, 0}};
      Label verticalMaskLabel{&verticalMaskLayout, Size{80, 0}};
      Label verticalMaskValue{&verticalMaskLayout, Size{50, 0}};
      HorizontalSlider verticalMaskSlider{&verticalMaskLayout, Size{~0, 0}};
    Label windowedModeLabel{&layout, Size{~0, 0}, 2};
    HorizontalLayout windowedModeLayout{&layout, Size{~0, 0}};
      CheckLabel windowedModeAspectCorrection{&windowedModeLayout, Size{0, 0}};
      CheckLabel windowedModeIntegralScaling{&windowedModeLayout, Size{0, 0}};
    Label fullscreenModeLabel{&layout, Size{~0, 0}, 2};
    HorizontalLayout fullscreenModeLayout{&layout, Size{~0, 0}};
      CheckLabel fullscreenModeAspectCorrection{&fullscreenModeLayout, Size{0, 0}};
      CheckLabel fullscreenModeIntegralScaling{&fullscreenModeLayout, Size{0, 0}};
      CheckLabel fullscreenModeExclusive{&fullscreenModeLayout, Size{0, 0}};

  auto show() -> void;
  auto updateColor(bool initializing = false) -> void;
  auto updateViewport(bool initializing = false) -> void;
};

extern unique_pointer<VideoSettingsManager> videoSettingsManager;

struct AudioSettingsManager : Window {
  AudioSettingsManager();

  VerticalLayout layout{this};
    Label driverLabel{&layout, Size{~0, 0}, 2};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}};
      Label deviceLabel{&controlLayout, Size{0, 0}};
      ComboButton deviceList{&controlLayout, Size{~0, 0}};
      Label frequencyLabel{&controlLayout, Size{0, 0}};
      ComboButton frequencyList{&controlLayout, Size{80, 0}};
      Label latencyLabel{&controlLayout, Size{0, 0}};
      ComboButton latencyList{&controlLayout, Size{80, 0}};
    CheckLabel exclusiveMode{&layout, Size{~0, 0}};
    Label effectsLabel{&layout, Size{~0, 0}, 2};
    HorizontalLayout volumeLayout{&layout, Size{~0, 0}};
      Label volumeLabel{&volumeLayout, Size{80, 0}};
      Label volumeValue{&volumeLayout, Size{50, 0}};
      HorizontalSlider volumeSlider{&volumeLayout, Size{~0, 0}};
    HorizontalLayout balanceLayout{&layout, Size{~0, 0}};
      Label balanceLabel{&balanceLayout, Size{80, 0}};
      Label balanceValue{&balanceLayout, Size{50, 0}};
      HorizontalSlider balanceSlider{&balanceLayout, Size{~0, 0}};
    CheckLabel reverbEnable{&layout, Size{~0, 0}};

  auto show() -> void;
  auto updateDevice() -> void;
  auto updateEffects(bool initializing = false) -> void;
};

extern unique_pointer<AudioSettingsManager> audioSettingsManager;

struct InputSettingsTab : TabFrameItem {
  InputSettingsTab(TabFrame*, uint);

  VerticalLayout layout{this};
    TableView mappingList{&layout, Size{~0, ~0}};

  auto activePort() -> InputPort&;
  auto activeDevice() -> InputDevice&;
  auto reloadMappings() -> void;
  auto refreshMappings() -> void;

  const uint portID;
};

struct InputSettingsManager : Window {
  InputSettingsManager();

  VerticalLayout layout{this};
    HorizontalLayout focusLayout{&layout, Size{~0, 0}};
      Label focusLabel{&focusLayout, Size{0, 0}};
      CheckLabel pauseEmulation{&focusLayout, Size{0, 0}};
      CheckLabel allowInput{&focusLayout, Size{0, 0}};
    TabFrame panel{&layout, Size{~0, ~0}};
      InputSettingsTab port1{&panel, 0};
      InputSettingsTab port2{&panel, 1};
      InputSettingsTab port3{&panel, 2};
      InputSettingsTab port4{&panel, 3};
      InputSettingsTab port5{&panel, 4};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}};
      Widget spacer{&controlLayout, Size{~0, 0}};
      Button resetButton{&controlLayout, Size{80, 0}};
      Button eraseButton{&controlLayout, Size{80, 0}};
  StatusBar statusBar{this};

  auto setVisible(bool visible = true) -> InputSettingsManager&;
  auto show() -> void;
  auto updateControls() -> void;
  auto activeTab() -> InputSettingsTab&;
  auto activePort() -> InputPort&;
  auto activeDevice() -> InputDevice&;
  auto reloadMappings() -> void;
  auto refreshMappings() -> void;
  auto assignMapping() -> void;
  auto inputEvent(shared_pointer<HID::Device> device, uint group, uint input, int16 oldValue, int16 newValue) -> void;

  InputMapping* activeMapping = nullptr;
  Timer timer;
};

extern unique_pointer<InputSettingsManager> inputSettingsManager;

struct HotkeySettingsManager : Window {
  HotkeySettingsManager();

  VerticalLayout layout{this};
    TableView mappingList{&layout, Size{~0, ~0}};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}};
      Widget spacer{&controlLayout, Size{~0, 0}};
      Button resetButton{&controlLayout, Size{80, 0}};
      Button eraseButton{&controlLayout, Size{80, 0}};
  StatusBar statusBar{this};

  auto setVisible(bool visible = true) -> HotkeySettingsManager&;
  auto show() -> void;
  auto reloadMappings() -> void;
  auto refreshMappings() -> void;
  auto assignMapping() -> void;
  auto inputEvent(shared_pointer<HID::Device> device, uint group, uint input, int16 oldValue, int16 newValue) -> void;

  InputMapping* activeMapping = nullptr;
  Timer timer;
};

extern unique_pointer<HotkeySettingsManager> hotkeySettingsManager;

struct AdvancedSettingsManager : Window {
  AdvancedSettingsManager();

  VerticalLayout layout{this};
    Label driverLabel{&layout, Size{~0, 0}, 2};
    HorizontalLayout driverLayout{&layout, Size{~0, 0}};
      Label videoLabel{&driverLayout, Size{0, 0}};
      ComboButton videoDriver{&driverLayout, Size{~0, 0}};
      Label audioLabel{&driverLayout, Size{0, 0}};
      ComboButton audioDriver{&driverLayout, Size{~0, 0}};
      Label inputLabel{&driverLayout, Size{0, 0}};
      ComboButton inputDriver{&driverLayout, Size{~0, 0}};
    Label otherLabel{&layout, Size{~0, 0}, 2};
    CheckLabel autoSaveMemory{&layout, Size{~0, 0}};
    Label effectLabel{&layout, Size{~0, 0}, 2};
    HorizontalLayout effectRow1{&layout, Size{~0, 0}};
      RadioLabel effectNone{&effectRow1, Size{~0, 0}};
      RadioLabel effectSnow{&effectRow1, Size{~0, 0}};
      RadioLabel effectBurning{&effectRow1, Size{~0, 0}};
    HorizontalLayout effectRow2{&layout, Size{~0, 0}};
      RadioLabel effectSmoke{&effectRow2, Size{~0, 0}};
      RadioLabel effectWaterA{&effectRow2, Size{~0, 0}};
      RadioLabel effectWaterB{&effectRow2, Size{~0, 0}};
    Group effectGroup{&effectNone, &effectSnow, &effectBurning, &effectSmoke, &effectWaterA, &effectWaterB};
 
  auto show() -> void;
};

extern unique_pointer<AdvancedSettingsManager> advancedSettingsManager;

struct PathSettingsManager : Window {
  PathSettingsManager();

  VerticalLayout layout{this};
    HorizontalLayout saveLayout{&layout, Size{~0, 0}};
      Label saveLabel{&saveLayout, Size{80, 0}};
      LineEdit saveEdit{&saveLayout, Size{~0, 0}};
      Button saveBrowse{&saveLayout, Size{30, 0}};
    HorizontalLayout stateLayout{&layout, Size{~0, 0}};
      Label stateLabel{&stateLayout, Size{80, 0}};
      LineEdit stateEdit{&stateLayout, Size{~0, 0}};
      Button stateBrowse{&stateLayout, Size{30, 0}};
    HorizontalLayout cheatLayout{&layout, Size{~0, 0}};
      Label cheatLabel{&cheatLayout, Size{80, 0}};
      LineEdit cheatEdit{&cheatLayout, Size{~0, 0}};
      Button cheatBrowse{&cheatLayout, Size{30, 0}};

  auto show() -> void;
};

extern unique_pointer<PathSettingsManager> pathSettingsManager;

struct DeviceSelector : Window {
  DeviceSelector();

  VerticalLayout layout{this};
    HorizontalLayout portsLayout{&layout, Size{~0, ~0}};
      VerticalLayout port1Layout{&portsLayout, Size{~0, ~0}};
        Label port1Label{&port1Layout, Size{~0, 0}};
        RadioLabel port1Gamepad{&port1Layout, Size{~0, 0}};
        RadioLabel port1Mouse{&port1Layout, Size{~0, 0}};
        Group port1Group{&port1Gamepad, &port1Mouse};
        Widget spacer{&port1Layout, Size{~0, ~0}};
      VerticalLayout port2Layout{&portsLayout, Size{~0, ~0}};
        Label port2Label{&port2Layout, Size{~0, 0}};
        RadioLabel port2Gamepad{&port2Layout, Size{~0, 0}};
        RadioLabel port2Mouse{&port2Layout, Size{~0, 0}};
        RadioLabel port2SuperScope{&port2Layout, Size{~0, 0}};
        RadioLabel port2Justifier{&port2Layout, Size{~0, 0}};
        RadioLabel port2Justifiers{&port2Layout, Size{~0, 0}};
        Group port2Group{&port2Gamepad, &port2Mouse, &port2SuperScope, &port2Justifier, &port2Justifiers};
    HorizontalLayout optionsLayout{&layout, Size{~0, 0}};
      VerticalLayout opts1Layout{&optionsLayout, Size{~0, 0}};
        CheckLabel port1LeftHanded{&opts1Layout, Size{~0, 0}};
      VerticalLayout opts2Layout{&optionsLayout, Size{~0, 0}};
        CheckLabel port2LeftHanded{&opts2Layout, Size{~0, 0}};
    Label superScopeLabel{&layout, Size{~0, 0}};
    HorizontalLayout superScopeLayout{&layout, Size{~0, 0}};
      Label turboLabel{&superScopeLayout, Size{~0, 0}};
      Button turboMap{&superScopeLayout, Size{~0, 0}};
      Label pauseLabel{&superScopeLayout, Size{~0, 0}};
      Button pauseMap{&superScopeLayout, Size{~0, 0}};

  auto show() -> void;
  auto bindMouse(uint portID, uint deviceID, uint xAxis, uint yAxis, uint left, uint right) -> void;
  auto refreshMappings() -> void;
  auto assignTurboMapping() -> void;
  auto assignPauseMapping() -> void;
  auto inputEvent(shared_pointer<HID::Device> device, uint group, uint input, int16 oldValue, int16 newValue) -> void;

  InputMapping* activeMapping = nullptr;
  Timer timer;
};

extern unique_pointer<DeviceSelector> deviceSelector;
