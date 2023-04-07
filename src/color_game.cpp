// FLTK headers.
#include "FL/Fl.H"
#include "FL/Fl_Double_Window.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Button.H"
#include "FL/Fl_Output.H"
#include "FL/Fl_Multiline_Output.H"
#include "FL/Fl_File_Browser.H"
#include "FL/Fl_PNG_Image.H"

#ifndef FL_BROWN
	#define FL_BROWN 0x65432100
#endif

#ifdef _WIN32
	#define dir_to_map_prf "%smaps\\"
	#include <windows.h>
	#include "FL/x.H"
#else
	#define dir_to_map_prf "%smaps/"
#endif

// Main Folder.
// MAINDIR must end with / or \ (windows) and be a full path.
#ifdef _WIN32
	#define MAINDIR ".\\"
#else
	#define MAINDIR "/usr/local/share/color_game/"
#endif

#define check_game_speed 5.0
#define max_map 50
#define button_gap 10
// buttet_height is the fraction of bside.
#define button_height 7

// Max blocks.
int MBLOCKS;

// Window.
Fl_Double_Window * _window;

// Color boxes.
class ColorBox;
ColorBox * boxs[50][50];
ColorBox * overlap_wid;

// Buttons.
Fl_Button * startb;
Fl_Button * exit_button;
Fl_Button * fullsc_button;
Fl_Button * browse_maps;

// Output.
Fl_Multiline_Output * _output;

// Browser.
class MapBrowser;
MapBrowser * map_browser;

// Error box.
Fl_Button * ok_button;
Fl_Output * error_msg;

// Game over
Fl_Button * game_over;

// Screen size and flags.
int bside, bsize;
bool g_active, ext_used;

// Std lib.
#include <cstdlib>
#include <cstdio>
#include <cstring>

// My headers.
#include "handle_json.h"
#include "map.h"
#include "color_box.h"
#include "input.h"
#include "window.h"

int main(int argv, char ** args) {
	Fl::background(0xC0, 0xC0, 0xC0);
	g_active = false;
	ext_used = false;

	_window = new MyWindow(Fl::w() / 2, Fl::h() / 2, "color game");
	_window->xclass("color game");

	bside = (_window->w() - _window->h()) / 2;
	bsize = _window->h() / MBLOCKS;

	int bw, by;
	bw = bside - (bside / 3);
	by = bside / button_height;

	add_boxs();

	// Start button.
	startb = new Fl_Button((_window->w() / 2) - (bside / 2),
			(_window->h() / 2) - (bside / 4), bside, bside / 2, "Start");
	startb->labelsize(bw / 4);
	startb->color(FL_BLUE);
	startb->labelcolor(FL_WHITE);
	startb->callback(startb_cb);
	startb->box(FL_ROUNDED_BOX);
	act_boxs(false);

	// Exit button.
	exit_button = new Fl_Button(0, 0, bw, by, "Exit");
	exit_button->labelsize(bw / 4);
	exit_button->callback(exit_cb);
	exit_button->shortcut(FL_Escape);

	// Fullscreen button.
	fullsc_button = new Fl_Button(0, 0, bw, by, "Fullscreen");
	fullsc_button->labelsize(bw / 6);
	fullsc_button->callback(fullsc_cb);
	fullsc_button->shortcut(FL_F + 11);

	// Output.
	_output = new Fl_Multiline_Output((_window->w() - bside) + 10, 10, \
			bside - 20, _window->h() - 20);
	_output->textsize(_output->w() / 8);

	// Browser button.
	browse_maps = new Fl_Button(0, 0, bw, by, "maps");
	browse_maps->labelsize(browse_maps->w() / 5);
	browse_maps->when(FL_WHEN_CHANGED);
	browse_maps->callback(browse_cb);

	// Overlap.
	overlap_wid = new ColorBox(0, 0, 0, 0);

	// Browser.
	map_browser = new MapBrowser((_window->w() / 2) - (_window->w() / 4),
			(_window->h() / 2) - (_window->h() / 4), _window->w() / 2,
			_window->h() / 2, "Map Browser");

	// Error box.
	int e_w, e_h, e_x, e_y;
	e_w = _window->w() / 5;
	e_h = _window->h() / 8;
	e_x = (_window->w() / 2) - (e_w / 2);
	e_y = (_window->h() / 2) - (e_h / 2);

	error_msg = new Fl_Output(e_x, e_y, e_w, e_h);
	error_msg->hide();

	ok_button = new Fl_Button(e_x, e_y + e_h, e_w, e_h, "Ok");
	ok_button->callback(error_cb);
	ok_button->hide();

	// Game over.
	int gx, gy, gw, gh;
	gw = _window->w() / 3;
	gh = _window->h() / 5;
	gx = (_window->w() / 2) - (gw / 2);
	gy = (_window->h() / 2) - (gh / 2);

	game_over = new Fl_Button(gx, gy, gw, gh, "You Win! (:");
	game_over->box(FL_UP_BOX);
	game_over->labeltype(FL_SHADOW_LABEL);
	game_over->labelsize(gw / 7);
	game_over->hide();
	game_over->callback(startb_cb);

	_window->end();
	_window->resizable(_window);
	((MyWindow*)_window)->reset_sizes();

	// Setting window icon.
	#ifdef _WIN32
		_window->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
	#else
		char icon_image_path[255];
		sprintf(icon_image_path, "%sicon.png", MAINDIR);
		Fl_PNG_Image * icon_image = 
			new Fl_PNG_Image((const char*)icon_image_path);

		if (!icon_image->fail())
			_window->icon(icon_image);
	#endif

	_window->show(argv, args);
	return Fl::run();
}
