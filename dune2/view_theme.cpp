#include "bsdata.h"
#include "draw.h"
#include "view_focus.h"
#include "view_theme.h"

using namespace draw;

color form_button, form_button_light, form_button_dark;
color font_pallette[16];

pushtheme::pushtheme(themen id) : form(form_button), dark(form_button_dark), light(form_button_light), fore(draw::fore) {
	auto& ei = bsdata<themei>::elements[id];
	draw::fore = ei.fore;
	form_button = ei.form;
	form_button_dark = ei.dark;
	form_button_light = ei.light;
}

pushtheme::~pushtheme() {
	draw::fore = fore;
	form_button = form;
	form_button_light = light;
	form_button_dark = dark;
}

pushfontb::pushfontb(themen id) : font(draw::font), fpal1(font_pallette[1]), fpal5(font_pallette[5]), fpal6(font_pallette[6]), palt(draw::palt) {
	auto& ei = bsdata<themei>::elements[id];
	draw::palt = font_pallette;
	font_pallette[1] = ei.form;
	font_pallette[5] = ei.light;
	font_pallette[6] = ei.dark;
}

pushfontb::~pushfontb() {
	draw::palt = palt;
	draw::font = font;
	font_pallette[1] = fpal1;
	font_pallette[5] = fpal5;
	font_pallette[6] = fpal6;
}

void copybits(int x, int y, int width, int height, int x1, int y1) {
	if(x == x1 && y == y1)
		return;
	auto ps = canvas->ptr(x, y);
	auto pd = canvas->ptr(x1, y1);
	auto sn = canvas->scanline;
	auto sz = width * sizeof(color);
	if(y < y1) {
		ps = canvas->ptr(x, y + height - 1);
		pd = canvas->ptr(x1, y1 + height - 1);
		sn = -sn;
	}
	for(auto i = 0; i < height; i++) {
		memmove(pd, ps, sz);
		ps += sn;
		pd += sn;
	}
}

void fillbitsh(int x, int y, int width, int height, int total_width) {
	total_width -= width;
	if(total_width <= 0 || width <= 0)
		return;
	auto x1 = x + width;
	for(auto n = total_width / width; n > 0; n--) {
		copybits(x, y, width, height, x1, y);
		x1 += width;
		total_width -= width;
	}
	if(total_width)
		copybits(x, y, total_width, height, x1, y);
}

void fillbitsv(int x, int y, int width, int height, int total_height) {
	total_height -= height;
	if(total_height <= 0 || height <= 0)
		return;
	auto y1 = y + height;
	for(auto n = total_height / height; n > 0; n--) {
		copybits(x, y, width, height, x, y1);
		y1 += height;
		total_height -= height;
	}
	if(total_height)
		copybits(x, y, width, total_height, x, y1);
}

void rectf(color v) {
	pushfore push(v);
	rectf();
}

void form_frame(color light_left_up, color shadow_right_down) {
	auto push_fore = fore;
	auto push_caret = caret;
	fore = light_left_up;
	line(caret.x + width - 1, caret.y);
	fore = shadow_right_down;
	line(caret.x, caret.y + height - 1);
	line(caret.x - (width - 1), caret.y);
	fore = light_left_up;
	line(caret.x, caret.y - (height - 2));
	fore = light_left_up.mix(shadow_right_down, 128);
	pixel(push_caret.x + width - 1, push_caret.y);
	pixel(push_caret.x, push_caret.y + height - 1);
	caret = push_caret;
	fore = push_fore;
}

void form_frame() {
	rectf(form_button);
}

void rectb_black() {
	pushfore push(colors::black);
	rectb();
}

void form_frame_rect() {
	form_frame(form_button_light, form_button_dark);
	setoffset(1, 1);
}

void form_frame(int thickness) {
	pushrect push;
	while(thickness > 0) {
		form_frame_rect();
		thickness--;
	}
	form_frame();
}

void form_press_effect() {
	auto size = sizeof(color) * (width - 1);
	for(auto y = caret.y + height - 2; y >= caret.y; y--) {
		memmove(canvas->ptr(caret.x + 1, y + 1), canvas->ptr(caret.x, y), size);
		*((color*)canvas->ptr(caret.x, y)) = color();
		*((color*)canvas->ptr(caret.x, y + 1)) = color();
	}
	memset(canvas->ptr(caret.x, caret.y), 0, width * sizeof(color));
}

void form_press_button_effect() {
	pushrect push;
	width--; height--;
	form_press_effect();
	auto push_fore = fore;
	fore = colors::black;
	pixel(caret.x, caret.y + height);
	pixel(caret.x + width, caret.y);
	fore = push_fore;
}

void form_shadow_effect() {
	auto push_alpha = alpha;
	auto push_fore = fore;
	alpha = 96; fore = colors::black;
	setoffset(1, 1);
	rectf();
	fore = push_fore;
	alpha = push_alpha;
}

bool button(const char* title, unsigned key, unsigned flags, bool paint_rect_black, int button_height, fnevent press_effect) {
	pushrect push;
	draw::height = button_height;
	if(paint_rect_black) {
		rectb_black();
		setoffset(1, 1);
	}
	auto push_fore = fore;
	auto button_focus = (void*)(*((int*)&caret));
	auto run = button_input(button_focus, key, false);
	auto pressed = (pressed_focus == button_focus);
	form_frame(1);
	if(current_focus == button_focus)
		fore = colors::active;
	setoffset(1, 2);
	texta(title, flags);
	fore = push_fore;
	if(pressed) {
		setoffset(-1, -2);
		press_effect();
	}
	return run;
}

bool button(const char* title, unsigned key, unsigned flags) {
	return button(title, key, flags, false, height, form_press_button_effect);
}

void button(const char* title, unsigned key, unsigned flags, fnevent proc, long param) {
	if(button(title, key, flags, true, texth() + 5, form_press_button_effect))
		execute(proc, param, 0, 0);
	caret.y += texth() + 4;
}

bool button(unsigned key, fnevent pressed_effect) {
	if(disable_input)
		return false;
	auto button_data = (void*)(*((int*)&caret));
	auto ishilited = ishilite();
	auto isfocused = (current_focus == button_data);
	auto run = button_input(button_data, key, false);
	if(pressed_focus == button_data)
		pressed_effect();
	return run;
}

void paint_background(color v) {
	caret.x = (getwidth() - 320) / 2;
	caret.y = (getheight() - 200) / 2;
	width = 320;
	height = 200;
	rectf(v);
}

point same_point(point v, int resolution) {
	return {v.x / resolution, v.y / resolution};
}

void update_buttonparam() {
	updatewindow();
	buttonparam();
}

BSDATA(themei) = {
	{"ButtonMenu", color(178, 178, 199), color(255, 255, 255), color(85, 85, 121), color(68, 68, 96)},
	{"ButtonLight", color(186, 190, 150), color(251, 255, 203), color(101, 101, 77), color(69, 69, 52)},
	{"ButtonDark", color(40, 40, 60), color(65, 65, 89), color(40, 40, 60), color(255, 215, 138)},
	{"ButtonYellow", color(203, 142, 16), color(255, 182, 44), color(154, 105, 0), color(60, 36, 0)},
	{"TextRed", color(215, 0, 0), color(89, 0, 0), color(65, 0, 0), color(20, 20, 40)},
	{"TextYellow", color(255, 215, 138), color(195, 154, 65), color(113, 89, 8), color(20, 20, 40)},
};
assert_enum(themei, TextYellow)