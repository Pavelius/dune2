#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "direction.h"
#include "draw.h"
#include "drawable.h"
#include "fix.h"
#include "fraction.h"
#include "game.h"
#include "io_stream.h"
#include "math.h"
#include "music.h"
#include "order.h"
#include "pushvalue.h"
#include "rand.h"
#include "resid.h"
#include "screenshoot.h"
#include "squad.h"
#include "timer.h"
#include "unit.h"
#include "unita.h"
#include "video.h"
#include "view.h"
#include "view_focus.h"
#include "view_indicator.h"
#include "view_list.h"
#include "view_theme.h"

using namespace draw;

static indicator spice;

color pallette[256], pallette_original[256];

const char* form_header;
static unsigned long form_opening_tick, next_game_time, time_move_order;
static point placement_size;
unsigned long animate_time, animate_delay = 200, animate_stop;
resid animate_id;
bool animate_once;
bool debug_toggle;

// External for debug tools. In release mode must be removed by linker.
void view_debug_input();

static void debug_control() {
	pushrect push;
	auto push_fore = fore; fore = pallette[144];
	auto push_alpha = alpha; alpha = 128;
	auto xm = (width + area_tile_width - 1) / area_tile_width;
	auto ym = (height + area_tile_height - 1) / area_tile_height;
	width = 14; height = 14;
	for(auto y = 0; y < ym; y++) {
		for(auto x = 0; x < xm; x++) {
			auto v = area_origin; v.x += x; v.y += y;
			if(path_map[v.y][v.x] == 0 || path_map[v.y][v.x] >= 0xFF00)
				continue;
			caret.x = x * area_tile_width + push.caret.x + 1;
			caret.y = y * area_tile_height + push.caret.y + 1;
			rectf();
		}
	}
	alpha = push_alpha;
	fore = push_fore;
}

static void debug_map_message() {
	pushrect push;
	caret.x = clipping.x1 + 2; caret.y = clipping.y1 + 2;
	auto t = area.get(area_spot);
	string sb;
	sb.add("Area %1i,%2i %3", area_spot.x, area_spot.y, bsdata<terraini>::elements[t].getname());
	auto pb = find_building(area.getcorner(area_spot));
	if(pb)
		sb.adds(pb->getname());
	text(sb.text, -1, TextStroke); caret.y += texth();
	sb.clear(); sb.add("Mouse %1i, %2i", hot.mouse.x, hot.mouse.y);
	text(sb.text, -1, TextStroke); caret.y += texth();
}

static void update_tick() {
	animate_time = getcputime();
}

static void update_next_turn() {
	if(!next_game_time)
		next_game_time = animate_time;
	if(next_game_time >= animate_time)
		return;
	auto delta = animate_time - next_game_time;
	next_game_time = animate_time;
	if(delta < 1000) { // Check if pause will be pressed
		game.time += delta;
		update_game_time();
	}
}

int get_frame(unsigned long resolution) {
	if(!resolution)
		resolution = 200;
	return (animate_time - form_opening_tick) / resolution;
}

static int get_alpha(int base, unsigned range, int tick) {
	auto seed = tick % range;
	auto half = range / 2;
	if(seed < half)
		return base * seed / half;
	else
		return base * (range - seed) / half;
}

static int get_alpha(int base, unsigned range) {
	return get_alpha(base, range, (animate_time - form_opening_tick) / 10);
}

color get_flash(color main, color back, int base, int range, int resolution) {
	auto alpha = get_alpha(320, 256, (animate_time - form_opening_tick) / resolution);
	if(alpha >= 256)
		return main;
	else
		return main.mix(back, alpha);
}

static int get_frame(sprite* ps, unsigned long resolution) {
	auto n = get_frame(resolution);
	if(!ps->count)
		return 0;
	return n % ps->count;
}

static point s2i(point v) {
	v = v - camera;
	v.x += area_screen.x1;
	v.y += area_screen.y1;
	return v;
}

static point i2s(point v) {
	v.x -= area_screen.x1;
	v.y -= area_screen.y1;
	v = v + camera;
	return v;
}

struct pushscene : pushfocus {
	pushscene() : pushfocus() {
		reset_form_animation();
	}
};

void hilite_unit_orders() {
	time_move_order = animate_time;
}

bool time_animate(unsigned long& value, unsigned long duration, unsigned long pause) {
	const unsigned time_step = 100;
	if(value <= form_opening_tick)
		value = form_opening_tick + xrand(pause * time_step, pause * 2 * time_step);
	if(value > animate_time)
		return false;
	else if((animate_time - value) < duration * time_step)
		return true;
	else
		value = animate_time + xrand(pause * time_step, pause * 2 * time_step);
	return false;
}

