name := LunarSNES
#flags += -DSFC_SUPERGAMEBOY

include sfc/GNUmakefile
#include gb/GNUmakefile
include processor/GNUmakefile

ui_objects := ui-LunarSNES ui-program ui-icarus ui-configuration ui-input
ui_objects += ui-settings ui-tools ui-presentation ui-embed
ui_objects += ruby hiro
ui_objects += $(if $(call streq,$(platform),windows),ui-resource)

# platform
ifeq ($(platform),windows)
  ruby += video.wgl video.direct3d video.directdraw video.gdi
  ruby += audio.asio audio.wasapi audio.xaudio2 audio.directsound
  ruby += input.windows
else ifeq ($(platform),macos)
  ruby += video.cgl
  ruby += audio.openal
  ruby += input.quartz input.carbon
else ifeq ($(platform),linux)
  ruby += video.glx video.xvideo video.xshm video.sdl
  ruby += audio.oss audio.alsa audio.openal audio.pulseaudio audio.pulseaudiosimple audio.ao
  ruby += input.sdl input.xlib input.udev
else ifeq ($(platform),bsd)
  ruby += video.glx video.xvideo video.xshm video.sdl
  ruby += audio.oss audio.openal
  ruby += input.sdl input.xlib
endif

# ruby
include ../ruby/GNUmakefile
link += $(rubylink)

# hiro
include ../hiro/GNUmakefile
link += $(hirolink)

# rules
objects := $(ui_objects) $(objects)
objects := $(patsubst %,obj/%.o,$(objects))

obj/ruby.o: ../ruby/ruby.cpp $(call rwildcard,../ruby/)
	$(compiler) $(rubyflags) -c $< -o $@

obj/hiro.o: ../hiro/hiro.cpp $(call rwildcard,../hiro/)
	$(compiler) $(hiroflags) -c $< -o $@

obj/ui-LunarSNES.o: $(ui)/LunarSNES.cpp $(call rwildcard,$(ui)/)
obj/ui-program.o: $(ui)/program/program.cpp $(call rwildcard,$(ui)/)
obj/ui-icarus.o: $(ui)/icarus/icarus.cpp $(call rwildcard,$(ui)/) $(call rwildcard,../icarus/core/) $(call rwildcard,../icarus/heuristics/)
obj/ui-configuration.o: $(ui)/configuration/configuration.cpp $(call rwildcard,$(ui)/)
obj/ui-input.o: $(ui)/input/input.cpp $(call rwildcard,$(ui)/)
obj/ui-library.o: $(ui)/library/library.cpp $(call rwildcard,$(ui)/)
obj/ui-settings.o: $(ui)/settings/settings.cpp $(call rwildcard,$(ui)/)
obj/ui-tools.o: $(ui)/tools/tools.cpp $(call rwildcard,$(ui)/)
obj/ui-presentation.o: $(ui)/presentation/presentation.cpp $(call rwildcard,$(ui)/)
obj/ui-embed.o: $(ui)/embed/embed.cpp

obj/ui-resource.o:
ifeq ($(architecture),win32)
	$(windres) --target=pe-i386 data/LunarSNES.rc obj/ui-resource.o
else
	$(windres) data/LunarSNES.rc obj/ui-resource.o
endif

# targets
build: $(objects)
	$(strip $(compiler) -o out/$(name) $(objects) $(link))
ifeq ($(platform),macos)
	@if [ -d out/$(name).app ]; then rm -r out/$(name).app; fi
	mkdir -p out/$(name).app/Contents/MacOS/
	mkdir -p out/$(name).app/Contents/Resources/
	mv out/$(name) out/$(name).app/Contents/MacOS/$(name)
	cp data/$(name).plist out/$(name).app/Contents/Info.plist
	sips -s format icns data/$(name).png --out out/$(name).app/Contents/Resources/$(name).icns
endif

install:
ifeq ($(shell id -un),root)
	$(error "make install should not be run as root")
else ifeq ($(platform),windows)
else ifeq ($(platform),macos)
	mkdir -p ~/Library/Application\ Support/$(name)/
	mkdir -p ~/Emulation/System/
	cp -R out/$(name).app /Applications/$(name).app
	cp -R systems/* ~/Library/Application\ Support/$(name)/
else ifneq ($(filter $(platform),linux bsd),)
	mkdir -p $(prefix)/bin/
	mkdir -p $(prefix)/share/applications/
	mkdir -p $(prefix)/share/icons/
	mkdir -p $(prefix)/share/$(name)/
	cp out/$(name) $(prefix)/bin/$(name)
	cp -R systems/* $(prefix)/share/$(name)/
	cp data/$(name).desktop $(prefix)/share/applications/$(name).desktop
	cp data/$(name).png $(prefix)/share/icons/$(name).png
endif

uninstall:
ifeq ($(shell id -un),root)
	$(error "make uninstall should not be run as root")
else ifeq ($(platform),windows)
else ifeq ($(platform),macos)
	if [ -d /Applications/$(name).app ]; then rm -r /Applications/$(name).app; fi
else ifneq ($(filter $(platform),linux bsd),)
	if [ -f $(prefix)/bin/$(name) ]; then rm $(prefix)/bin/$(name); fi
	if [ -f $(prefix)/share/applications/$(name).desktop ]; then rm $(prefix)/share/applications/$(name).desktop; fi
	if [ -f $(prefix)/share/icons/$(name).png ]; then rm $(prefix)/share/icons/$(name).png; fi
endif
