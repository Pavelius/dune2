#include "area.h"
#include "bsdata.h"
#include "direction.h"
#include "draw.h"
#include "drawable.h"
#include "fraction.h"
#include "game.h"
#include "io_stream.h"
#include "pushvalue.h"
#include "rand.h"
#include "resid.h"
#include "timer.h"
#include "unit.h"
#include "view.h"
#include "view_focus.h"

using namespace draw;
static rect screen_map_area = {area_screen_x1, area_screen_y1, area_screen_x1 + area_screen_width * area_tile_width, area_screen_y1 + area_screen_height * area_tile_height};
static unsigned long form_opening_tick;
static unsigned long next_turn_tick;
static unsigned long eye_clapping, eye_show_cursor;
unsigned long current_tick;
resid mentat_subject;

static bool debug_toggle;

static void debug_map_message() {
	rectpush push;
	caret.x = clipping.x1 + 2; caret.y = clipping.y1 + 2;
	auto t = area.get(area_spot);
	textnc(str("area %1i,%2i %3", area_spot.x, area_spot.y, bsdata<terraini>::elements[t].getname()), -1, 0);
	if(area.isbuilding(area_spot)) {
		caret.x += 6;
		textnc("Building", -1, 0);
	}
}

const unsigned time_step = 100;

static void update_tick() {
	current_tick = getcputime();
}

static void update_next_turn() {
	if(!next_turn_tick || (next_turn_tick < current_tick && (current_tick - next_turn_tick)>=10000))
		next_turn_tick = current_tick;
	while(next_turn_tick < current_tick) {
		next_turn_tick += 1000;
		update_game_turn();
	}
}

int get_frame(unsigned long resolution) {
	if(!resolution)
		resolution = 200;
	return (current_tick - form_opening_tick) / resolution;
}

struct pushscene : pushfocus {
	const sprite* font;
	pushscene() : pushfocus(), font(draw::font) {
		form_opening_tick = getcputime();
		current_tick = form_opening_tick;
	}
	~pushscene() { draw::font = font; }
};

bool time_animate(unsigned long& value, unsigned long duration, unsigned long pause = 20) {
	if(value <= form_opening_tick)
		value = form_opening_tick + xrand(pause * time_step, pause * 2 * time_step);
	if(value > current_tick)
		return false;
	else if((current_tick - value) < duration * time_step)
		return true;
	else
		value = current_tick + xrand(pause * time_step, pause * 2 * time_step);
	return false;
}

bool mouse_hower(unsigned long duration = 1000, bool single_time = true) {
	static point pos;
	static unsigned long pos_time;
	point v;
	v.x = hot.mouse.x / 2;
	v.y = hot.mouse.y / 2;
	if(pos_time < form_opening_tick) {
		pos = {-1000, -1000};
		pos_time = form_opening_tick;
	}
	if(pos != v) {
		pos = v;
		pos_time = current_tick;
		return false;
	} else if(pos_time + duration > current_tick)
		return false;
	else {
		if(single_time)
			pos_time = 0xFFFFFFFF - duration * 2;
		else
			pos_time = current_tick;
		return true;
	}
}

void set_font(resid v) {
	font = gres(v);
}

static void make_screenshoot() {
	auto index = get_file_number("screenshoots", "scr*.bmp");
	char temp[260]; stringbuilder sb(temp);
	sb.add("screenshoots/scr%1.5i.bmp", index);
	write(temp,
		draw::canvas->ptr(0, 0), canvas->width, canvas->height, canvas->bpp, canvas->scanline, 0);
}

static void paint_sprites(resid id, point offset, int index, int& focus, int per_line) {
	auto p = gres(id);
	if(!p)
		return;
	auto push_line = caret;
	auto count = per_line;
	while(index < p->count) {
		image(p, index, 0);
		if(focus == index) {
			auto push_caret = caret;
			caret = caret - offset;
			rectb();
			caret = push_caret;
		}
		index++;
		caret.x += width;
		if((--count) == 0) {
			count = per_line;
			caret.y += height;
			caret.x = push_line.x;
		}
		if((caret.y + height) > getheight())
			break;
	}
}

static void show_sprites(resid id, point start, point size, color backgc) {
	rectpush push;
	auto push_fore = fore;
	auto push_font = font;
	set_font(FONT6);
	int focus = 0;
	auto maximum = gres(id)->count;
	auto per_line = 320 / size.x;
	auto maximum_height = (getheight() / size.y);
	auto origin = 0;
	while(ismodal()) {
		if(focus < 0)
			focus = 0;
		else if(focus > maximum - 1)
			focus = maximum - 1;
		if(focus < origin)
			origin = (focus / per_line) * per_line;
		else if(focus > origin + (maximum_height)*per_line)
			origin = ((focus - ((maximum_height - 1) * per_line)) / per_line) * per_line;
		fore = backgc;
		rectf();
		width = size.x;
		height = size.y;
		caret = start;
		fore = colors::white;
		paint_sprites(id, start, origin, focus, per_line);
		caret = {1, 193};
		text(str("index %1i", focus), -1, TextBold);
		domodal();
		switch(hot.key) {
		case KeyRight: focus++; break;
		case KeyLeft: focus--; break;
		case KeyDown: focus += per_line; break;
		case KeyUp: focus -= per_line; break;
		case KeyEscape: breakmodal(0); break;
		}
		focus_input();
	}
	font = push_font;
	fore = push_fore;
}

