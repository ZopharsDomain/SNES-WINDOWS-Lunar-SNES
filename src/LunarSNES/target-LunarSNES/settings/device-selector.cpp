unique_pointer<DeviceSelector> deviceSelector;

DeviceSelector::DeviceSelector() {
  deviceSelector = this;

  layout.setMargin(5);

  port1Label.setText("Port 1:");

  port1Gamepad.setText("Gamepad").onActivate([&] {
    settings["SuperFamicom/ControllerPort1"].setValue("Gamepad");
    emulator->connect(SFC::ID::Port::Controller1, SFC::ID::Device::Gamepad);
  });
  if(settings["SuperFamicom/ControllerPort1"].text() == "Gamepad") port1Gamepad.setChecked();

  port1Mouse.setText("Mouse").onActivate([&] {
    settings["SuperFamicom/ControllerPort1"].setValue("Mouse");
    emulator->connect(SFC::ID::Port::Controller1, SFC::ID::Device::Mouse);
    port1LeftHanded.doToggle();
  });
  if(settings["SuperFamicom/ControllerPort1"].text() == "Mouse") port1Mouse.setChecked();

  port1LeftHanded.setText("Left Handed").onToggle([&] {
    if(port1LeftHanded.checked()) {
      bindMouse(SFC::ID::Port::Controller1, SFC::ID::Device::Mouse, 0, 1, 3, 2);  //X-axis, Y-axis, Right, Left
    } else {
      bindMouse(SFC::ID::Port::Controller1, SFC::ID::Device::Mouse, 0, 1, 2, 3);  //X-axis, Y-axis, Left, Right
    }
  });

  port2Label.setText("Port 2:");

  port2Gamepad.setText("Gamepad").onActivate([&] {
    settings["SuperFamicom/ControllerPort2"].setValue("Gamepad");
    emulator->connect(SFC::ID::Port::Controller2, SFC::ID::Device::Gamepad);
  });
  if(settings["SuperFamicom/ControllerPort2"].text() == "Gamepad") port2Gamepad.setChecked();

  port2Mouse.setText("Mouse").onActivate([&] {
    settings["SuperFamicom/ControllerPort2"].setValue("Mouse");
    emulator->connect(SFC::ID::Port::Controller2, SFC::ID::Device::Mouse);
    port2LeftHanded.doToggle();
  });
  if(settings["SuperFamicom/ControllerPort2"].text() == "Mouse") port2Mouse.setChecked();

  port2SuperScope.setText("Super Scope").onActivate([&] {
    settings["SuperFamicom/ControllerPort2"].setValue("Super Scope");
    emulator->connect(SFC::ID::Port::Controller2, SFC::ID::Device::SuperScope);
    port2LeftHanded.doToggle();
  });
  if(settings["SuperFamicom/ControllerPort2"].text() == "Super Scope") port2SuperScope.setChecked();

  port2Justifier.setText("1 Justifier").onActivate([&] {
    settings["SuperFamicom/ControllerPort2"].setValue("Justifier");
    emulator->connect(SFC::ID::Port::Controller2, SFC::ID::Device::Justifier);
    port2LeftHanded.doToggle();
  });
  if(settings["SuperFamicom/ControllerPort2"].text() == "Justifier") port2Justifier.setChecked();

  port2Justifiers.setText("2 Justifiers").onActivate([&] {
    settings["SuperFamicom/ControllerPort2"].setValue("Justifiers");
    emulator->connect(SFC::ID::Port::Controller2, SFC::ID::Device::Justifiers);
  });
  if(settings["SuperFamicom/ControllerPort2"].text() == "Justifiers") port2Justifiers.setChecked();

  port2LeftHanded.setText("Left Handed").onToggle([&] {
    if(port2LeftHanded.checked()) {
      bindMouse(SFC::ID::Port::Controller2, SFC::ID::Device::Mouse,      0, 1, 3, 2);  //X-axis,Y-axis,Right,Left
      bindMouse(SFC::ID::Port::Controller2, SFC::ID::Device::SuperScope, 0, 1, 3, 2);  //X-axis,Y-axis,Cursor,Trigger
      bindMouse(SFC::ID::Port::Controller2, SFC::ID::Device::Justifier,  0, 1, 3, 2);  //X-axis,Y-axis,Start,Trigger
    } else {
      bindMouse(SFC::ID::Port::Controller2, SFC::ID::Device::Mouse,      0, 1, 2, 3);  //X-axis,Y-axis,Left,Right
      bindMouse(SFC::ID::Port::Controller2, SFC::ID::Device::SuperScope, 0, 1, 2, 3);  //X-axis,Y-axis,Trigger,Cursor
      bindMouse(SFC::ID::Port::Controller2, SFC::ID::Device::Justifier,  0, 1, 2, 3);  //X-axis,Y-axis,Trigger,Start
    }
  });

  superScopeLabel.setText("Super Scope Keys:");
  turboLabel.setText("Auto-Fire");
  pauseLabel.setText("SS Pause");

  setTitle("Device Selector");
  setAlignment({0.5, 0.5});
  setResizable(false);
  setDismissable();
}

