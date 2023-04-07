/*
Warning ugly code on line 248 turn back now if u 
want to keep your joy and happens.
*/

void show_error(const char * label) {
	ok_button->show();
	ok_button->labelsize(ok_button->w() / 5);
	error_msg->show();
	error_msg->value(label);

	if (!g_active)
		startb->do_callback();

	act_boxs(false);
}

void error_cb(Fl_Widget * w, void * d) {
	ok_button->hide();
	error_msg->hide();
	act_boxs(true);
}

class MapBrowser : public Fl_File_Browser {
	public:
		MapBrowser(int _x, int _y, int _w, int _h, const char * l=0) :
			Fl_File_Browser(_x, _y, _w, _h, l) {
				sprintf(mapdir, dir_to_map_prf, MAINDIR);

				load(mapdir);
				type(FL_HOLD_BROWSER);
				hide();
				callback(map_browser_cb, (void*)this);

				int bw, bh;
				bw = w() / 5;
				bh = h() / 5;

				close_button = new Fl_Button(x(), y() - bh, bw, bh,
						"close");
				close_button->callback(close_cb, (void*)this);
				close_button->labelsize(bw / 3);
				close_button->hide();

				if (text(1))
					if (!strcmp(text(1), "../"))
						remove(1);
			}

		static void map_browser_cb(Fl_Widget * w, void * d) {
			MapBrowser * br = (MapBrowser*)d;
			br->real_cb();
		}

		void show_all() {
			close_button->show();
			show();
			act_boxs(false);
		}

		void hide_all() {
			close_button->hide();
			hide();

			if (g_active)
				act_boxs(true);
		}

		bool is_visible() {
			if (close_button->visible())
				return true;
			else
				return false;
		}

		static void close_cb(Fl_Widget * w, void * d) {
			MapBrowser * br = (MapBrowser*)d;
			br->hide_all();
		}

		void handle_resize() {
			close_button->labelsize(close_button->w() / 3);
		}

		void real_cb();
	private:
		Fl_Button * close_button;
		int last_clicked;
		char mapdir[255];
};

void MapBrowser::real_cb() {

	if (!text(value()))
		return;
	else if (last_clicked != value()) {
		last_clicked = value();
		return;
	}

	last_clicked = -1;
	map_browser->hide_all();

	// Loading json.
	char filename[255];
	sprintf(filename, "%s%s", mapdir, text(value()));

	FILE * fp = fopen(filename, "rt");

	if (!fp) {
		show_error("File not found");
		fclose(fp);
		return;
	}

	long int file_size = findsize(fp);
	char file_buf[file_size];
	file_buf[0] = '\0';
	file_buf[file_size] = '\0';
	int a_w, a_h, c, r;

	fread(file_buf, file_size, 1, fp);
	fclose(fp);

	json_value * _value = json_parse(file_buf, file_size);

	if (!_value) {
		show_error("Can't read map");
		return;
	}

	if (get_jsonf_size(_value, &a_w, &a_h)) {
		show_error("Bad formatt");
		return;
	}

	// Making 2d array.
	unsigned char ** the_map = new unsigned char*[a_h];
	unsigned char i;

	if (the_map == NULL) {
		show_error("Can't malloc data");
		delete [] the_map;
		return;
	}

	for (i = 0; i < a_h; i++)
		the_map[i] = new unsigned char[a_w];

	// Reading data.
	if (read_data_jsonf(_value, a_w, a_h, the_map)) {
		show_error("Bad formatt");

		// Deleting memory.
		for (i = 0; i < a_h; i++) {
			delete [] the_map[i];
		}

		delete [] the_map;

		return;
	}

	// Settings Max blocks.
	if (a_w > a_h)
		MBLOCKS = a_w;
	else
		MBLOCKS = a_h;
	
	if (a_w > max_map || a_h > max_map) {
		// Deleting memory.
		for (i = 0; i < a_h; i++) {
			delete [] the_map[i];
		}
		
		delete [] the_map;

		loaded_map = false;
		map_init();
		add_boxs();

		show_error("Map is to big");
		return;
	}

	map_init(the_map, a_w, a_h);

	// Deleting memory.
	for (i = 0; i < a_h; i++) {
		delete [] the_map[i];
	}
	
	delete [] the_map;
	loaded_map = true;

	add_boxs();

	// Activate game.
	if (!g_active)
		startb->do_callback();
	g_active = true;
}

void startb_cb(Fl_Widget * w, void * d) {
	int bw, by;
	bw = bside - (bside / 3);
	by = bside / 5;
	ext_used = false;

	if (!g_active) {
		g_active = true;
		startb->label("Restart");
		startb->labelsize(bw / 5);
		startb->resize(10, 10, exit_button->w(),
				exit_button->h());
		startb->color(FL_GRAY);
		startb->labelcolor(FL_BLACK);
		startb->box(FL_UP_BOX);
		act_boxs(true);
	}

	overlap_wid->resize(0, 0, 0, 0);
	map_init();
	add_boxs();

	game_over->hide();
}

void exit_cb(Fl_Widget * w, void * d) {
	_window->hide();
}

void fullsc_cb(Fl_Widget * w, void * d) {
	if (_window->fullscreen_active())
		_window->fullscreen_off();
	else
		_window->fullscreen();
}

void browse_cb(Fl_Widget * w, void * d) {
	/*
	My compiler was being stupid. The extistence of 
	this code makes me angery.
	It should look like this:
	game_over->hide();
	Why do I have to do this!
	*/
	if (game_over->visible())
		startb->do_callback();
	// The saddness is over for now.
	
	map_browser->show_all();
}
