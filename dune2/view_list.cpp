#include "draw.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

void correct_list(int& origin, int maximum, int per_page) {
	if(origin + per_page >= maximum)
		origin = maximum - per_page;
	if(origin < 0)
		origin = 0;
}

static void mouse_input_list(int& origin, int maximum, int row_height) {
	if(!ishilite())
		return;
	auto per_page = height / row_height;
	switch(hot.key) {
	case MouseWheelUp: execute(cbsetint, origin - 1, 0, &origin); break;
	case MouseWheelDown: execute(cbsetint, origin + 1, 0, &origin); break;
	default: break;
	}
}

static bool mouse_drag_change() {
	static point mouse_grad_update;
	if(mouse_grad_update != hot.mouse) {
		mouse_grad_update = hot.mouse;
		return true;
	}
	return false;
}

static void mouse_input_scroll(int& origin, int maximum, int per_page, int slider_height, int bar_position, int bar_height) {
	static int bar_position_start;
	static point mouse_start;
	pushrect push;
	rect ru = {caret.x, caret.y, caret.x + width, caret.y + bar_position - 1};
	rect rb = {caret.x, caret.y + bar_position, caret.x + width, caret.y + bar_position + bar_height - 1};
	rect rd = {caret.x, caret.y + bar_position + bar_height, caret.x + width, caret.y + slider_height};
	if(dragactive(mouse_input_scroll)) {
		if(mouse_drag_change()) {
			auto new_origin = (bar_position_start + (hot.mouse.y - mouse_start.y)) * maximum / slider_height;
			execute(cbsetint, new_origin, 0, &origin);
		}
	} else {
		if(hot.key == MouseLeft && hot.mouse.in(ru)) {
			if(!hot.pressed)
				execute(cbsetint, origin - per_page + 1, 0, &origin);
		} else if(hot.key == MouseLeft && hot.mouse.in(rd)) {
			if(!hot.pressed)
				execute(cbsetint, origin + per_page - 1, 0, &origin);
		} else if(hot.mouse.in(rb)) {
			if(dragbegin(mouse_input_scroll)) {
				mouse_start = hot.mouse;
				bar_position_start = bar_position;
			}
		}
	}
}

static void paint_scroll(int& origin, int maximum, int row_height) {
	if(!maximum)
		return;
	mouse_input_list(origin, maximum, row_height);
	pushrect push;
	auto per_page = height / row_height;
	auto dy = width; height = dy;
	if(per_page >= maximum)
		return;
	auto slide_height = push.height - height * 2;
	// Paint scroll up button
	button("^", &origin, 0, AlignCenterCenter, false, cbsetint, origin - 1);
	// Paint scroll down button
	caret.y = push.caret.y + push.height - height - 1;
	button("U", &origin, 0, AlignCenterCenter, false, cbsetint, origin + 1);
	// Paint slider zone
	caret.y = push.caret.y + dy;
	setoffset(1, 0);
	height = slide_height;
	auto push_alpha = alpha; alpha = 128;
	rectf();
	alpha = push_alpha;
	// Paint slider
	auto bar_position = slide_height * origin / maximum;
	auto bar_height = slide_height * per_page / maximum + 2;
	caret.y = push.caret.y + dy;
	mouse_input_scroll(origin, maximum, per_page, slide_height, bar_position, bar_height);
	caret.y += bar_position;
	height = bar_height;
	setoffset(-1, 0);
	buttonwr(0, 0, 0, 0);
}

void paint_list(int& origin, int maximum, void* elements, size_t element_size, int row_height, fnlistrow proc) {
	if(!height || !width || !row_height)
		return;
	pushrect push;
	auto push_clip = clipping; setcliparea();
	auto per_page = height / row_height;
	correct_list(origin, maximum, per_page);
	mouse_input_list(origin, maximum, row_height);
	for(auto i = origin; i < maximum; i++) {
		if(i > origin + per_page)
			break;
		proc(i, (unsigned char*)elements + element_size * i);
		caret.y += row_height;
	}
	clipping = push_clip;
}

void paint_list_and_scroll(int& origin, int maximum, void* elements, size_t element_size, int row_height, fnlistrow proc) {
	pushrect push;
	paint_list(origin, maximum, elements, element_size, row_height, proc);
	caret.x += width; width = 12;
	paint_scroll(origin, maximum, row_height);
}