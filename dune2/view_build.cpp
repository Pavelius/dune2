#include "bsdata.h"
#include "building.h"
#include "draw.h"
#include "resid.h"
#include "objecta.h"
#include "shape.h"
#include "view.h"
#include "view_list.h"
#include "view_theme.h"

using namespace draw;

static int build_current;

static void paint_background() {
	const int right_panel = 212;
	image(0, 0, gres(CHOAM), 0, ImagePallette);
	auto herb = bsdata<playeri>::elements[player_index].fraction - 1;
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
}

static void paint_field(const char* id, int value) {
	texta(str("%1:", getnm(id)), AlignLeft);
	texta(str("%1i", value), AlignRight);
}

static void paint_name_and_info(objectn subject) {
	pushrect push;
	caret.x = width - 190; caret.y += 24; width = 180;
	text(getnmo(subject));
	caret.x += 120; width -= 120;
	paint_field("Cost", getcreditscost(subject));
}

static void paint_building_subject(objectn subject) {
	auto ps = gres(getavatar(subject));
	if(ps && ps->count)
		image(width - 192, caret.y + 48, ps, get_frame(400) % ps->count, 0);
	if(getparent(subject)==Building)
		paint_build_shape(width - 32, caret.y + 136, subject);
	paint_name_and_info(subject);
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

static void paint_buttons(objectn subject) {
	auto push_font = font;
	pushrect push;
	font = gres(FONT8);
	caret.x = width - 160;
	caret.y += 168;
	width = 119;
	height = (getheight() - caret.y) / 2;
	if(button(getnm("BuildIt"), KeyEnter, AlignCenterCenter, true, height - 1, form_press_button_effect))
		execute(buttonok);
	paint_shadow_border();
	caret.y += height;
	//if(caret.y + height != getheight())
	height = getheight() - caret.y;
	if(button(getnm("ResumeGame"), KeyEscape, AlignCenterCenter, true, height - 1, form_press_button_effect))
		execute(buttoncancel);
	paint_shadow_border();
}

static void paint_subject(int index, void* data) {
	auto type = *((objectn*)data);
	image(gres(SHAPES), geticonavatar(type), 0);
}

static void paint_elements() {
	static int origin;
	pushrect push;
	caret.x += 68; width -= 212 + caret.x;
	caret.y += 20; height -= 18 + caret.y;
	// rectb();
	auto hilite = get_flash(pallette[145], colors::white, 512, 256);
	paint_list(origin, build_current, subjects.count, subjects.data, sizeof(subjects.data[0]),
		hilite, {40, 32}, {4, 4}, paint_subject);
}

static void paint_main() {
	pushtheme push_theme(ButtonYellow);
	paint_background();
	paint_spice();
	paint_elements();
	auto subject = subjects.data[build_current];
	paint_building_subject(subject);
	paint_buttons(subject);
}

static bool choose_build() {
	return show_scene(paint_main, 0, 0) != 0;
}

void open_building() {
	auto push = subjects;
	last_building = (building*)hot.param;
	last_building->buildlist();
	subjects.sort();
	build_current = subjects.find(last_building->build);
	if(build_current == -1)
		build_current = 0;
	if(choose_build()) {
		last_building->build = subjects[build_current];
		last_building->progress();
	}
	subjects = push;
}