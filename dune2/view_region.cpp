#include "draw.h"
#include "game.h"
#include "midi.h"
#include "object.h"
#include "player.h"
#include "pushvalue.h"
#include "screenshoot.h"
#include "slice.h"
#include "timer.h"
#include "view_map.h"
#include "view_theme.h"

using namespace draw;

struct attackregioni {
	unsigned char	index;
	unsigned char	frame;
	point			position;
	constexpr operator bool() const { return index != 0; }
};
static attackregioni attack_regions[3];

static const char* form_header;

static point piece_points[] = {
	{0, 0}, {55, 0}, {97, 0}, {145, 0}, {211, 0},
	{247, 0}, {0, 9}, {71, 38}, {133, 31}, {173, 16},
	{220, 22}, {264, 16}, {0, 41}, {21, 46}, {87, 70},
	{148, 45}, {186, 45}, {243, 47}, {280, 47}, {0, 82},
	{23, 91}, {59, 86}, {133, 92}, {172, 90}, {195, 70},
	{216, 98}, {249, 77}
};
static short unsigned piece_world[][2] = {
	{},
};

static void paint_background() {
	music_update();
	paint_background(colors::black);
	image(gres(DUNEMAP), 0, 0);
	switch(last_fraction) {
	case Ordos:
		copybits(caret.x + 8, caret.y + 24, 56, 40, caret.x + 0, caret.y + 152);
		copybits(caret.x + 8, caret.y + 24, 56, 40, caret.x + 264, caret.y + 152);
		break;
	case Harkonens:
		copybits(caret.x + 0, caret.y + 152, 56, 40, caret.x + 264, caret.y + 152);
		break;
	case Atreides:
		copybits(caret.x + 264, caret.y + 152, 56, 40, caret.x + 0, caret.y + 152);
		break;
	}
}

static void paint_filled_regions() {
	auto ps = gres(PIECES);
	for(unsigned i = 0; i < lengthof(game.controls); i++) {
		if(!game.controls[i])
			continue;
		update_pallette_by_player(getdefaultcolor(game.controls[i]));
		image(caret.x + piece_points[i].x + 8, caret.y + piece_points[i].y + 24, ps, i + 1, 0);
	}
	update_pallette_by_player();
}

static void paint_header() {
	if(!form_header)
		return;
	pushrect push_rect;
	pushfont push(gres(FONT8));
	pushfore push_fore(colors::black);
	caret.x += 64; caret.y += 167; width = 192; height = 10;
	texta(form_header, AlignCenter);
}

static void paint_arrows() {
	update_game_time();
	auto tm = (getcputime() / 400) % 4;
	auto ps = gres(ARROWS);
	auto j = 128 + mainplayer().color_index * 16;
	for(unsigned i = 0; i < 4; i++)
		pallette[251 + (i + 4 - tm) % 4] = pallette[j + i];
	for(auto& e : attack_regions) {
		if(e)
			image(caret.x + e.position.x, caret.y + e.position.y, ps, e.frame, 0);
	}
}

static void paint_regions() {
	paint_background();
	image(caret.x + 8, caret.y + 24, gres(DUNEMAP), 2, 0);
	paint_filled_regions();
	paint_header();
	paint_arrows();
}

static void paint_planet_standart() {
	paint_background();
	image(caret.x + 8, caret.y + 24, gres(DUNEMAP), 3, 0);
	paint_header();
}

void clear_attack_regions() {
	memset(attack_regions, 0, sizeof(attack_regions));
}

static attackregioni* new_attack_region() {
	for(auto& e : attack_regions) {
		if(!e)
			return &e;
	}
	return attack_regions;
}

void add_attack_region(unsigned char index, unsigned char frame, short x, short y) {
	auto p = new_attack_region();
	p->index = index;
	p->frame = frame;
	p->position.x = x;
	p->position.y = y;
}

void choose_regions() {
	pushvalue push_music(current_music, MusicConquest);
	pushvalue push_fraction(last_fraction, mainplayer().fraction);
	show_scene_raw(paint_regions, 0);
}

void claim_regions(objectn fraction, unsigned index, const char* format) {
	pushvalue push_music(current_music, MusicConquest);
	pushvalue push_fraction(last_fraction, mainplayer().fraction);
	paint_regions();
	if(format)
		form_header = format;
	if(index)
		game.controls[index - 1] = fraction;
	appear_scene(paint_regions, 1000);
}

void press_planet(const char* format) {
	pushvalue push_music(current_music, MusicConquest);
	pushvalue push_fraction(last_fraction, mainplayer().fraction);
	if(format)
		form_header = format;
	appear_scene(paint_planet_standart, 1000);
}