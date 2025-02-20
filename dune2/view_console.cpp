#include "stringbuilder.h"
#include "draw.h"
#include "resid.h"
#include "slice.h"
#include "timer.h"

using namespace draw;
static char temp[1024];
static stringbuilder sb(temp);
static unsigned time_stamp;

const int font_height = 12;
const int font_shift_time = 200;

void paint_console() {
	pushfont push(gres(FONT8));
	pushfore push_fore(colors::black);
	pushrect push_rect;
	auto push_clip = clipping;
	caret.x = 8; caret.y = 23; width = getwidth() - 8 * 2; height = font_height;
	setcliparea();
	auto current_stamp = getcputime();
	if(!time_stamp) {
		auto pn = zchr(sb, '\n');
		if(pn)
			time_stamp = current_stamp;
	} else if((current_stamp - time_stamp) > font_shift_time * font_height) {
		auto pn = zchr(sb, '\n');
		auto sz = zlen(pn);
		auto pb = (char*)sb.begin();
		memcpy(pb, pn + 1, sz - 1);
		sb.set(pb + sz - 1); pb[sz] = 0;
		time_stamp = 0;
	} else
		caret.y -= (short)((current_stamp - time_stamp) / font_shift_time);
	texta(sb, AlignLeft);
	clipping = push_clip;
}

void print_console_proc(const char* string) {
	sb.addn(string);
}