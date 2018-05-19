AboutWindow::AboutWindow() {
  aboutWindow = this;

  setTitle("About Lunar SNES...");
  layout.setMargin(10);
  frame.setText("Lunar SNES : Super Nintendo Entertainment System Emulator");
  information.setAlignment(0.5).setText({
    "Lunar SNES was an April Fools joke created as a criticism of\n"
    "the closed source philosophy. It forcefully prepended copier headers\n"
    "to ROMs and used high-level emulation of the NEC DSP, both of which are\n"
    "missing in this version.\n"
    "\n",
    Emulator::Version, "\n"
    "Public Build x64 --- Apr  2 2018\n"
    "Based on higan by ", Emulator::Author, "\n",
    Emulator::Website
  });

  setResizable(false);
  setSize({400, 200});  //layout.minimumSize() does not work with frames
  setCentered();
  setDismissable();
}
