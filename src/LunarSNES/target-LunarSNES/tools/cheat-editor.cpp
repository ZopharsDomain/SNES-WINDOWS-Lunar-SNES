unique_pointer<CheatEditor> cheatEditor;

CheatEditor::CheatEditor() {
  cheatEditor = this;

  setTitle("Cheat Editor");

  layout.setMargin(5);
  setSize({600, 405});
  setAlignment({0.5, 0.5});
  setDismissable();

  cheatList.append(TableViewHeader().setVisible()
    .append(TableViewColumn().setText("Slot").setForegroundColor({0, 128, 0}).setAlignment(1.0))
    .append(TableViewColumn().setText("Code(s)"))
    .append(TableViewColumn().setText("Description").setExpandable())
  );
  for(auto slot : range(Slots)) {
    cheatList.append(TableViewItem()
      .append(TableViewCell().setCheckable().setText(1 + slot))
      .append(TableViewCell())
      .append(TableViewCell())
    );
  }
  cheatList.onChange([&] { doChangeSelected(); });
  cheatList.onToggle([&](TableViewCell cell) {
    cheats[cell.parent().offset()].enabled = cell.checked();
    synchronizeCodes();
  });
  codeLabel.setText("Code(s):");
  codeValue.onChange([&] { doModify(); });
  descriptionLabel.setText("Description:");
  descriptionValue.onChange([&] { doModify(); });
  //findCodesButton.setText("Find Codes ...").onActivate([&] { cheatDatabase->findCodes(); });
  resetButton.setText("Reset").onActivate([&] { doReset(); });
  eraseButton.setText("Erase").onActivate([&] { doErase(); });

  //do not display "Find Codes" button if there is no cheat database to look up codes in
  /*if(!file::exists(locate("cheats.bml")))*/ findCodesButton.setVisible(false);
}

auto CheatEditor::show() -> void {
  setVisible();
  setFocused();
  doSize();
}

auto CheatEditor::doChangeSelected() -> void {
  if(auto item = cheatList.selected()) {
    auto& cheat = cheats[item.offset()];
    codeValue.setEnabled(true).setText(cheat.code);
    descriptionValue.setEnabled(true).setText(cheat.description);
    eraseButton.setEnabled(true);
  } else {
    codeValue.setEnabled(false).setText("");
    descriptionValue.setEnabled(false).setText("");
    eraseButton.setEnabled(false);
  }
}

auto CheatEditor::doModify() -> void {
  if(auto item = cheatList.selected()) {
    auto& cheat = cheats[item.offset()];
    cheat.code = codeValue.text();
    cheat.description = descriptionValue.text();
    doRefresh();
    synchronizeCodes();
  }
}

auto CheatEditor::doRefresh() -> void {
  for(auto slot : range(Slots)) {
    auto& cheat = cheats[slot];
    if(cheat.code || cheat.description) {
      auto codes = cheat.code.split("+");
      if(codes.size() > 1) codes[0].append("+...");
      cheatList.item(slot).cell(0).setChecked(cheat.enabled);
      cheatList.item(slot).cell(1).setText(codes[0]);
      cheatList.item(slot).cell(2).setText(cheat.description).setForegroundColor({0, 0, 0});
    } else {
      cheatList.item(slot).cell(0).setChecked(false);
      cheatList.item(slot).cell(1).setText("");
      cheatList.item(slot).cell(2).setText("(empty)").setForegroundColor({128, 128, 128});
    }
  }

  cheatList.resizeColumns();
}

auto CheatEditor::doReset(bool force) -> void {
  if(force || MessageDialog().setParent(*this).setText("Permanently erase all slots?").question() == "Yes") {
    for(auto& cheat : cheats) {
      cheat.enabled = false;
      cheat.code = "";
      cheat.description = "";
    }
    for(auto& item : cheatList.items()) {
      item.cell(0).setChecked(false);
    }
    doChangeSelected();
    doRefresh();
    synchronizeCodes();
  }
}

auto CheatEditor::doErase() -> void {
  if(auto item = cheatList.selected()) {
    auto& cheat = cheats[item.offset()];
    cheat.enabled = false;
    cheat.code = "";
    cheat.description = "";
    codeValue.setText("");
    descriptionValue.setText("");
    doRefresh();
    synchronizeCodes();
  }
}

auto CheatEditor::synchronizeCodes() -> void {
  if(!emulator->loaded()) return;

  string_vector codes;
  for(auto& cheat : cheats) {
    if(!cheat.enabled || !cheat.code) continue;
    codes.append(cheat.code);
  }

  emulator->cheatSet(codes);
}