auto DeviceSelector::show() -> void {
  setVisible();
  setFocused();
  doSize();
}

auto DeviceSelector::bindMouse(uint portID, uint deviceID, uint xAxis, uint yAxis, uint left, uint right) -> void {
  if(auto mouse = inputManager->findMouse()) {
    InputDevice* device = nullptr;
    for(auto& _device : inputManager->ports[portID].devices) {
      if(_device.id == deviceID) {
        device = &_device;
        break;
      }
    }
    if(!device) return;

    device->mappings[xAxis].unbind();
    device->mappings[yAxis].unbind();
    device->mappings[left ].unbind();
    device->mappings[right].unbind();
    device->mappings[xAxis].bind(mouse, HID::Mouse::GroupID::Axis,   0, 0, +32767);
    device->mappings[yAxis].bind(mouse, HID::Mouse::GroupID::Axis,   1, 0, +32767);
    device->mappings[left ].bind(mouse, HID::Mouse::GroupID::Button, 0, 0, 1);
    device->mappings[right].bind(mouse, HID::Mouse::GroupID::Button, 2, 0, 1);
  }
}

auto DeviceSelector::refreshMappings() -> void {
  InputDevice* device = nullptr;
  for(auto& _device : inputManager->ports[SFC::ID::Port::Controller2].devices) {
    if(_device.id == SFC::ID::Device::SuperScope) {
      device = &_device;
      break;
    }
  }

  turboMap.setText(device->mappings[SFC::SuperScope::Turbo].name);
  pauseMap.setText(device->mappings[SFC::SuperScope::Pause].name);
}

auto DeviceSelector::assignTurboMapping() -> void {
  inputManager->poll();  //clear any pending events first

  InputDevice* device = nullptr;
  for(auto& _device : inputManager->ports[SFC::ID::Port::Controller2].devices) {
    if(_device.id == SFC::ID::Device::SuperScope) {
      device = &_device;
      break;
    }
  }
  if(device) {
    activeMapping = &device->mappings[SFC::SuperScope::Turbo];
    layout.setEnabled(false);
  }
}

auto DeviceSelector::assignPauseMapping() -> void {
  inputManager->poll();  //clear any pending events first

  InputDevice* device = nullptr;
  for(auto& _device : inputManager->ports[SFC::ID::Port::Controller2].devices) {
    if(_device.id == SFC::ID::Device::SuperScope) {
      device = &_device;
      break;
    }
  }
  if(device) {
    activeMapping = &device->mappings[SFC::SuperScope::Pause];
    layout.setEnabled(false);
  }
}

auto DeviceSelector::inputEvent(shared_pointer<HID::Device> device, uint group, uint input, int16 oldValue, int16 newValue) -> void {
  if(!activeMapping) return;
  if(device->isMouse()) return;

  if(activeMapping->bind(device, group, input, oldValue, newValue)) {
    activeMapping = nullptr;
    refreshMappings();
    timer.onActivate([&] {
      timer.setEnabled(false);
      layout.setEnabled();
    }).setInterval(200).setEnabled();
  }
}
