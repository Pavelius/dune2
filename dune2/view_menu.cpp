#include "answers.h"
#include "draw.h"
#include "math.h"
#include "resid.h"
#include "screenshoot.h"
#include "view_focus.h"
#include "view_menu.h"
#include "view_theme.h"

using namespace draw;

static bool menu_button(int index, const void* data, const char* text, unsigned key) {
	if(!data)
		data = text;
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

static void rectf_shadow() {
	pushfore push(colors::black);
	auto push_alpha = alpha; alpha = 128;
	rectf();
	alpha = push_alpha;
}

long show_menu(const char* header, int menu_width, const char* cancel, const char* additional, fnevent additional_proc) {
	const int additional_offset = 6;
	const int padding = 2;
	pushtheme push_theme(ButtonMenu);
	pushfont push_font(gres(FONT8));
	screenshoot push_screen;
	point window_size(getwidth(), getheight());
	auto menu_height = texth() + 4;
	auto header_height = 0;
	if(!additional_proc)
		additional_proc = buttoncancel;
	while(ismodal()) {
		push_screen.restore();
		rectf_shadow();
		draw::width = menu_width + additional_offset * 2;
		draw::height = (an.getcount() + 1) * (menu_height + padding) + additional_offset * 2 - padding;
		if(header) {
			header_height = texth(header, width) + padding * 2;
			draw::height += header_height;
		}
		caret.x = (getwidth() - draw::width) / 2;
		caret.y = imax(24, (getheight() - draw::height) / 2);
		form_frame(2);
		setoffset(12, 6);
		draw::height = menu_height;
		auto index = 0;
		if(header) {
			auto push_height = height;
			pushfore push(form_button_dark);
			height = header_height;
			texta(header, AlignCenterCenter);
			caret.y += header_height;
			height = push_height;
		}
		for(auto& e : an) {
			if(menu_button(index, e.value, e.text, e.key))
				execute(update_buttonparam, (long)e.value, 0, 0);
			caret.y += menu_height + padding;
		}
		auto width_origin = draw::width;
		draw::width = (width_origin - padding) / 2;
		if(menu_button(-1, 0, additional, 0))
			execute(additional_proc);
		caret.x = caret.x + width_origin - draw::width;
		if(menu_button(-1, 0, cancel, KeyEscape))
			execute(buttoncancel);
		domodal();
		if(window_size.x != getwidth() || window_size.y != getheight())
			breakmodal(0);
	}
	push_screen.restore();
	return getresult();
}

bool confirm(const char* header, const char* yes, const char* no) {
	pushanswer push;
	return show_menu(header, 200, no, yes, buttonok);
}

void execute_menu(const char* header, int menu_width, const char* cancel, const char* additional, fnevent additional_proc, const menui* elements) {
	pushanswer push;
	auto index = 0;
	for(auto p = elements; *p; p++)
		an.addv(p->proc, getnm(p->id), 0, '1' + index);
	auto p = (fnevent)show_menu(header, menu_width, cancel, additional, additional_proc);
	if(p)
		p();
}