static bool mouse_hower(unsigned long duration = 1000, bool single_time = true) {
	static point pos;
	static unsigned long pos_time;
	if(pos_time < form_opening_tick) {
		pos = {-10000, -10000};
		pos_time = form_opening_tick;
	}
	if(same_point(pos, 4) != same_point(hot.mouse, 4)) {
		pos = hot.mouse;
		pos_time = animate_time;
		return false;
	} else if(pos_time + duration > animate_time)
		return false;
	else {
		if(single_time)
			pos_time = 0xFFFFFFFF - duration * 2;
		else
			pos_time = animate_time;
		return true;
	}
}

static void music_play(const char* id, int index) {
	song_play(str("%1%2.2i", id, index));
}

static void check_played_music() {
	if(music_disabled)
		return;
	if(!music_played())
		music_play("ambient", xrand(1, 10));
}

static void make_screenshoot() {
	auto index = get_file_number("screenshoots", "scr*.bmp");
	char temp[260]; stringbuilder sb(temp);
	sb.add("screenshoots/scr%1.5i.bmp", index);
	write(temp,
		draw::canvas->ptr(0, 0), canvas->width, canvas->height, canvas->bpp, canvas->scanline, 0);
}

static void common_input() {
	update_tick();
	check_played_music();
	switch(hot.key) {
	case Ctrl + F5: make_screenshoot(); break;
	}
#ifdef _DEBUG
	view_debug_input();
#endif
}

static void set_area_view() {
	auto v = (unsigned)hot.param;
	area.setcamera(v, hot.param2 > 0);
}

static rect get_corner_area(direction d) {
	const int ex = area_tile_width / 2;
	const int ey = area_tile_height / 2;
	const int x1 = area_screen.x1;
	const int y1 = area_screen.y1;
	const int x2 = area_screen.x2;
	const int y2 = area_screen.y2;
	switch(d) {
	case LeftUp: return {x1, y1, x1 + ex, y1 + ey - 1};
	case Left: return {x1, y1 + ey, x1 + ex, y2 - ey - 1};
	case LeftDown:return {x1, y2 - ey - 1, x1 + ex, y2};
	case Up: return {x1 + ex, y1, x2 - ex - 1, y1 + ey - 1};
	case RightUp: return {x2 - ex, y1, x2, y1 + ey - 1};
	case Right: return {x2 - ex, y1 + ey, x2, y2 - ey - 1};
	case RightDown: return {x2 - ex, y2 - ey, x2, y2};
	case Down: return {x1 + ex, y2 - ey - 1, x2 - ex - 1, y2};
	case Center: return {x1 + ex, y1 + ey, x2 - ex - 1, y2 - ey - 1};
	default: return {};
	}
}

static int get_arrows_frame(direction d) {
	switch(d) {
	case LeftUp: return 1;
	case Up: return 2;
	case RightUp: return 3;
	case Right: return 4;
	case RightDown: return 5;
	case Down: return 6;
	case LeftDown: return 7;
	case Left: return 8;
	default: return -1;
	}
}

static void show_mouse_camera_slider(direction d, int x, int y, int frame) {
	auto ps = gres(ARROWS);
	auto& fr = ps->get(frame);
	switch(d) {
	case Down: case Up: x = hot.mouse.x; break;
	case Left: case Right: y = hot.mouse.y; break;
	default: break;
	}
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
	static direction all[] = {LeftUp, Up, RightUp, Right, RightDown, Down, LeftDown, Left};
	if(hot.mouse.in(get_corner_area(Center)))
		return;
	for(auto d : all) {
		auto rc = get_corner_area(d);
		if(hot.mouse.in(rc)) {
			show_mouse_camera_slider(d, rc.centerx(), rc.centery(), get_arrows_frame(d));
			if(mouse_hower(100, false))
				execute(set_area_view, (long)(area_origin + getpoint(d)));
		}
	}
}

static point map_to_screen(point v) {
	return {(short)(v.x * area_tile_width + area_screen.x1), (short)(v.y * area_tile_height + area_screen.y1)};
}

static void rectb_hilite() {
	auto push = fore;
	fore = get_flash(colors::gray, colors::white, 512, 256);
	rectb();
	fore = push;
}

static void paint_cursor(int avatar, point offset, bool choose_mode) {
	auto v = area_spot - area_origin;
	if(!area.isvalid(v))
		return;
	auto pt = map_to_screen(v) + offset;
	image(pt.x, pt.y, gres(MOUSE), avatar, 0);
	if(choose_mode && hot.key == MouseLeft && hot.pressed)
		execute(buttonparam, (long)area_spot);
}

