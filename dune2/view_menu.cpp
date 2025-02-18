#include "answers.h"
#include "draw.h"
#include "math.h"
#include "resid.h"
#include "screenshoot.h"
#include "view.h"
#include "view_focus.h"
#include "view_theme.h"

using namespace draw;

static bool menu_button(int index, const void* data, const char* text, unsigned key) {
	if(true) {
		pushrect push;
		form_frame(2);
		caret.y += 1;
		texta(text, AlignCenterCenter);
	}
	auto run = button_input(data, key, false);
	if(pressed_focus==data)
		form_press_effect();
	return run;
}

long show_menu(const char* header, point size, const char* cancel, const char* additional, fnevent proc) {
	const int additional_offset = 6;
	const int padding = 2;
	auto push_font = font; font = gres(FONT8);
	pushtheme push_theme(ButtonMenu);
	screenshoot push_screen;
	point window_size(getwidth(), getheight());
	while(ismodal()) {
		push_screen.restore();
		width = size.x + additional_offset * 2;
		height = an.getcount() * (size.y + padding) + additional_offset * 2 - padding;
		if(header)
			height += texth() + padding * 2;
		caret.x = (getwidth() - width) / 2;
		caret.y = imax(24, (getheight() - height) / 2);
		form_frame(2);
		setoffset(12, 6);
		height = size.y;
		auto index = 0;
		if(header) {
			auto push_fore = fore; fore = color(255, 215, 138);
			texta(header, AlignCenterCenter);
			fore = push_fore;
			caret.y += texth() + padding * 2;
		}
		for(auto& e : an) {
			if(menu_button(index, e.value, e.text, e.key))
				execute(update_buttonparam, (long)e.value, 0, 0);
			caret.y += size.y + padding;
		}
		domodal();
		if(window_size.x != getwidth() || window_size.y != getheight())
			breakmodal(0);
	}
	font = push_font;
	return getresult();
}