static void common_input() {
	update_tick();
	switch(hot.key) {
	case Ctrl + F5: make_screenshoot(); break;
	case Ctrl + 'S': show_sprites(SHAPES, {0, 0}, {32, 24}, color(64, 0, 128)); break;
	case Ctrl + 'I': show_sprites(ICONS, {0, 0}, {16, 16}, color(24, 0, 64)); break;
	case Ctrl + 'A': show_sprites(ARROWS, {0, 0}, {16, 16}, color(24, 0, 64)); break;
	case 'A': area.set(area_spot, d100() < 60 ? CarRemains : AircraftRemains); break;
	case 'B': area.set(area_spot, Blood); break;
	case 'D': debug_toggle = !debug_toggle; break;
	case 'E': area.set(area_spot, Explosion); break;
	case 'F': area.set(area_spot, d100() < 60 ? Body : Bodies); break;
	}
}

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
	case MENSHPA: image(40, 80, gres(rid), frame, 0); break;
	case MENSHPH: image(32, 88, gres(rid), frame, 0); break;
	case MENSHPO: image(16, 80, gres(rid), frame, 0); break;
	case MENSHPM: image(64, 80, gres(rid), frame, 0); break;
	}
}

static void paint_mentat_speaking_mouth() {
	static int speak_frames[] = {5, 6, 5, 6, 5, 6, 5, 6, 7, 6, 5, 6, 7, 8, 9};
	auto rid = bsdata<fractioni>::elements[last_fraction].mentat_face;
	auto frame = speak_frames[get_frame() % (sizeof(speak_frames) / sizeof(speak_frames[0]))];
	switch(rid) {
	case MENSHPA: image(40, 96, gres(rid), frame, 0); break;
	case MENSHPH: image(32, 104, gres(rid), frame, 0); break;
	case MENSHPO: image(16, 96, gres(rid), frame, 0); break;
	case MENSHPM: image(56, 96, gres(rid), frame, 0); break;
	}
}

static void paint_mentat_back() {
	auto ps = gres(mentat_subject);
	if(!ps || !ps->count)
		return;
	image(128, 48, ps, get_frame(400) % ps->count, 0);
	auto rid = bsdata<fractioni>::elements[last_fraction].mentat_face;
	switch(rid) {
	case MENSHPA: image(128, 128, gres(rid), 10, 0); break;
	case MENSHPH: image(128, 104, gres(rid), 10, 0); break;
	case MENSHPO: image(128, 128, gres(rid), 10, 0); break;
	}
}

static void paint_mentat_background() {
	image(0, 0, gres(MENTATS), bsdata<fractioni>::elements[last_fraction].mentat_frame, 0);
}

void paint_mentat() {
	paint_mentat_background();
	paint_mentat_eyes();
	paint_mentat_speaking_mouth();
	paint_mentat_back();
}

void paint_mentat_silent() {
	paint_mentat_background();
	paint_mentat_eyes();
	paint_mentat_back();
}

static void set_area_view() {
	auto v = (unsigned)hot.param;
	area.setcamera(v, hot.param2 > 0);
}

static rect get_corner_area(direction d) {
	const int ex = area_tile_width / 2;
	const int ey = area_tile_height / 2;
	const int x1 = area_screen_x1;
	const int y1 = area_screen_y1;
	const int x2 = area_screen_x1 + area_screen_width * area_tile_width;
	const int y2 = area_screen_y1 + area_screen_height * area_tile_height;
	switch(d) {
	case LeftUp: return {x1, y1, x1 + ex, y1 + ey - 1};
	case Left: return {x1, y1 + ey, x1 + ex, y2 - ey - 1};
	case LeftDown:return {x1, y2 - ey - 1, x1 + ex, y2};
	case Up: return {x1 + ex, y1, x2 - ex - 1, y1 + ey - 1};
	case RightUp: return {x2 - ex, y1, x2, y1 + ey - 1};
	case Right: return {x2 - ex, y1 + ey, x2, y2 - ey - 1};
	case RightDowm: return {x2 - ex, y2 - ey, x2, y2};
	case Down: return {x1 + ex, y2 - ey - 1, x2 - ex - 1, y2};
	case Center: return {x1 + ex, y1 + ey, x2 - ex - 1, y2 - ey - 1};
	default: return {};
	}
}

