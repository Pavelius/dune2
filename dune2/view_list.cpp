#include "draw.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

void correct_list(int& origin, int& current, int maximum, int per_page) {
	if(current < 0)
		current = 0;
	else if(current > maximum - 1)
		current = maximum - 1;
	if(current < origin)
		origin = current;
	else if(current >= origin + per_page)
		origin = current - per_page;
}

static void paint_scroll(int& origin, int& current, int maximum, int row_height) {
	pushrect push;
	auto dy = width; height = dy;
	auto per_page = height / row_height;
	button("U", 0, 0, AlignCenterCenter, false, cbsetint, origin - 1);
}

void paint_list(int& origin, int& current, int maximum, void* elements, size_t element_size, int row_height, fnlistrow proc) {
	if(!height || !width || !row_height)
		return;
	pushrect push;
	auto push_clip = clipping; setcliparea();
	auto per_page = height / row_height;
	correct_list(origin, current, maximum, per_page);
	for(auto i = origin; i < maximum; i++) {
		if(i > origin + per_page)
			break;
		proc(i, (unsigned char*)elements + element_size * i);
		caret.y += row_height;
	}
	clipping = push_clip;
}

void paint_list_and_scroll(int& origin, int& current, int maximum, void* elements, size_t element_size, int row_height, fnlistrow proc) {
	pushrect push;
	paint_list(origin, current, maximum, elements, element_size, row_height, proc);
	caret.x += width; width = 8;
	paint_scroll(origin, current, maximum, row_height);
}