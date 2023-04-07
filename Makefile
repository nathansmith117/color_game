flcon = fltk-config
compiler = $(shell $(flcon) --cxx)
flags = $(shell $(flcon) --use-images --cxxflags) -Ijson -Isrc
ldflags = $(shell $(flcon) --use-images --ldstaticflags) \
json/json.c -static-libstdc++ -static-libgcc

files = src/color_game.cpp src/*.h
target = -o color_game src/color_game.cpp

color_game: $(files)
	$(compiler) $(flags) $(target) $(ldflags)
icon:
	echo 101 ICON DISCARDABLE "icons.windows/icon.ico" > src/icon.rc
	windres src/icon.rc -O coff -o src/icon.res
g:
	$(compiler) $(flags) -g $(target) $(ldflags)
	gdb color_game
install:
	sudo cp color_game.desktop \
			/usr/share/applications/color\ game.desktop
	sudo mkdir /usr/local/share/color_game
	sudo cp icon.png /usr/local/share/color_game/
	sudo cp -rf maps /usr/local/share/color_game/
	sudo cp color_game /usr/local/games/
	sudo cp example.txt /usr/local/share/color_game/
	sudo cp color_game.3 /usr/local/man/man3/
uninstall:
	sudo rm /usr/share/applications/color\ game.desktop
	sudo rm -rf /usr/local/share/color_game
	sudo rm /usr/local/games/color_game
	sudo rm /usr/local/man/man3/color_game.3
clean:
	rm color_game
