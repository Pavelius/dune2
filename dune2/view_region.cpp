#include "draw.h"
#include "game.h"
#include "midi.h"
#include "object.h"
#include "player.h"
#include "pushvalue.h"
#include "view_map.h"
#include "view_theme.h"

using namespace draw;

static void paint_background() {
	music_update();
	paint_background(colors::black);
	image(gres(DUNEMAP), 0, 0);
	switch(last_fraction) {
	case Atreides:
		copybits(caret.x + 8, caret.y + 24, 56, 40, caret.x + 0, caret.y + 152);
		copybits(caret.x + 8, caret.y + 24, 56, 40, caret.x + 264, caret.y + 152);
		break;
	}
}

static void paint_regions() {
	paint_background();
}

void choose_regions() {
	pushvalue push_fraction(last_fraction, mainplayer().fraction);
	show_scene_raw(paint_regions, 0);
}