static void paint_cursor(point size, bool choose_mode) {
	auto v = area_spot - area_origin;
	if(!area.isvalid(v))
		return;
	pushrect push;
	width = 16 * size.x;
	height = 16 * size.y;
	caret = map_to_screen(v);
	rectb_hilite();
	if(choose_mode && hot.key == MouseLeft && hot.pressed)
		execute(buttonparam, (long)area_spot);
}

static void paint_main_map_debug() {
	if(!debug_toggle)
		return;
	debug_control();
	paint_cursor(5, {8, 8}, false);
	debug_map_message();
}

static void update_pallette_by_player(int index) {
	auto j = 128 + index * 16;
	for(auto i = 0; i < 6; i++)
		pallette[144 + i] = pallette_original[j + i];
}

static void update_pallette_by_player() {
	update_pallette_by_player(player->color_index);
}

static void update_pallette_colors() {
	// Wind trap
	pallette[223] = get_flash(color(80, 152, 232), colors::black, 640, 512, 20);
}

static void paint_map_tiles() {
	auto ps = gres(ICONS);
	auto xm = (width + area_tile_width - 1) / area_tile_width;
	auto ym = (height + area_tile_height - 1) / area_tile_height;
	for(auto y = 0; y < ym; y++) {
		for(auto x = 0; x < xm; x++) {
			auto v = area_origin; v.x += x; v.y += y;
			auto i = area.getframe(v);
			if((animate_time / 300) % 2) {
				if(map_alternate[i])
					i = map_alternate[i];
			}
			image(x * area_tile_width + caret.x, y * area_tile_height + caret.y, ps, i, ImagePallette);
		}
	}
}

static void paint_map_features() {
	auto ps = gres(ICONS);
	auto xm = (width + area_tile_width - 1) / area_tile_width;
	auto ym = (height + area_tile_height - 1) / area_tile_height;
	for(auto y = 0; y < ym; y++) {
		for(auto x = 0; x < xm; x++) {
			auto v = area_origin; v.x += x; v.y += y;
			auto i = area.getframefeature(v);
			if(!i)
				continue;
			image(x * area_tile_width + caret.x, y * area_tile_height + caret.y, ps, i, 0);
		}
	}
}

static void paint_platform(const sprite* ps, int frame, direction d) {
	switch(d) {
	case Up: image(ps, frame + 0, ImagePallette); break;
	case RightUp: image(ps, frame + 1, ImagePallette); break;
	case Right: image(ps, frame + 2, ImagePallette); break;
	case RightDown: image(ps, frame + 3, ImagePallette); break;
	case Down: image(ps, frame + 4, ImagePallette); break;
	case LeftDown: image(caret.x, caret.y, ps, frame + 3, ImageMirrorH | ImagePallette); break;
	case Left: image(caret.x, caret.y, ps, frame + 2, ImageMirrorH | ImagePallette); break;
	case LeftUp: image(caret.x, caret.y, ps, frame + 1, ImageMirrorH | ImagePallette); break;
	default: break;
	}
}

static void paint_unit(const uniti& e, direction move_direction, direction shoot_direction, unsigned char color_index) {
	update_pallette_by_player(color_index);
	paint_platform(gres(e.res), e.frame, move_direction);
	if(e.frame_shoot)
		paint_platform(gres(e.res), e.frame_shoot, shoot_direction);
}

static void paint_unit() {
	auto p = static_cast<unit*>(last_object);
	auto& e = p->geti();
	update_pallette_by_player(p->getplayer().color_index);
	paint_platform(gres(e.res), e.frame, p->move_direction);
	if(e.frame_shoot)
		paint_platform(gres(e.res), e.frame_shoot, p->shoot_direction);
}

static void paint_effect_fix() {
	auto p = (draweffect*)last_object;
	if(!p->param || p->start_time > animate_time)
		return;
	auto pf = bsdata<fixeffecti>::elements + p->param;
	auto delay = pf->milliseconds;
	if(!delay)
		delay = 60;
	auto frame = pf->frame;
	auto frame_offset = (short unsigned)((animate_time - p->start_time) / delay);
	if(frame_offset >= pf->count) {
		if(pf->apply)
			pf->apply();
		if(pf->next) {
			p->param = pf->next;
			p->start_time = animate_time;
			pf = bsdata<fixeffecti>::elements + pf->next;
			frame = pf->frame;
		} else {
			p->clearobject();
			return; // No render image
		}
	} else
		frame += frame_offset;
	image(gres(pf->rid), frame, 0);
}

static void paint_radar_rect() {
	pushrect push;
	caret.x = getwidth() - 64 + area_origin.x;
	caret.y = getheight() - 64 + area_origin.y;
	width = area_screen.width() / area_tile_width;
	height = area_screen.height() / area_tile_height;
	rectb();
}

static color get_color_by_index(int index) {
	return pallette_original[128 + index * 16 + 2];
}

