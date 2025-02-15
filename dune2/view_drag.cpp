#include "draw.h"
#include "view_drag.h"

using namespace draw;

point mouse_drag_start = {-10000, -10000};

bool mouse_drag_active() {
	if(mouse_drag_start == point(-10000, -10000))
		return false;
	if((hot.key == MouseLeft || hot.key == MouseRight) && !hot.pressed) {
		mouse_drag_start = point(-10000, -10000);
		return false;
	}
	return true;
}

bool mouse_drag_begin() {
	if(mouse_drag_start == point(-10000, -10000)) {
		if(hot.key == MouseLeft && hot.pressed) {
			mouse_drag_start = hot.mouse;
			hot.key = InputUpdate;
			return true;
		}
	}
	return false;
}