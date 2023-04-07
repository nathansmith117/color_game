#define RED 1
#define YELLOW 2
#define GREEN 3
#define BLUE 4

#define WHITE 5
#define BLACK 6
#define BROWN 7
#define BLANK 8

// 0 == not part of the map.

unsigned char info_output[9];
unsigned char all_colors[9];
unsigned char color_map[max_map][max_map];
unsigned char old_color_map[max_map][max_map];
bool loaded_map;
int map_x_offset, map_y_offset;

unsigned char default_map[9][8] = {
	{0, 0, 0, 0, 0, 0, 8, 8}, // 1
	{0, 0, 4, 3, 2, 0, 8, 4}, // 2
	{6, 8, 8, 8, 8, 8, 8, 0}, // 3
	{0, 8, 0, 8, 0, 8, 8, 0}, // 4
	{0, 1, 8, 8, 8, 8, 0, 0}, // 5
	{0, 0, 8, 0, 8, 8, 8, 8}, // 6
	{0, 8, 8, 0, 8, 8, 8, 5}, // 7
	{0, 0, 8, 0, 8, 0, 0, 0}, // 8
	{0, 0, 2, 7, 8, 0, 0, 0} // 9
};

const char * color_to_label(unsigned char _color) {
	switch (_color) {
		case RED:
			return "R";
		case YELLOW:
			return "Y";
		case GREEN:
			return "G";
		case BLUE:
			return "B";
		case WHITE:
			return "W";
		case BLACK:
			return "X";
		case BROWN:
			return "N";
		default:
			return "\0";
	}
}

Fl_Color color_to_flcolor(unsigned char _color) {
	switch (_color) {
		case RED:
			return FL_RED;
		case YELLOW:
			return FL_YELLOW;
		case GREEN:
			return FL_GREEN;
		case BLUE:
			return FL_BLUE;
		case WHITE:
			return FL_WHITE;
		case BLACK:
			return FL_BLACK;
		case BROWN:
			return FL_BROWN;
		default:
			return FL_GRAY;
	}
}

long int findsize(FILE * fp) {
	fseek(fp, 0L, SEEK_END);
	long int _size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return _size;
}

void map_init(unsigned char ** new_map=NULL, int a_w=0, int a_h=0) {
	memset(color_map, 0, sizeof(color_map));
	memset(all_colors, 0, sizeof(all_colors));
	ext_used = false;

	int x, y;

	/*
	Sorry this function is a bit hacky.
	It was made for testing but then made 
	it to this version.
	*/

	// Loading new maps.
	if (new_map || loaded_map) {

		// For rectangle shaped maps.
		if (new_map) {
			map_x_offset = 0;
			map_y_offset = 0;

			if (a_w < MBLOCKS)
				map_x_offset = MBLOCKS - a_w;
			if (a_h < MBLOCKS)
				map_y_offset = MBLOCKS - a_h;
		}

		for (y = 0; y < MBLOCKS; y++)
			for (x = 0; x < MBLOCKS; x++) {

				if (y + map_y_offset >= MBLOCKS ||
						x + map_x_offset >= MBLOCKS)
					continue;

				if (loaded_map && !new_map)
					color_map[y][x] = old_color_map[y][x];
				else {
					color_map[y][x] = new_map[y][x];
					old_color_map[y][x] = color_map[y][x];
				}

				all_colors[color_map[y][x]]++;
				info_output[color_map[y][x]]++;
			}

		loaded_map = true;
		return;
	}

	// Default map.
	MBLOCKS = 9;
	for (y = 0; y < MBLOCKS; y++)
		for (x = 0; x < MBLOCKS; x++) {

			if (y < 9 && x < 8)
				color_map[y][x] = default_map[y][x];

			all_colors[color_map[y][x]]++;
			info_output[color_map[y][x]]++;
		}
}
