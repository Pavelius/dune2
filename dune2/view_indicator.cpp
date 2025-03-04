#include "draw.h"
#include "resid.h"
#include "view_map.h"
#include "view_indicator.h"

const int indicator_change_time = 300;
const int indicator_digits = 6;
const int indicator_digit_width = 10;

using namespace draw;

static void paint_control(int v) {
	auto push = caret;
	auto ps = gres(SHAPES);
	for(auto i = 0; i < 6; i++) {
		if(i != 0 && !v)
			break;
		image(ps, (v % 10) + 2, 0);
		v = v / 10;
		caret.x -= indicator_digit_width;
	}
	caret = push;
}

static void paint_control(int v1, int v2) {
	auto push = caret;
	auto ps = gres(SHAPES);
	for(auto i = 0; i < 6; i++) {
		auto i1 = v1 % 10; v1 = v1 / 10;
		auto i2 = v2 % 10; v2 = v2 / 10;
		if(i1 != i2) {
			image(ps, i1 + 2, 0);
			image(caret.x, caret.y + 8, ps, i2 + 2, 0);
		}
		caret.x -= indicator_digit_width;
	}
	caret = push;
}

void indicator::paint() {
	pushrect push;
	if(value != next && !time)
		time = animate_time;
	caret.x += indicator_digit_width * indicator_digits - 8;
	caret.y += 2;
	auto t = animate_time - time;
	if(t >= indicator_change_time) {
		time = 0;
		value = next;
	}
	if(value == next)
		paint_control(value);
	else {
		auto push_clip = clipping;
		setclip({push.caret.x, caret.y - 1, push.caret.x + indicator_digits * indicator_digit_width, caret.y + 8});
		if(false) {
			caret.y -= (short)(t * 8 / indicator_change_time);
			paint_control(value);
			caret.y += 8;
			paint_control(next);
		} else {
			paint_control(value);
			caret.y -= (short)(t * 8 / indicator_change_time);
			paint_control(value, next);
		}
		clipping = push_clip;
	}
}