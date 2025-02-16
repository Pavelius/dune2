#include "draw.h"
#include "math.h"
#include "view.h"
#include "view_theme.h"
#include "view_list.h"

using namespace draw;

int list_hilite;

void correct_list(int& origin, int maximum, int per_page) {
	if(origin + per_page >= maximum)
		origin = maximum - per_page;
	if(origin < 0)
		origin = 0;
}

static void mouse_input_list(int& origin, int maximum, int row_height) {
	if(!ishilite())
		return;
	auto per_row = width / row_height;
	auto per_page = height / row_height;
	switch(hot.key) {
	case MouseWheelUp: execute(cbsetint, origin - 1, 0, &origin); break;
	case MouseWheelDown: execute(cbsetint, origin + 1, 0, &origin); break;
	default: break;
	}
}

static void mouse_input_list(int& origin, int maximum, point size) {
	if(!size.x || !size.y)
		return;
	if(!ishilite())
		return;
	auto per_row = imax(1, width / size.x);
	auto per_page = (height / size.y) * per_row;
	switch(hot.key) {
	case MouseWheelUp: execute(cbsetint, origin - per_row, 0, &origin); break;
	case MouseWheelDown: execute(cbsetint, origin + per_row, 0, &origin); break;
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
	if(button("^", 0, AlignCenterCenter))
		execute(cbsetint, origin - 1, 0, &origin);
	// Paint scroll down button
	caret.y = push.caret.y + push.height - height;
	if(button("^", 0, AlignCenterCenter))
		execute(cbsetint, origin + 1, 0, &origin);
	// Paint slider zone
	caret.y = push.caret.y + dy;
	height = slide_height;
	auto push_alpha = alpha; alpha = 128;
	rectf();
	alpha = push_alpha;
	// Paint slider
	auto bar_position = slide_height * origin / maximum;
	auto bar_height = slide_height * per_page / maximum + 1;
	caret.y = push.caret.y + dy;
	mouse_input_scroll(origin, maximum, per_page, slide_height, bar_position, bar_height);
	caret.y += bar_position;
	height = bar_height;
	button(0, 0, 0);
}

static void rectf_hilite() {
	auto push_alpha = alpha; alpha = hot.pressed ? 64 : 32;
	auto push_fore = fore; fore = colors::white;
	rectf();
	fore = push_fore;
	alpha = push_alpha;
}

void paint_list(int& origin, int maximum, void* elements, size_t element_size, int row_height, fnlistrow proc, fnlistrowallow proc_allow, fnevent choose) {
	if(!height || !width || !row_height)
		return;
	pushrect push;
	auto push_clip = clipping; setcliparea();
	auto per_page = height / row_height;
	correct_list(origin, maximum, per_page);
	mouse_input_list(origin, maximum, row_height);
	if(!ishilite())
		list_hilite = -1;
	else
		list_hilite = origin + (hot.mouse.y - caret.y) / row_height;
	height = row_height;
	for(auto i = origin; i < maximum; i++) {
		if(i > origin + per_page)
			break;
		auto pd = (unsigned char*)elements + element_size * i;
		auto allow = proc_allow ? proc_allow(i, pd) : true;
		if(i == list_hilite) {
			if(allow)
				rectf_hilite();
		}
		proc(i, pd);
		if(allow && choose && i == list_hilite) {
			if(hot.key == MouseLeft && !hot.pressed)
				execute(choose, (long)pd, i, 0);
		}
		caret.y += row_height;
	}
	clipping = push_clip;
}

static void rectb_hilite(int thickness, color main) {
	pushrect push;
	auto push_fore = fore; fore = main;
	for(auto i = 0; i < thickness; i++) {
		setoffset(-1, -1);
		rectb();
	}
	fore = push_fore;
}

void paint_list(int& origin, int& current, int maximum, void* elements, size_t element_size, color hilite, point size, point offset, fnlistrow proc) {
	if(!height || !width || !size.y || !size.x)
		return;
	pushrect push;
	auto push_clip = clipping; setcliparea();
	auto per_row = imax(1, width / size.x);
	auto per_page = (height / size.y) * per_row;
	correct_list(origin, maximum, per_page);
	mouse_input_list(origin, maximum, size);
	list_hilite = -1;
	height = size.y;
	width = size.x;
	for(auto i = origin; i < maximum; i++) {
		if(i > origin + per_page)
			break;
		caret.x = push.caret.x + ((i - origin) % per_row) * size.x + offset.x;
		caret.y = push.caret.y + ((i - origin) / per_row) * size.y + offset.y;
		width = size.x - offset.x * 2;
		height = size.y - offset.y * 2;
		if(ishilite())
			list_hilite = i;
		auto pd = (unsigned char*)elements + element_size * i;
		proc(i, pd);
		if(i == current)
			rectb_hilite(2, hilite);
		if(i == list_hilite) {
			rectb_hilite(1, hilite);
			if(hot.key == MouseLeft && !hot.pressed)
				execute(cbsetint, i, 0, &current);
		}
	}
	clipping = push_clip;
}

void paint_list_and_scroll(int& origin, int maximum, void* elements, size_t element_size, int row_height, fnlistrow proc, fnlistrowallow allow, fnevent choose) {
	pushrect push;
	paint_list(origin, maximum, elements, element_size, row_height, proc, allow, choose);
	caret.x += width; width = 8;
	paint_scroll(origin, maximum, row_height);
}