static void paint_radar_units() {
	auto player_index = player->getindex();
	auto push_fore = fore; fore = get_color_by_index(player->color_index);
	for(auto& e : bsdata<unit>()) {
		if(!e || e.player!=player_index)
			continue;
		pixel(caret.x + e.position.x, caret.y + e.position.y);
	}
	fore = push_fore;
}

static void paint_radar_buildings() {
	auto player_index = player->getindex();
	auto push_fore = fore; fore = get_color_by_index(player->color_index);
	for(auto& e : bsdata<building>()) {
		if(!e || e.player != player_index)
			continue;
		auto size = e.getsize();
		auto pt = caret + e.position;
		for(auto y1 = 0; y1 < size.y; y1++) {
			for(auto x1 = 0; x1 < size.x; x1++)
				pixel(pt.x + x1, pt.y + y1);
		}
	}
	fore = push_fore;
}

static void input_radar() {
	point hot_mouse = hot.mouse - caret;
	if((hot_mouse.x < width && hot_mouse.x > 0) && (hot_mouse.y < height && hot_mouse.y > 0)) {
		switch(hot.key) {
		case MouseLeft:
			if(hot.pressed)
				execute(set_area_view, (long)(hot_mouse), 1);
			break;
		case MouseRight:
			if(hot.pressed)
				execute(mouse_unit_move, (long)hot_mouse);
			break;
		}
	}
}

static void paint_radar_land() {
	auto push_fore = fore;
	for(auto y = 0; y < area.maximum.y; y++) {
		for(auto x = 0; x < area.maximum.x; x++) {
			auto t = area.get(point(x, y));
			if(t > Mountain)
				continue;
			fore = bsdata<terraini>::elements[t].minimap;
			pixel(caret.x + x, caret.y + y);
		}
	}
	fore = push_fore;
}

static void paint_radar_off() {
	rectf(colors::black);
	// paint_radar_units();
	paint_radar_buildings();
	input_radar();
}

static void paint_radar_on() {
	paint_radar_land();
	paint_radar_units();
	paint_radar_buildings();
	input_radar();
}

static void paint_radar() {
	pushrect push;
	width = 64; height = 64;
	caret.x = getwidth() - width;
	caret.y = getheight() - height;
	if(player->buildings[RadarOutpost])
		paint_radar_on();
	else
		paint_radar_off();
}

void copybits(int x, int y, int width, int height, int x1, int y1) {
	if(x == x1 && y == y1)
		return;
	auto ps = canvas->ptr(x, y);
	auto pd = canvas->ptr(x1, y1);
	auto sn = canvas->scanline;
	auto sz = width * sizeof(color);
	if(y < y1) {
		ps = canvas->ptr(x, y + height - 1);
		pd = canvas->ptr(x1, y1 + height - 1);
		sn = -sn;
	}
	for(auto i = 0; i < height; i++) {
		memmove(pd, ps, sz);
		ps += sn;
		pd += sn;
	}
}

void fillbitsh(int x, int y, int width, int height, int total_width) {
	total_width -= width;
	if(total_width <= 0 || width <= 0)
		return;
	auto x1 = x + width;
	for(auto n = total_width / width; n > 0; n--) {
		copybits(x, y, width, height, x1, y);
		x1 += width;
		total_width -= width;
	}
	if(total_width)
		copybits(x, y, total_width, height, x1, y);
}

void fillbitsv(int x, int y, int width, int height, int total_height) {
	total_height -= height;
	if(total_height <= 0 || height <= 0)
		return;
	auto y1 = y + height;
	for(auto n = total_height / height; n > 0; n--) {
		copybits(x, y, width, height, x, y1);
		y1 += height;
		total_height -= height;
	}
	if(total_height)
		copybits(x, y, width, total_height, x, y1);
}

static void paint_background(resid rid) {
	caret.x = 0; caret.y = 0;
	if(rid == SCREEN) {
		const int right_panel = 120;
		image(0, 0, gres(rid), 0, ImagePallette);
		caret.x = 0; caret.y = 40;
		if(width > 320) {
			copybits(320 - right_panel, 0, right_panel, 200, width - right_panel, 0);
			fillbitsh(184, 0, 16, 16, width - right_panel - 184);
			fillbitsh(184, 16, 115, 24, width - right_panel - 184);
		}
		if(height > 200) {
			copybits(width - 80, 118, 80, 82, width - 80, height - 82);
			fillbitsv(width - 80, 92, 16, 25, height - 200 + 25);
		}
		width = width - 80;
		height = height - caret.y;
		area_screen.set(caret.x, caret.y, caret.x + width, caret.y + height);
	} else {
		if(width > 320 || height > 200)
			paint_background(colors::black);
		image(caret.x, caret.y, gres(rid), 0, 0);
	}
}

