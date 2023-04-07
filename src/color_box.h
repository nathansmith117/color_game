class ColorBox : public Fl_Box {
	public:
		ColorBox(int _x, int _y, int _w, int _h, const char * l=0) :
			Fl_Box(_x, _y, _w, _h, l) {
				box(FL_UP_BOX);
				labeltype(FL_SHADOW_LABEL);
			}

		int handle(int event);
		
		void xy_vars(int _x, int _y) {
			this->_x = _x;
			this->_y = _y;
		}

		void set_overlap(bool overlap) {
			this->overlap = overlap;
		}

		bool set_overlap() {
			return overlap;
		}

		void setbox();
	private:
		int _x, _y;
		bool overlap;
		int old_x, old_y;
};

void add_boxs() {
	int _y, _x, _font_size;
	memset(all_colors, 0, sizeof(all_colors));

	bside = (_window->w() - _window->h()) / 2;
	bsize = _window->h() / MBLOCKS;
	_font_size = bsize / 2;

	for (_y = 0; _y < 50; _y++)
		for (_x = 0; _x < 50; _x++) {

			if (_y + map_y_offset >= MBLOCKS ||
					_x + map_x_offset >= MBLOCKS) {
				if (boxs[_y][_x])
					boxs[_y][_x]->hide();
				continue;
			}

			all_colors[color_map[_y][_x]]++;

			// Add boxes.
			if (!boxs[_y][_x])
				boxs[_y][_x] = new ColorBox(0, 0, 0, 0);

			boxs[_y][_x]->resize((_x * bsize) + bside, _y *
					bsize, bsize, bsize);

			// Activate.
			if (g_active)
				boxs[_y][_x]->activate();

			// Hide some boxes.
			if (color_map[_y][_x] == 0) {
				boxs[_y][_x]->hide();
				continue;
			}

			boxs[_y][_x]->show();

			// Adding color and label to boxes.
			boxs[_y][_x]->label(color_to_label(color_map[_y][_x]));
			boxs[_y][_x]->labelsize(_font_size);
			boxs[_y][_x]->color(color_to_flcolor(color_map[_y][_x]));
			boxs[_y][_x]->xy_vars(_x, _y);
			boxs[_y][_x]->set_overlap(false);

			if (color_map[_y][_x] == BLACK)
				boxs[_y][_x]->labelcolor(FL_GRAY);
			else
				boxs[_y][_x]->labelcolor(FL_BLACK);
		}

	_window->redraw();
}

void act_boxs(bool act) {
	int x, y;

	for (y = 0; y < MBLOCKS; y++)
		for (x = 0; x < MBLOCKS; x++) {
			if (boxs[y][x]) {
				if (act)
					boxs[y][x]->activate();
				else
					boxs[y][x]->deactivate();
			}
		}

	if (overlap_wid)
		overlap_wid->resize(0, 0, 0, 0);
}

int ColorBox::handle(int event) {
	int i, the_x, the_y, real_x, real_y;

	switch(event) {
		case FL_ENTER:
			if (!g_active || !active_r())
				return 1;

			if (!overlap && color() != FL_GRAY && color() != FL_WHITE && 
					color() != FL_BLACK && color() != FL_BROWN) {
				int _font_size = bsize / 2;

				overlap_wid->labelsize(_font_size);
				overlap_wid->xy_vars(_x, _y);
				overlap_wid->resize((_x * bsize) + bside, _y *
						bsize, bsize, bsize);

				overlap_wid->set_overlap(true);
				overlap_wid->color(color());
				overlap_wid->labelcolor(labelcolor());

				overlap_wid->label(label());
			}

			return 1;
		case FL_PUSH:
			return 1;
		case FL_DRAG:
			if (!g_active || !active_r())
				return 1;

			switch(Fl::event_button()) {
				case FL_LEFT_MOUSE:

					if (!overlap)
						break;

					position(Fl::event_x(), Fl::event_y());

					// Stop box from leaving window.
					real_x = x() + (w() / 2);
					real_y = y() + (h() / 2);

					if (real_x <= 0)
						position(3, y());
					if (real_x >= _window->w())
						position(_window->w() - (w() + 3), y());
					if (real_y <= 0)
						position(x(), 3);
					if (real_y >= _window->h())
						position(x(), _window->h() - (h() + 3));

					// Drawing.
					if (old_x && old_y) {
						old_x = (old_x - x()) * 3;
						old_y = (old_y - y()) * 3;

						if (old_x < 0)
							old_x *= -1;
						if (old_y < 0)
							old_y *= -1;

						if (old_x == 0)
							old_x = 100;
						if (old_y == 0)
							old_y = 100;

						_window->damage(FL_DAMAGE_ALL, x() - old_x,
								y() - old_y, w() + (old_x * 2), h()
								+ (old_y * 2));
					} else
						_window->redraw();

					old_x = x();
					old_y = y();

					break;
				default:
					break;
			}

			return 1;
		case FL_RELEASE:

			if (Fl::event_button() == FL_LEFT_MOUSE)
				setbox();

			return 1;
		default:
			return 0;
	}
}

