#include "draw.h"
#include "game.h"
#include "midi.h"
#include "object.h"
#include "player.h"
#include "pushvalue.h"
#include "scenario.h"
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
	auto tm = (current_tick / 400) % 4;
	auto ps = gres(ARROWS);
	auto j = 128 + mainplayer().color_index * 16;
	for(unsigned i = 0; i < 4; i++)
		pallette[251 + (i + 4 - tm) % 4] = pallette[j + i];
	for(auto& e : attack_regions) {
		if(e)
			image(caret.x + e.position.x, caret.y + e.position.y, ps, e.frame, 0);
	}
}

static bool is_attack_region(unsigned char n) {
	for(auto v : attack_regions) {
		if(v.index == n)
			return true;
	}
	return false;
}

static void paint_region_choose() {
	pushrect push;
	auto ps = gres(DUNEMAP);
	caret.x += 8; caret.y += 24;
	auto& e = ps->frames[4];
	width = e.sx; height = e.sy;
	if(!hot.pressed && hot.key == MouseLeft && hot.mouse.in(getrect())) {
		auto n = *((unsigned char*)ps->ptr(e.offset) + (hot.mouse.y - caret.y) * e.sx + (hot.mouse.x - caret.x));
		if(is_attack_region(n))
			breakmodal(n);
	}
}

static void paint_regions() {
	paint_background();
	image(caret.x + 8, caret.y + 24, gres(DUNEMAP), 2, 0);
	paint_filled_regions();
	paint_header();
}

static void paint_regions_choose() {
	paint_background();
	image(caret.x + 8, caret.y + 24, gres(DUNEMAP), 2, 0);
	paint_filled_regions();
	paint_header();
	paint_arrows();
	paint_region_choose();
}

static void paint_planet_standart() {
	paint_background();
	image(caret.x + 8, caret.y + 24, gres(DUNEMAP), 3, 0);
	paint_header();
}

static void paint_dune_map() {
	paint_background();
	image(caret.x + 8, caret.y + 24, gres(DUNEMAP), 1, 0);
	paint_header();
}

static void paint_dune_map_area() {
	paint_background();
	image(caret.x + 8, caret.y + 24, gres(DUNEMAP), 2, 0);
	paint_header();
}

static void clear_attack_regions() {
	memset(attack_regions, 0, sizeof(attack_regions));
}

static attackregioni* new_attack_region() {
	for(auto& e : attack_regions) {
		if(!e)
			return &e;
	}
	return attack_regions;
}

static void add_attack_region(unsigned char index, unsigned char frame, short x, short y) {
	auto p = new_attack_region();
	p->index = index;
	p->frame = frame;
	p->position.x = x;
	p->position.y = y;
}

static unsigned char choose_regions() {
	pushvalue push_music(current_music, MusicConquest);
	pushvalue push_fraction(last_fraction, mainplayer().fraction);
	return (unsigned char)show_scene_raw(paint_regions_choose, 0);
}

static void appear_region(fnevent proc, unsigned delay = 500) {
	pushvalue push_music(current_music, MusicConquest);
	pushvalue push_fraction(last_fraction, mainplayer().fraction);
	appear_scene(proc, delay);
}

static void flash_region(objectn fraction, unsigned char index) {
	auto previous = game.controls[index - 1];
	game.controls[index - 1] = fraction;
	appear_region(paint_regions, 250);
	game.controls[index - 1] = previous;
	appear_region(paint_regions, 250);
	game.controls[index - 1] = fraction;
	appear_region(paint_regions, 250);
	game.controls[index - 1] = previous;
	appear_region(paint_regions, 250);
	game.controls[index - 1] = fraction;
	appear_region(paint_regions, 250);
}

void show_scenario_moves(objectn fraction, unsigned char scenario) {
	paint_background(colors::black);
	if(scenario == 1) {
		form_header = getnm(ThreeHouseArrive);
		appear_region(paint_planet_standart, 500);
		appear_region(paint_planet_standart, 1500);
		form_header = getnm(ToTakeControlOfLand);
		appear_region(paint_dune_map);
		appear_region(paint_dune_map, 1000);
		form_header = getnm(ThatHasBecameDivided);
		appear_region(paint_dune_map_area);
		appear_region(paint_dune_map_area, 1500);
	}
	// Scenario movements
	auto& ei = scenarios[(fraction - Atreides) * 9 + (scenario-1)];
	for(auto& e : ei.moves) {
		form_header = regions_conquere[e.message];
		for(auto i : e.regions) {
			if(!i)
				continue;
			game.controls[i - 1] = e.fraction;
			appear_region(paint_regions);
		}
	}
	// Main map
	form_header = getnm(SelectNextTarget);
	clear_attack_regions();
	for(auto& e : ei.order)
		add_attack_region(e.region, e.frame, e.x, e.y);
	auto n = choose_regions();
	if(n)
		flash_region(fraction, n);
	clear_attack_regions();
}