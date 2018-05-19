unique_pointer<ManifestViewer> manifestViewer;

ManifestViewer::ManifestViewer() {
  manifestViewer = this;
  setTitle("Manifest Viewer");

  manifestView.setEditable(false).setWordWrap(false).setFont(Font().setFamily(Font::Mono));

  layout.setMargin(5);
  setSize({600, 405});
  setAlignment({0.5, 0.5});
  setDismissable();
  doSize();
}

auto ManifestViewer::show() -> void {
  setVisible();
  setFocused();
  doSize();
}

auto ManifestViewer::doRefresh() -> void {
  manifestView.setText("");
  if(emulator->loaded()) manifestView.setText(emulator->manifest());
}