static int get_arrows_frame(direction d) {
	switch(d) {
	case LeftUp: return 0;
	case Up: return 1;
	case RightUp: return 2;
	case Right: return 3;
	case RightDowm: return 4;
	case Down: return 5;
	case LeftDown: return 6;
	case Left: return 7;
	default: return -1;
	}
}

static void show_mouse_camera_slider(int x, int y, int frame) {
	auto ps = gres(ARROWS);
	auto& fr = ps->get(frame);
	x -= fr.sx / 2;
	y -= fr.sy / 2;
	if(x < clipping.x1)
		x = clipping.x1;
	if(y < clipping.y1)
		y = clipping.y1;
	if(x > clipping.x2 - fr.sx)
		x = clipping.x2 - fr.sx;
	if(y > clipping.y2 - fr.sy)
		y = clipping.y2 - fr.sy;
	image(x, y, ps, frame, ImageNoOffset);
}

static void check_mouse_corner_slice() {
	static direction all[] = {LeftUp, Up, RightUp, Right, RightDowm, Down, LeftDown, Left};
	if(hot.mouse.in(get_corner_area(Center)))
		return;
	for(auto d : all) {
		auto rc = get_corner_area(d);
		if(hot.mouse.in(rc)) {
			show_mouse_camera_slider(rc.centerx(), rc.centery(), get_arrows_frame(d));
			if(mouse_hower(100, false))
				execute(set_area_view, (long)(area_origin + getpoint(d)));
		}
	}
}

static point map_to_screen(point v) {
	if(!area.isvalid(v))
		return {-1, -1};
	return {(short)(v.x * area_tile_width + area_screen_x1), (short)(v.y * area_tile_height + area_screen_y1)};
}

static void paint_area_box() {
	if(!area.isvalid(area_spot))
		return;
	rectpush push;
	caret = map_to_screen(area_spot - area_origin);
	width = area_tile_width;
	height = area_tile_height;
	rectb();
}

static void handle_main_map_mouse_input() {
	switch(hot.key) {
	case 'C':
		execute(set_area_view, (long)area_spot, 1);
		break;
	}
}

static void paint_main_map_debug() {
	if(!debug_toggle)
		return;
	paint_area_box();
	debug_map_message();
}

static void paint_map_tiles() {
	auto ps = gres(ICONS);
	for(auto y = 0; y < area_screen_height; y++) {
		for(auto x = 0; x < area_screen_width; x++) {
			auto v = area_origin; v.x += x; v.y += y;
			auto i = area.getframe(v);
			if((current_tick / 300) % 2) {
				if(map_alternate[i])
					i = map_alternate[i];
			}
			image(x * area_tile_width + area_screen_x1, y * area_tile_height + area_screen_y1, ps, i, 0);
		}
	}
}

static void paint_map_features() {
	auto ps = gres(ICONS);
	for(auto y = 0; y < area_screen_height; y++) {
		for(auto x = 0; x < area_screen_width; x++) {
			auto v = area_origin; v.x += x; v.y += y;
			auto i = area.getframefeature(v);
			if(!i)
				continue;
			image(x * area_tile_width + area_screen_x1, y * area_tile_height + area_screen_y1, ps, i, 0);
		}
	}
}

static void paint_unit() {
}

static void paint_effect() {
}

void paint_main_map() {
	image(0, 0, gres(SCREEN), 0, 0);
	auto push_clip = clipping; setclip(screen_map_area);
	auto need_check_corner_slice = false;
	if(hot.mouse.in(screen_map_area)) {
		area_spot.x = (hot.mouse.x - area_screen_x1) / area_tile_width + area_origin.x;
		area_spot.y = (hot.mouse.y - area_screen_y1) / area_tile_height + area_origin.y;
		need_check_corner_slice = true;
		handle_main_map_mouse_input();
	} else
		area_spot = 0xFFFF;
	paint_map_tiles();
	paint_map_features();
	if(need_check_corner_slice)
		check_mouse_corner_slice();
	paint_main_map_debug();
	clipping = push_clip;
	update_next_turn();
}

void show_scene(fnevent before_paint, fnevent input, void* focus) {
	rectpush push;
	pushscene push_scene;
	current_focus = focus;
	while(ismodal()) {
		before_paint();
		domodal();
		if(input)
			input();
		common_input();
	}
}

void initialize_view(const char* title, fnevent main_scene) {
	draw::create(-1, -1, 320, 200, 0, 32);
	draw::setcaption(title);
	draw::settimer(100);
	set_font(FONT8);
	fore = colors::white;
	set_next_scene(main_scene);
	run_next_scene();
}

BSDATA(drawtypei) = {
	{"DrawUnit", paint_unit, 0, &bsdata<unit>::source, bsdata<unit>::elements},
	{"DrawEffect", paint_effect},
};
BSDATAF(drawtypei)