static void rectb_alpha() {
	auto push_alpha = alpha; alpha = 64;
	rectb();
	alpha = push_alpha;
}

static void selection_rect_dropped(const rect& rc) {
	human_selected.clear();
	if(!area.isvalid(area_spot))
		return;
	last_building = find_building(area.getcorner(area_spot));
	if(last_building)
		return;
	human_selected.select(player, rc);
}

static rect drag_finish_rect(point start, point finish, int minimal) {
	rect rc;
	rc.x1 = start.x;
	rc.y1 = start.y;
	rc.x2 = finish.x;
	rc.y2 = finish.y;
	rc.normalize();
	if(rc.width() < minimal && rc.height() < minimal) {
		rc.x1 = rc.centerx() - minimal / 2; rc.x2 = rc.x1 + minimal;
		rc.y1 = rc.centery() - minimal / 2; rc.y2 = rc.y1 + minimal;
	}
	return rc;
}

static void rectb_alpha_drag(point mouse_start) {
	pushrect push;
	auto start = s2i(mouse_start);
	caret = hot.mouse;
	width = start.x - caret.x;
	height = start.y - caret.y;
	rectb_alpha();
}

static void open_options() {
}

static void button(rect rc, unsigned key, fnevent pressed_effect, fnevent proc, long param) {
	pushrect push;
	caret.x = rc.x1; caret.y = rc.y1;
	width = rc.width(); height = rc.height();
	if(button(key, pressed_effect))
		execute(proc, param);
}

static void input_game_menu() {
	button({16, 1, 94, 15}, 'M', form_press_effect, open_mentat, 0);
	button({104, 1, 182, 15}, 'O', form_press_effect, open_options, 0);
}

static void input_game_map() {
	static point mouse_start;
	if(!hot.mouse.in(area_screen))
		return;
	switch(hot.key) {
	case MouseRight:
		if(hot.pressed)
			execute(mouse_unit_move, (long)area_spot);
		break;
	case 'C':
		execute(set_area_view, (long)area_spot, 1);
		break;
	}
	auto mouse_finish = i2s(hot.mouse);
	if(dragactive(input_game_map)) {
		if(!dragactive()) // Drop dragged object
			selection_rect_dropped(drag_finish_rect(mouse_start, mouse_finish, 8));
		else if(same_point(mouse_start, 3) != same_point(mouse_finish, 3))
			rectb_alpha_drag(mouse_start);
	} else if(dragbegin(input_game_map))
		mouse_start = mouse_finish;
}

static void rectb_last_building() {
	if(!last_building)
		return;
	pushrect push;
	caret = s2i(m2s(last_building->position));
	width = last_building->getsize().x * area_tile_width;
	height = last_building->getsize().y * area_tile_height;
	rectb_hilite();
}

static void paint_fow() {
	pushrect push;
	auto push_fore = fore; fore = colors::black;
	auto player_index = player->getindex();
	auto ps = gres(ICONS);
	auto xm = (width + area_tile_width - 1) / area_tile_width;
	auto ym = (height + area_tile_height - 1) / area_tile_height;
	width = area_tile_width; height = area_tile_height;
	for(auto y = 0; y < ym; y++) {
		for(auto x = 0; x < xm; x++) {
			auto v = area_origin; v.x += x; v.y += y;
			if(!area.is(v, player_index, Explored)) {
				caret.x = x * area_tile_width + push.caret.x;
				caret.y = y * area_tile_height + push.caret.y;
				rectf();
			} else {
				auto frame = area.getframefow(v, player_index, Explored);
				frame ^= 0x0f;
				if(frame==0 || frame==15)
					continue;
				image(x * area_tile_width + push.caret.x, y * area_tile_height + push.caret.y, ps, 108 + frame, 0);
			}
		}
	}
	fore = push_fore;
}

static void paint_visibility() {
	pushrect push;
	auto push_alpha = alpha; alpha = 32;
	auto push_fore = fore; fore = colors::black;
	auto player_index = player->getindex();
	auto ps = gres(ICONS);
	auto xm = (width + area_tile_width - 1) / area_tile_width;
	auto ym = (height + area_tile_height - 1) / area_tile_height;
	width = area_tile_width; height = area_tile_height;
	for(auto y = 0; y < ym; y++) {
		for(auto x = 0; x < xm; x++) {
			auto v = area_origin; v.x += x; v.y += y;
			if(!area.is(v, player_index, Explored))
				continue;
			if(!area.is(v, player_index, Visible)) {
				caret.x = x * area_tile_width + push.caret.x;
				caret.y = y * area_tile_height + push.caret.y;
				rectf();
			} else {
				auto frame = area.getframefow(v, player_index, Visible);
				frame ^= 0x0f;
				if(frame == 0 || frame == 15)
					continue;
				image(x * area_tile_width + push.caret.x, y * area_tile_height + push.caret.y, ps, 108 + frame, 0);
			}
		}
	}
	alpha = push_alpha;
	fore = push_fore;
}