void ColorBox::setbox() {
	int x_block = (x() - bside) / bsize;
	int y_block = y() / bsize;
	int _font_size, repos_t = 0;
	unsigned char ccolor = color_map[_y][_x], chc = 0;
	bool repos = false;

	if (!overlap)
		return;
	else if (!boxs[y_block][x_block]) {
		resize(0, 0, 0, 0);
		_window->redraw();
		return;
	}

	resize(0, 0, 0, 0);

	// Color combo.
	switch (color_map[y_block][x_block]) {
		case BLANK:
			repos = true;
			break;
		case RED:
			if (ccolor == GREEN)
				repos = true;
			break;
		case YELLOW:
			if (ccolor == RED)
				repos = true;
			break;
		case GREEN:
			if (ccolor == BLUE)
				repos = true;
			break;
		case BLUE:
			if (ccolor == YELLOW)
				repos = true;
			break;
		case WHITE:
			chc = 1;
			repos = true;
			break;
		case BLACK:
			chc = 2;
			repos = true;
			break;
		default:
			break;
	}

	if (!repos) {
		_window->redraw();
		return;
	}

	// Distances.
	if (x_block > _x) {
		if (x_block - _x == 1)
			repos = true;
		else
			repos = false;
		repos_t++;
	} if (_x > x_block) {
		if (_x - x_block == 1)
			repos = true;
		else
			repos = false;
		repos_t++;
	} if (y_block > _y) {
		if (y_block - _y == 1)
			repos = true;
		else
			repos = false;
		repos_t++;
	} if (_y > y_block) {
		if (_y - y_block == 1)
			repos = true;
		else
			repos = false;
		repos_t++;
	}

	// repos_t stops diagonal movemet.
	if (repos_t > 1)
		repos = false;
	else if (repos && chc > 0) {
		unsigned char new_color;

		// White.
		if (chc == 1) {
			switch (ccolor) {
				case RED:
					new_color = BLUE;
					break;
				case YELLOW:
					new_color = GREEN;
					break;
				case GREEN:
					new_color = YELLOW;
					break;
				case BLUE:
					new_color = RED;
					break;
				default:
					break;
			}
		// Black.
		} else if (chc == 2) {
			switch(ccolor) {
				case RED:
					new_color = GREEN;
					break;
				case YELLOW:
					new_color = RED;
					break;
				case GREEN:
					new_color = BLUE;
					break;
				case BLUE:
					new_color = YELLOW;
					break;
				default:
					break;
			}
		}

		all_colors[new_color]++;
		all_colors[color_map[y_block][x_block]]--;

		color_map[y_block][x_block] = new_color;
		int _font_size = bsize / 2;

		boxs[y_block][x_block]->labelsize(_font_size);
		boxs[y_block][x_block]->color(color_to_flcolor(new_color));
		boxs[y_block][x_block]->label(color_to_label(new_color));
		boxs[y_block][x_block]->labelcolor(FL_BLACK);

	} else if (repos) {
		all_colors[color_map[_y][_x]]++;
		all_colors[color_map[y_block][x_block]]--;

		color_map[y_block][x_block] = color_map[_y][_x];
		_font_size = bsize / 2;

		boxs[y_block][x_block]->labelsize(_font_size);
		boxs[y_block][x_block]->color(color());
		boxs[y_block][x_block]->labelcolor(labelcolor());
		boxs[y_block][x_block]->label(label());
	}

	int da_size = (bsize * 2 + 10);

	_window->damage(FL_DAMAGE_ALL, boxs[y_block][x_block]->x() - da_size,
			boxs[y_block][x_block]->y() - da_size, boxs[y_block][x_block]->w() + da_size * 2,
			boxs[y_block][x_block]->h() + da_size * 2);
}
