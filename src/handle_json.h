#include "json.h"

bool get_jsonf_size(json_value * value, int * w, int * h) {
	json_object_entry _size = value->u.object.values[0];
	json_object_entry * _size_objects = _size.value->u.object.values;

	if (strcmp(_size.name, "size"))
		return true;

	// Width.
	if (!strcmp(_size_objects[0].name, "w"))
		*w = _size_objects[0].value->u.integer;
	else if (!strcmp(_size_objects[0].name, "h"))
		*h = _size_objects[0].value->u.integer;
	else
		return true;

	// Height
	if (!strcmp(_size_objects[1].name, "w"))
		*w = _size_objects[1].value->u.integer;
	else if (!strcmp(_size_objects[1].name, "h"))
		*h = _size_objects[1].value->u.integer;
	else
		return true;

	if (*w > 255 || *w < 2 || *h > 255 || *h < 2)
		return true;

	return false;
}

bool read_data_jsonf(json_value * value, int w, int h, unsigned char ** buf) {
	json_object_entry _size = value->u.object.values[0];
	json_object_entry _map = value->u.object.values[1];
	json_value ** astart = _map.value->u.array.values;

	if (strcmp(_size.name, "size") ||
			strcmp(_map.name, "map"))
		return true;

	unsigned char y, x;
	long int int_value;

	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++) {

			if (astart[y]->u.array.values[x]->type != json_integer)
				return true;

			int_value = astart[y]->u.array.values[x]->u.integer;

			if (int_value > 8 || int_value < 0)
				return true;
			buf[y][x] = (unsigned char)int_value;
		}

	return false;
}
