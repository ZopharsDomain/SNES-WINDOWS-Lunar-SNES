/*
struct CheatDatabase : Window {
  CheatDatabase();
  auto findCodes() -> void;
  auto addCodes() -> void;

  vector<string> codes;

  VerticalLayout layout{this};
    ListView cheatList{&layout, Size{~0, ~0}};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}};
      Button selectAllButton{&controlLayout, Size{100, 0}};
      Button unselectAllButton{&controlLayout, Size{100, 0}};
      Widget spacer{&controlLayout, Size{~0, 0}};
      Button addCodesButton{&controlLayout, Size{100, 0}};
};
*/

struct CheatEditor : Window {
  enum : uint { Slots = 128 };

  CheatEditor();
  auto show() -> void;
  auto doChangeSelected() -> void;
  auto doModify() -> void;
  auto doRefresh() -> void;
  auto doReset(bool force = false) -> void;
  auto doErase() -> void;
  auto synchronizeCodes() -> void;
  auto addCode(const string& code, const string& description, bool enabled = false) -> bool;
  auto loadCheats() -> void;
  auto saveCheats() -> void;
  //Lunar SNES: track keystrokes for activating the "01000001-01101101-01111001" easter egg
  auto trackKeystrokes() -> void;

  struct Cheat {
    bool enabled = false;
    string code;
    string description;
  };
  Cheat cheats[Slots];

  VerticalLayout layout{this};
    TableView cheatList{&layout, Size{~0, ~0}};
    HorizontalLayout codeLayout{&layout, Size{~0, 0}};
      Label codeLabel{&codeLayout, Size{70, 0}};
      LineEdit codeValue{&codeLayout, Size{~0, 0}};
    HorizontalLayout descriptionLayout{&layout, Size{~0, 0}};
      Label descriptionLabel{&descriptionLayout, Size{70, 0}};
      LineEdit descriptionValue{&descriptionLayout, Size{~0, 0}};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}};
      Button findCodesButton{&controlLayout, Size{120, 0}};
      Widget spacer{&controlLayout, Size{~0, 0}};
      Button resetButton{&controlLayout, Size{80, 0}};
      Button eraseButton{&controlLayout, Size{80, 0}};
};

extern unique_pointer<CheatEditor> cheatEditor;

struct StateManager : Window {
  enum : uint { Slots = 32 };

  StateManager();
  auto show() -> void;
  auto doChangeSelected() -> void;
  auto doRefresh() -> void;
  auto doUpdateControls() -> void;
  auto doChangeDescription() -> void;
  auto doLoad() -> void;
  auto doSave() -> void;
  auto doReset() -> void;
  auto doErase() -> void;
  //Lunar SNES: track keystrokes for activating the "ChibiMoon" easter egg
  auto trackKeystrokes() -> void;

  VerticalLayout layout{this};
    TableView stateList{&layout, Size{~0, ~0}};
    HorizontalLayout descriptionLayout{&layout, Size{~0, 0}};
      Label descriptionLabel{&descriptionLayout, Size{70, 0}};
      LineEdit descriptionValue{&descriptionLayout, Size{~0, 0}};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}};
      Button saveButton{&controlLayout, Size{80, 0}};
      Button loadButton{&controlLayout, Size{80, 0}};
      Widget spacer{&controlLayout, Size{~0, 0}};
      Button resetButton{&controlLayout, Size{80, 0}};
      Button eraseButton{&controlLayout, Size{80, 0}};
};

extern unique_pointer<StateManager> stateManager;

struct ManifestViewer : Window {
  ManifestViewer();
  auto show() -> void;
  auto doRefresh() -> void;

  VerticalLayout layout{this};
    TextEdit manifestView{&layout, Size{~0, ~0}};
};

extern unique_pointer<ManifestViewer> manifestViewer;

/*
struct GameNotes : TabFrameItem {
  GameNotes(TabFrame*);
  auto loadNotes() -> void;
  auto saveNotes() -> void;

  VerticalLayout layout{this};
    TextEdit notes{&layout, Size{~0, ~0}};
};
*/

/*
struct ToolsManager : Window {
  ToolsManager();
  auto show(uint tool) -> void;

  VerticalLayout layout{this};
    TabFrame panel{&layout, Size{~0, ~0}};
    //StateManager stateManager{&panel};
    //ManifestViewer manifestViewer{&panel};
    //GameNotes gameNotes{&panel};
};
*/

//extern unique_pointer<ToolsManager> toolsManager;
