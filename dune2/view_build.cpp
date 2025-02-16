#include "bsdata.h"
#include "building.h"
#include "draw.h"
#include "resid.h"
#include "shape.h"
#include "topicable.h"
#include "view.h"
#include "view_theme.h"

using namespace draw;

static void paint_background() {
	const int right_panel = 212;
	image(0, 0, gres(CHOAM), 0, ImagePallette);
	auto herb = player->fraction - 1;
	if(herb >= 0 && herb <= 2) {
		static point heraldic[3] = {{0, 152}, {0, 8}, {128, 48}};
		auto pt = heraldic[herb];
		for(auto i = 0; i < 2; i++)
			copybits(pt.x, pt.y, 56, 40, heraldic[i].x, heraldic[i].y);
	}
	copybits(62, 148, 54, 18, 62, 182);
	fillbitsv(62, 24, 54, 124, 176 - 18);
	if(width > 320) {
		copybits(320 - right_panel, 0, right_panel, 200, width - right_panel, 0);
		fillbitsh(68, 0, 40, 200, width - right_panel - 68);
	}
	if(height > 200) {
		copybits(0, 128, width - 199, 72, 0, height - 72);
		copybits(width - 199, 187, 199, 13, width - 199, height - 13);
		fillbitsv(0, 72, width - 199, 57, height - 142);
		fillbitsv(width - 199, 182, 199, 5, height - 13 - 182);
	}
	//width = width - 80;
	//height = height - caret.y;
}

static void paint_build_shape(int x, int y, shapen shape) {
	auto ps = gres(SHAPES);
	image(x, y, ps, 52, 0);
	auto& ei = bsdata<shapei>::elements[shape];
	x++; y++;
	for(auto i = 0; i < ei.count; i++)
		image(x + 6 * ei.points[i].x, y + 6 * ei.points[i].y, ps, 12, 0);
}

static void paint_building_subject(const buildingi* subject) {
	auto ps = gres(subject->mentat_avatar);
	if(ps && ps->count)
		image(width - 192, caret.y + 48, ps, get_frame(400) % ps->count, 0);
	paint_build_shape(width - 32, caret.y + 136, subject->shape);
}

static void paint_shadow_border() {
	auto push_fore = fore;
	auto push_caret = caret;
	fore = form_button_light.mix(form_button_dark, 128);
	pixel(caret.x, caret.y + height - 1);
	pixel(caret.x + width, caret.y);
	caret.y += height - 1;
	fore = fore.mix(form_button_dark, 128);
	caret.x++; line(caret.x + width - 1, caret.y);
	line(caret.x, caret.y - height);
	fore = push_fore;
	caret = push_caret;
}

static void paint_buttons(const buildingi* subject) {
	auto push_font = font;
	pushrect push;
	pushtheme push_theme(ButtonYellow);
	font = gres(FONT8);
	caret.x = width - 160;
	caret.y += 168;
	width = 119;
	height = (getheight() - caret.y) / 2;
	if(button(getnm("BuildThis"), KeyEnter, AlignCenterCenter, true, height - 1, form_press_button_effect))
		execute(buttonok, (long)subject);
	paint_shadow_border();
	caret.y += height;
	//if(caret.y + height != getheight())
	height = getheight() - caret.y;
	if(button(getnm("ResumeGame"), KeyEscape, AlignCenterCenter, true, height - 1, form_press_button_effect))
		execute(buttoncancel);
	paint_shadow_border();
}

static void paint_main() {
	auto subject = bsdata<buildingi>::elements + Barracks;
	paint_background();
	paint_spice();
	paint_building_subject(subject);
	paint_buttons(subject);
}

bool choose_build() {
	return show_scene(paint_main, 0, 0) != 0;
}

void open_building() {
	last_building = (building*)hot.param;
	choose_build();
}