static void paint_move_order() {
	if(!time_move_order)
		return;
	auto duration = (animate_time - time_move_order);
	if(duration > 1000)
		return;
	auto push_alpha = alpha;
	alpha = get_alpha(164, 1000, duration);
	auto ps = gres(MOUSE);
	for(auto p : human_selected) {
		if(!p->operator bool() || !area.isvalid(p->order))
			continue;
		auto v = s2i(m2sc(p->order));
		image(v.x, v.y, ps, 5, 0);
	}
	alpha = push_alpha;
}

static void paint_game_map() {
	auto push_clip = clipping; setclip(area_screen);
	camera = m2s(area_origin);
	if(hot.mouse.in(area_screen)) {
		area_spot.x = (hot.mouse.x - area_screen.x1) / area_tile_width + area_origin.x;
		area_spot.y = (hot.mouse.y - area_screen.y1) / area_tile_height + area_origin.y;
	} else
		area_spot = 0xFFFF;
	paint_map_tiles();
	paint_map_features();
	paint_objects();
	rectb_last_building();
	paint_visibility();
	paint_fow();
	paint_move_order();
	if(area.isvalid(area_spot))
		check_mouse_corner_slice();
	paint_main_map_debug();
	clipping = push_clip;
}

static void paint_map_info_background() {
	width = 64;
	caret.x = getwidth() - width;
	caret.y = 42;
	height = getheight() - caret.y - 75;
	form_frame(1);
	setoffset(2, 1);
	font = gres(FONT6);
	fore = color(69, 69, 52);
}

static void paint_bar(int value, int value_maximum) {
	form_frame_rect();
	if(!value_maximum)
		return;
	auto push_fore = fore;
	width = width * value / value_maximum;
	if(value <= value_maximum / 2)
		fore = colors::yellow;
	else if(value <= value_maximum / 5)
		fore = colors::red;
	else
		fore = colors::green;
	rectf();
	fore = push_fore;
}

static void paint_health_bar(int value, int value_maximum) {
	pushrect push;
	height = 8; width = 27;
	paint_bar(value, value_maximum);
	image(caret.x, caret.y + height + 1, gres(SHAPES), 15, 0);
}

static void paint_shoots(int frame, int value) {
	auto push_caret = caret;
	while(value-- > 0) {
		image(gres(UNITS1), frame, ImageNoOffset);
		caret.x += 3;
	}
	caret = push_caret;
	caret.y += 6;
}

static void paint_unit_panel(int frame, int hits, int hits_maximum, fnevent proc, long param) {
	auto push_caret = caret;
	auto push_width = width;
	image(gres(SHAPES), frame, 0);
	if(proc)
		button({caret.x, caret.y, caret.x + 32, caret.y + 24}, 0, form_shadow_effect, proc, param);
	caret.x += 32 + 1; width -= 32 + 1;
	paint_health_bar(hits, hits_maximum);
	caret.y += 12;
	// paint_shoots(25, 8); // 8 maximum. Can track
	texta("Dmg", AlignCenter | TextSingleLine);
	caret = push_caret;
	width = push_width;
	caret.y += 24;
}

static void paint_choose_panel(const char* id, int avatar, long cancel_result) {
	auto y2 = caret.y + height;
	texta(getnm(id), AlignCenter | TextSingleLine); caret.y += texth() - 1;
	image(gres(SHAPES), avatar, 0);
	caret.y += 32; height = y2 - caret.y - texth() - 4;
	// rectb_black();
	texta(getnm(ids(id, "Info")), AlignCenterCenter);
	caret.y += height;
	setoffset(-1, -1);
	button(getnm("Cancel"), KeyEscape, AlignCenterCenter, buttonparam, cancel_result);
}

static void paint_choose_terrain() {
	paint_choose_panel("ChooseTarget", 17, (long)point(-1000, -1000));
	paint_cursor(5, {8, 8}, true);
}

static void paint_choose_terrain_placement() {
	paint_choose_panel("ChoosePlacement", 17, (long)point(-1000, -1000));
	paint_cursor(placement_size, true);
}

static void human_order() {
	auto type = (ordern)hot.param;
	human_selected.order(type, Center, {-1, -1}, true);
}

static void button(ordern order, int key) {
	button(bsdata<orderi>::elements[order].getname(), key, AlignCenter, human_order, order);
}

void paint_spice() {
	auto push_caret = caret;
	caret.x = getwidth() - 62;
	caret.y = 3;
	spice.next = player->get(Credits);
	spice.paint();
	caret = push_caret;
}

