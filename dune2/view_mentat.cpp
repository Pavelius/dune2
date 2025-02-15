#include "bsdata.h"
#include "draw.h"
#include "fraction.h"
#include "player.h"
#include "pushvalue.h"
#include "resid.h"
#include "music.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

static unsigned long eye_clapping, eye_show_cursor;
static fnevent paint_mentat_proc;

static void paint_mentat_eyes() {
	auto rid = bsdata<fractioni>::elements[last_fraction].mentat_face;
	auto frame = 0;
	if(time_animate(eye_clapping, 1, 16))
		frame = 4;
	else if(time_animate(eye_show_cursor, 30, 40)) {
		if(hot.mouse.x > 200)
			frame = 2;
		else if(hot.mouse.x < 30)
			frame = 1;
		else if(hot.mouse.y > 130)
			frame = 3;
	}
	switch(rid) {
	case MENSHPA: image(caret.x + 40, caret.y + 80, gres(rid), frame, 0); break;
	case MENSHPH: image(caret.x + 32, caret.y + 88, gres(rid), frame, 0); break;
	case MENSHPO: image(caret.x + 16, caret.y + 80, gres(rid), frame, 0); break;
	case MENSHPM: image(caret.x + 64, caret.y + 80, gres(rid), frame, 0); break;
	}
}

static void paint_mentat_speaking_mouth() {
	static int speak_frames[] = {5, 6, 5, 6, 5, 6, 5, 6, 7, 6, 5, 6, 7, 8, 9};
	auto rid = bsdata<fractioni>::elements[last_fraction].mentat_face;
	auto frame = speak_frames[get_frame() % (sizeof(speak_frames) / sizeof(speak_frames[0]))];
	switch(rid) {
	case MENSHPA: image(caret.x + 40, caret.y + 96, gres(rid), frame, 0); break;
	case MENSHPH: image(caret.x + 32, caret.y + 104, gres(rid), frame, 0); break;
	case MENSHPO: image(caret.x + 16, caret.y + 96, gres(rid), frame, 0); break;
	case MENSHPM: image(caret.x + 56, caret.y + 96, gres(rid), frame, 0); break;
	}
}

static void paint_mentat_content() {
	auto ps = gres(animate_id);
	if(ps && ps->count)
		image(caret.x + 128, caret.y + 48, ps, get_frame(400) % ps->count, 0);
}

static void paint_brief_row(int index, void* data) {
}

static void paint_mentat_list() {
	static int origin;
	static const char* test[50] = {0};
	width -= 12;
	paint_list_and_scroll(origin, sizeof(test) / sizeof(test[0]), test, sizeof(test[0]), 8, paint_brief_row);
}

static void paint_mentat_information() {
	pushrect push;
	caret.x += 128; caret.y += 48;
	width = 184; height = 112;
	paint_mentat_proc();
}

static void paint_mentat_back() {
	paint_mentat_information();
	auto rid = bsdata<fractioni>::elements[last_fraction].mentat_face;
	switch(rid) {
	case MENSHPA: image(caret.x + 128, caret.y + 128, gres(rid), 10, 0); break;
	case MENSHPH: image(caret.x + 128, caret.y + 104, gres(rid), 10, 0); break;
	case MENSHPO: image(caret.x + 128, caret.y + 128, gres(rid), 10, 0); break;
	}
}

static void paint_mentat_background() {
	paint_background(colors::black);
	image(gres(MENTATS), bsdata<fractioni>::elements[last_fraction].mentat_frame, 0);
}

static void paint_mentat_exit() {
	pushrect push;
	auto push_fore = fore; fore = colors::black;
	caret.x += 200; caret.y += 180; width = 48; height = 24;
	button(getnm("Exit"), 0, KeyEscape, AlignCenterCenter, false, buttoncancel, 0);
	fore = push_fore;
}

void paint_mentat() {
	paint_mentat_background();
	paint_mentat_eyes();
	paint_mentat_speaking_mouth();
	paint_mentat_back();
	paint_mentat_exit();
}

void paint_mentat_silent() {
	paint_mentat_background();
	paint_mentat_eyes();
	paint_mentat_back();
	paint_mentat_exit();
}

void open_mentat() {
	pushvalue push_fraction(last_fraction, player->fraction);
	pushvalue push_proc(paint_mentat_proc, paint_mentat_list);
	//song_play(str("mentat%1", player->getfractionsuffix()));
	show_scene(paint_mentat_silent, 0, 0);
	music_play(0);
}