//returns true if code was added
//returns false if there are no more free slots for additional codes
auto CheatEditor::addCode(const string& code, const string& description, bool enabled) -> bool {
  for(auto& cheat : cheats) {
    if(cheat.code || cheat.description) continue;
    cheat.enabled = enabled;
    cheat.code = code;
    cheat.description = description;
    return true;
  }

  return false;
}

auto CheatEditor::loadCheats() -> void {
  doReset(true);
  auto contents = string::read({settings["Path/Cheat"].text(), Location::prefix(program->mediumPaths(1)), ".cht"});
  auto document = BML::unserialize(contents);
  for(auto cheat : document["cartridge"].find("cheat")) {
    if(!addCode(cheat["code"].text(), cheat["description"].text(), (bool)cheat["enabled"])) break;
  }
  doRefresh();
  synchronizeCodes();
}

auto CheatEditor::saveCheats() -> void {
  if(!emulator->loaded()) return;
  string document = {"cartridge sha256:", emulator->sha256(), "\n"};
  uint count = 0;
  for(auto& cheat : cheats) {
    if(!cheat.code && !cheat.description) continue;
    document.append("  cheat", cheat.enabled ? " enabled" : "", "\n");
    document.append("    description:", cheat.description, "\n");
    document.append("    code:", cheat.code, "\n");
    count++;
  }
  if(count) {
    directory::create({program->mediumPaths(1), "higan/"});
    file::write({settings["Path/Cheat"].text(), Location::prefix(program->mediumPaths(1)), ".cht"}, document);
  } else {
    file::remove({settings["Path/Cheat"].text(), Location::prefix(program->mediumPaths(1)), ".cht"});
  }
  doReset(true);
}

//Lunar SNES: Change icon when activating the "01000001-01101101-01111001" easter egg
auto CheatEditor::trackKeystrokes() -> void {
  const string sequence0 = "Amy is cute!";
  const string sequence1 = "01000001-01101101-01111001";

  const string_vector validKeys = {
    "Space", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    "Dash",
  };
  const uint size = hiro::Keyboard::keys.size();
  static vector<bool> previousKeys;
  previousKeys.resize(size);
  static vector<bool> currentKeys;
  currentKeys.resize(size);
  static bool amyIsCute = false;
  static uint keystrokes = 0;
  string_view sequence = amyIsCute ? sequence1 : sequence0;
  if(focused() && keystrokes < sequence.size()) {
    assert(previousKeys.size() == size);
    assert(currentKeys.size() == size);
    memory::copy(previousKeys.data(), currentKeys.data(), size);
    currentKeys = hiro::Keyboard::poll();
    for(const string& key : validKeys) {
      uint index = *hiro::Keyboard::keys.find(key);
      if(currentKeys[index] && !previousKeys[index]) {
        if(key == "Space" && sequence[keystrokes] == ' ') {
          keystrokes++;
        } else if(key == "1" && sequence[keystrokes] == '!') {
          if(hiro::Keyboard::pressed("Shift")) {
            keystrokes++;
          } else {
            keystrokes = 0;
          }
        } else if(key == "Dash" && sequence[keystrokes] == '-') {
          if(hiro::Keyboard::pressed("Shift")) {
            keystrokes = 0;
          } else {
            keystrokes++;
          }
        } else if(key[0] >= '0' && key[0] <= '9' && key[0] == sequence[keystrokes]) {
          if(hiro::Keyboard::pressed("Shift")) {
            keystrokes = 0;
          } else {
            keystrokes++;
          }
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
      if(!amyIsCute) {
        amyIsCute = true;
        keystrokes = 0;
        self().setIcon(image(Embed::LunarSNES::Amy32), image(Embed::LunarSNES::Amy16));
        MessageDialog().setParent(*this).setTitle("01000001-01101101-01111001").setText(
          "MegaHAL: Nr., Relm und Chibi-USA sind unschuldig und sind\n"
          "Verteidiger durch dieses vortreffliche Lunarian, FuSoYa ist der\n"
          "Verteidiger des Südpazifiks. Die schalldichte Zelle ist Goodyears 7. und\n"
          "wird es aufdecken, wenn sie an Jupiter gelangen. Die Affen werden\n"
          "merkwürdig zur Regierung zur `` Rückkehr-Zustandeigenschaft zum\n"
          "Abschnitt von Westworld bevölkert durch Roboter desperadoes,\n"
          "Roboter Tanzenhalle Gallonen und die einzige Sache gezeichnet, die\n"
          "sogar langsam es unten kann? A. Elf auf einem volé le Leeloo-\n"
          "Leeloo-Club?"
        ).question({"Abort", "Retry", "Ignore"});
      } else {
        self().setIcon(image(Embed::LunarSNES::Chocobo32), image(Embed::LunarSNES::Chocobo16));
        cheatList.header().column(2).setText("Wark wark wark!");
      }
    }
  }
}