static void paint_unit_orders() {
	pushrect push;
	setoffset(-1, 0);
	height = 12;
	button(Attack, 'A');
	button(Move, 'M');
	button(Retreat, 0);
	button(Stop, 'G');
}

static void paint_unit_list() {
	auto push_caret = caret;
	caret = push_caret + point(6, 8);
	for(auto p : human_selected) {
		paint_unit(p->geti(), RightDown, RightDown, p->getplayer().color_index);
		caret.x += 16;
		if(caret.x >= clipping.x2) {
			caret.x = push_caret.x;
			if(p != *human_selected.end() - 1)
				caret.y += 16;
		}
	}
	caret = push_caret;
}

static void paint_build_shape(int x, int y, shapen shape) {
	auto ps = gres(SHAPES);
	image(x, y, ps, 51, 0);
	auto& ei = bsdata<shapei>::elements[shape];
	x++; y++;
	for(auto i = 0; i < ei.count; i++)
		image(x + 6 * ei.points[i].x, y + 6 * ei.points[i].y, ps, 12, 0);
}

static point choose_placement() {
	return show_scene(paint_main_map_choose_placement, 0, 0);
}

static void human_build() {
	auto p = (building*)hot.object;
	if(!p->isworking())
		p->progress();
	else if(p->getprogress() == 100) {
		auto push = placement_size;
		placement_size = p->getbuildsize();
		p->construct(choose_placement());
		placement_size = push;
	}
}

static void human_cancel() {
	auto p = (building*)hot.object;
	p->cancel();
}

static void paint_build_button(const char* format, int avatar, shapen shape, unsigned key) {
	bool pressed;
	auto push_fore = fore;
	if(true) {
		pushrect push;
		rectb_black();
		setoffset(1, 1);
		auto button_focus = (void*)(*((int*)&caret));
		auto run = button_input(button_focus, key, false);
		pressed = (pressed_focus == button_focus);
		form_frame(1);
		setoffset(2, 2);
		image(gres(SHAPES), avatar, 0);
		paint_build_shape(caret.x + 35, caret.y + 2, shape);
		fore = form_button_light;
		caret.y += 24; texta(format, AlignCenter);
		if(run)
			execute((hot.key == MouseRight) ? human_cancel : human_build, 0, 0, last_building);
	}
	if(pressed)
		form_press_effect();
	fore = push_fore;
}

static void paint_build_button() {
	auto push_height = height; height = 36;
	setoffset(-1, 0);
	auto& ei = bsdata<buildingi>::elements[last_building->build];
	const char* format = 0;
	if(last_building->isworking())
		format = str("%1i%%", last_building->getprogress());
	else
		format = getnm("BuildIt");
	paint_build_button(format, ei.frame_avatar, ei.shape, 'B');
	height = push_height;
}

static void paint_building_info() {
	texta(last_building->getname(), AlignCenter | TextSingleLine); caret.y += texth() - 1;
	if(last_building->canbuild()) {
		paint_unit_panel(last_building->geti().frame_avatar, last_building->hits, last_building->geti().hits, open_building, (long)last_building);
		caret.y += 14;
		paint_build_button();
	} else {
		paint_unit_panel(last_building->geti().frame_avatar, last_building->hits, last_building->geti().hits, 0, 0);
		caret.y += 14;
	}
}

static void paint_unit_info() {
	pushrect push;
	if(!human_selected) {
		if(last_building)
			paint_building_info();
	} else if(human_selected.count == 1) {
		auto push_unit = last_unit;
		last_unit = human_selected[0];
		texta(last_unit->getname(), AlignCenter | TextSingleLine); caret.y += texth() - 1;
		paint_unit_panel(last_unit->geti().frame_avatar, last_unit->hits, last_unit->getmaximum(Hits), 0, 0);
		caret.y += 1;
		paint_unit_orders();
		last_unit = push_unit;
	} else {
		paint_unit_list();
		caret.y += 16 * 2;
		paint_unit_orders();
	}
}

static void paint_map_info(fnevent proc) {
	pushrect push;
	auto push_font = font;
	auto push_fore = fore;
	paint_map_info_background();
	proc();
	fore = push_fore;
	font = push_font;
}

void paint_main_map() {
	update_pallette_by_player();
	paint_background(SCREEN);
	update_pallette_colors();
	input_game_menu();
	paint_spice();
	paint_game_map();
	paint_map_info(paint_unit_info);
	input_game_map();
	paint_radar();
	paint_radar_rect();
	update_next_turn();
}

static void mouse_cancel(rect rc) {
	if(hot.mouse.in(rc)) {
		if((hot.key == MouseLeft || hot.key == MouseRight || hot.key == MouseLeftDBL) && hot.pressed)
			execute(buttoncancel);
	}
}

