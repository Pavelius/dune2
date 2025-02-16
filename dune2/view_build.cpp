#include "building.h"
#include "draw.h"
#include "resid.h"
#include "view.h"

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
	if(width > 320) {
		copybits(320 - right_panel, 0, right_panel, 200, width - right_panel, 0);
		fillbitsh(68, 0, 40, 200, width - right_panel - 68);
	}
	if(height > 200) {
		copybits(0, 128, width - 199, 72, 0, height - 72);
		fillbitsv(0, 72, width - 199, 57, height - 142);
	}
	width = width - 80;
	height = height - caret.y;
	//if(width > 320 || height > 200)
	//	paint_background(color(212, 148, 20));
	//image(caret.x, caret.y, gres(CHOAM), 0, 0);
}

static void paint_main() {
	paint_background();
	paint_spice();
}

bool choose_build() {
	return show_scene(paint_main, 0, 0) != 0;
}

void open_building() {
	last_building = (building*)hot.param;
	choose_build();
}