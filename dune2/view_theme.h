#pragma once

#include "color.h"
#include "nameable.h"
#include "point.h"

typedef void(*fnevent)();

enum themen : unsigned char {
	ButtonMenu, ButtonLight, ButtonDark, ButtonYellow, TextRed, TextYellow,
};
enum resid : unsigned short;
struct sprite;
struct themei : nameable {
	color form, light, dark, fore;
};
struct pushtheme {
	color form, light, dark, fore;
	pushtheme(themen id);
	~pushtheme();
};
struct pushfontb {
	const sprite* font;
	color fpal1, fpal5, fpal6;
	color* palt;
	pushfontb(themen id);
	~pushfontb();
};
extern color form_button, form_button_light, form_button_dark;
extern color font_pallette[16];

point same_point(point v, int resolution = 2);

bool button(const char* title, unsigned key, unsigned flags, bool paint_rect_black, int button_height, fnevent press_effect);
bool button(const char* title, unsigned key, unsigned flags);
void button(const char* title, unsigned key, unsigned flags, fnevent proc, long param);
bool button(unsigned key, fnevent pressed_effect);
void form_frame();
void form_frame_rect();
void form_frame(color light_left_up, color shadow_right_down);
void rectb_black();
void rectf(color form);
void update_buttonparam();