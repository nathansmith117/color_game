class MyWindow : public Fl_Double_Window {
	public:
		MyWindow(int _w, int _h, const char * l=0) : 
			Fl_Double_Window(_w, _h, l) {
				color(FL_GRAY);
				map_init();
				old_w = w();
				old_h = h();
				Fl::add_timeout(1.0/check_game_speed, time_cb, (void*)this);
			}

		static void time_cb(void * data) {
			MyWindow * win = (MyWindow*)data;
			win->real_time_cb();
			Fl::repeat_timeout(1.0/check_game_speed, time_cb, data);
		}

		void reset_sizes();
		void real_time_cb();
	private:
		int old_w, old_h;
};

void update_output() {
	char buf[10], end_buf[100], clabel[8];
	unsigned char i;
	buf[0] = '\0';
	end_buf[0] = '\0';
	clabel[0] = '\0';

	for (i = 1; i < 9; i++) {
		info_output[i] = all_colors[i];
		clabel[0] = '\0';

		// Label.
		switch(i) {
			case BLANK:
				strcat(clabel, "Blank");
				break;
			default:
				strcat(clabel, color_to_label(i));
				break;
		}

		sprintf(buf, "%s %d\n", clabel, all_colors[i]);
		strcat(end_buf, buf);
	}

	_output->value(end_buf);
}

void check_game() {
	unsigned char i, ext_color = 0;
	bool updated = false;

	// Checking colors and for game over.
	for (i = 0; i < 9; i++) {
		// Updating output.
		if (info_output[i] != all_colors[i] && !updated) {
			update_output();
			updated = true;
		// Game over.
		} if (all_colors[i] + all_colors[0] == (MBLOCKS -
				map_x_offset) * (MBLOCKS - map_y_offset)) {
			act_boxs(false);
			game_over->show();
			break;
		// Extinct color.
		} if (all_colors[i] == 0 && i > 0 && i <= 4 && !ext_used) {
			ext_color = i;
			break;
		}
	}

	if (!ext_color)
		return;

	int num_of_browns = 0, x, y, max_browns = all_colors[BROWN];

	for (y = 0; y < MBLOCKS; y++)
		for (x = 0; x < MBLOCKS; x++) {

			if (color_map[y][x] == BROWN) {
				color_map[y][x] = ext_color;
				boxs[y][x]->color(color_to_flcolor(ext_color));
				boxs[y][x]->label(color_to_label(ext_color));
				all_colors[ext_color]++;
				num_of_browns++;
			}

			if (num_of_browns >= max_browns)
				break;
		}

	all_colors[BROWN] = 0;
	ext_used = true;
}

void MyWindow::reset_sizes() {
	// Varibles.
	old_w = w();
	old_h = h();
	bside = (_window->w() - _window->h()) / 2;
	bsize = _window->h() / MBLOCKS;

	int bw, by;
	bw = bside - (bside / 3);
	by = bside / button_height;

	// Overlap.
	overlap_wid->resize(0, 0, 0, 0);

	// Start button position.
	if (g_active)
		startb->resize(10, 10, exit_button->w(), exit_button->h());

	startb->labelsize(startb->w() / 4);

	// Exit button.
	exit_button->labelsize(exit_button->w() / 4);
	exit_button->position(10, 10 + button_gap + exit_button->h());

	// Fullscreen button.
	fullsc_button->labelsize(fullsc_button->w() / 6);
	fullsc_button->position(10, exit_button->y() + exit_button->h() + button_gap);

	// Browse button.
	browse_maps->labelsize(browse_maps->w() / 5);
	browse_maps->position(10, fullsc_button->y() + fullsc_button->h() + button_gap);

	// Map browser.
	map_browser->handle_resize();

	// Output.
	_output->textsize(_output->w() / 8);

	// Error.
	ok_button->labelsize(ok_button->w() / 5);

	// Game over.
	game_over->labelsize(game_over->w() / 7);

	add_boxs();

	if (map_browser->is_visible())
		act_boxs(false);
}

void MyWindow::real_time_cb() {
	bool dont_resize = false;

	// Testing screen size.
	float screen_ra = (float)w() / (float)h();

	if (screen_ra > 2.0 || screen_ra < 1.75) {
		size(w(), int((float)w() / 1.756653));
		dont_resize = true;
	}

	// Window resizing.
	if ((old_w != w() || old_h != h()) && !dont_resize) {
		reset_sizes();
	}

	check_game();
}