void paint_main_map_choose_terrain() {
	update_pallette_by_player();
	paint_background(SCREEN);
	update_pallette_colors();
	paint_spice();
	paint_game_map();
	paint_map_info(paint_choose_terrain);
	paint_radar();
	paint_radar_rect();
	mouse_cancel({0, 0, getwidth(), area_screen.y1});
	update_next_turn();
}

void paint_main_map_choose_placement() {
	update_pallette_by_player();
	paint_background(SCREEN);
	update_pallette_colors();
	paint_spice();
	paint_game_map();
	paint_map_info(paint_choose_terrain_placement);
	paint_radar();
	paint_radar_rect();
	mouse_cancel({0, 0, getwidth(), area_screen.y1});
	update_next_turn();
}

void check_animation_time() {
	if(animate_stop && animate_time >= animate_stop)
		execute(buttonok);
}

static void paint_video_fps() {
	pushvalue push(caret);
	pushvalue push_font(font, gres(FONT8));
	pushvalue push_fore(fore, colors::white);
	auto seconds = (animate_time - start_video) / 1000;
	auto minutes = seconds / 60;
	string sb; sb.add("%1.2i:%2.2i", minutes, seconds % 60);
	caret.x = 4; caret.y = 4; text(sb, -1, TextBold);
}

void paint_video() {
	paint_background(colors::black);
	auto push_font = font; font = gres(FONT16);
	auto push_caret = caret;
	auto ps = gres(animate_id);
	if(ps) {
		caret.x += (320 - ps->width) / 2;
		caret.y += imax((100 - ps->height) / 2, 24);
		auto frame = get_frame(animate_delay);
		if(animate_once) {
			if(frame >= ps->count) {
				frame = ps->count - 1;
				if(!animate_stop)
					execute(buttonok);
			}
		} else
			frame = frame % ps->count;
		image(ps, frame, 0);
		caret.x = push_caret.x;
		caret.y += ps->height + 8;
		texta(form_header, AlignCenter | ImagePallette);
	} else {
		pushrect push;
		setoffset(32, 32);
		texta(form_header, AlignCenterCenter | ImagePallette);
	}
	mouse_cancel({0, 0, getwidth(), getheight()});
	if(hot.key == KeySpace || hot.key == KeyEscape || hot.key == KeyEnter)
		execute(buttoncancel);
	check_animation_time();
	font = push_font;
	if(debug_toggle)
		paint_video_fps();
}

long show_scene_raw(fnevent before_paint, fnevent input, void* focus) {
	while(ismodal()) {
		before_paint();
		if(input)
			input();
		domodal();
		common_input();
	}
	return getresult();
}

long show_scene(fnevent before_paint, fnevent input, void* focus) {
	pushrect push;
	pushscene push_scene;
	current_focus = focus;
	return show_scene_raw(before_paint, input, focus);
}

static void default_time(unsigned long& milliseconds) {
	if(!milliseconds)
		milliseconds = 1 * 1000;
}

void appear_scene(fnevent before_paint, unsigned long milliseconds) {
	default_time(milliseconds);
	screenshoot before;
	before_paint();
	screenshoot after;
	before.blend(after, milliseconds);
	update_tick();
}

void disappear_scene(color back, unsigned long milliseconds) {
	default_time(milliseconds);
	screenshoot before;
	pushrect push;
	auto push_fore = fore; fore = back;
	caret.x = 0; caret.y = 0; width = getwidth(); height = getheight();
	rectf();
	fore = push_fore;
	screenshoot after;
	before.blend(after, milliseconds);
	update_tick();
}

void reset_form_animation() {
	update_tick();
	form_opening_tick = animate_time;
}

static void main_beforemodal() {
	clear_focus_data();
}

static void make_pallette() {
	auto p = gres(ICONS);
	if(!p)
		return;
	memcpy(pallette, p->ptr(p->get(0).pallette), sizeof(pallette));
	memcpy(pallette_original, pallette, sizeof(pallette));
	palt = pallette;
}

void initialize_view(const char* title, fnevent main_scene) {
	draw::create(-1, -1, 320, 200, 0, 32, false);
	draw::setcaption(title);
	draw::settimer(40);
	draw::pbeforemodal = main_beforemodal;
	font = gres(FONT6);
	fore = colors::white;
	make_pallette();
	set_next_scene(main_scene);
	run_next_scene();
}

BSDATA(drawrenderi) = {
	{"PaintUnit", bsdata<unit>::source, bsdata<unit>::elements, paint_unit},
	{"PaintEffectFix", bsdata<draweffect>::source, bsdata<draweffect>::elements, paint_effect_fix},
};
BSDATAF(drawrenderi)