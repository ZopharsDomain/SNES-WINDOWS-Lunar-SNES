struct Icarus {
  struct Output {
    vector<uint8_t> buffer;
    string manifest;
    string_vector filenames;
    vector<uint> offsets;
    vector<uint> sizes;
    auto reset() -> void;
  };
  string_vector zipPaths;
  string_vector zipTypes;

  //icarus.cpp
  Icarus();

  auto extension(string extension) -> string;
  auto select(const string& path) -> Output*;
  auto filename(const string& path) -> string;
  auto offset(const string& path) -> uint;
  auto size(const string& path) -> uint;
  auto reset() -> void;

  //shim.cpp
  auto create(const string& pathname) -> bool;
  auto read(const string& path) -> vector<uint8_t>;  //change by the libretro team
  auto exists(const string& path) -> bool;
  auto directory_exists(const string& path) -> bool;  //change by the libretro team
  auto copy(const string& target, const string& source) -> bool;
  auto write(const string& path, const uint8_t* data, uint size) -> bool;

  auto write(const string& path, const vector<uint8_t>& buffer) -> bool {
    return write(path, buffer.data(), buffer.size());
  }

  auto write(const string& path, const string& text) -> bool {
    return write(path, (const uint8_t*)text.data(), text.size());
  }

  //core.cpp
  auto error() const -> string;
  auto missing() const -> string_vector;
  auto success(string location) -> string;
  auto failure(string message) -> string;

  auto manifest(string location) -> string;
  auto import(string location) -> string;

  auto concatenate(vector<uint8_t>& output, string location) -> void;

  //super-famicom.cpp
  auto superFamicomManifest(string location) -> string;
  auto superFamicomManifest(vector<uint8_t>& buffer, string location) -> string;
  auto superFamicomManifestScan(vector<Markup::Node>& roms, Markup::Node node) -> void;
  auto superFamicomImport(vector<uint8_t>& buffer, string location) -> string;

  //game-boy.cpp
  auto gameBoyManifest(string location) -> string;
  auto gameBoyManifest(vector<uint8_t>& buffer, string location) -> string;
  auto gameBoyImport(vector<uint8_t>& buffer, string location) -> string;

  //bs-memory.cpp
  auto bsMemoryManifest(string location) -> string;
  auto bsMemoryManifest(vector<uint8_t>& buffer, string location) -> string;
  auto bsMemoryImport(vector<uint8_t>& buffer, string location) -> string;

  //sufami-turbo.cpp
  auto sufamiTurboManifest(string location) -> string;
  auto sufamiTurboManifest(vector<uint8_t>& buffer, string location) -> string;
  auto sufamiTurboImport(vector<uint8_t>& buffer, string location) -> string;

  bool gamepak;
  Output sfc;
  Output gb;
  Output bs;
  Output st;

private:
  Markup::Node settings;
  string errorMessage;
  string_vector missingFiles;

  struct {
    Markup::Node superFamicom;
    Markup::Node gameBoy;
    Markup::Node bsMemory;
    Markup::Node sufamiTurbo;
  } database;
};

extern unique_pointer<Icarus> icarus;
