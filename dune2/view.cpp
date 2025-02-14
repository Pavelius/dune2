#include "area.h"
#include "bsdata.h"
#include "direction.h"
#include "draw.h"
#include "drawable.h"
#include "fix.h"
#include "fraction.h"
#include "game.h"
#include "io_stream.h"
#include "math.h"
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

using namespace draw;

static point drag_mouse_start, drag_mouse_finish;
static bool drag_begin;

static color color_form = color(186, 190, 150);
static color color_form_light = color(251, 255, 203);
static color color_form_shadow = color(101, 101, 77);
static color font_pallette[16];

const char* form_header;
static unsigned long form_opening_tick;
static unsigned long next_game_time;
static unsigned long eye_clapping, eye_show_cursor;
unsigned long animate_time, animate_delay = 200, animate_stop;
resid animate_id;
bool animate_once;
bool debug_toggle;

// External for debug tools. In release mode must be removed by linker.
void view_debug_input();

static void debug_map_message() {
	rectpush push;
	caret.x = clipping.x1 + 2; caret.y = clipping.y1 + 2;
	auto t = area.get(area_spot);
	string sb;
	sb.add("Area %1i,%2i %3", area_spot.x, area_spot.y, bsdata<terraini>::elements[t].getname());
	if(area.isbuilding(area_spot))
		sb.adds("Building");
	text(sb.text, -1, TextStroke);
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

static int get_frame(unsigned long resolution) {
	if(!resolution)
		resolution = 200;
	return (animate_time - form_opening_tick) / resolution;
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

static void paint_background(color v) {
	auto push_fore = fore;
	fore = v; rectf();
	caret.x = (getwidth() - 320) / 2;
	caret.y = (getheight() - 200) / 2;
	width = 320;
	height = 200;
	fore = push_fore;
}

void create_title_font_pallette() {
	font_pallette[1] = fore;
	font_pallette[2] = fore_stroke;
	font_pallette[3] = colors::gray.mix(fore_stroke, 64);
	font_pallette[5] = fore.mix(fore_stroke, 140);
	font_pallette[6] = fore.mix(fore_stroke, 107);
	palt = font_pallette;
}

static void form_frame(color light_left_up, color shadow_right_down) {
	auto push_fore = fore;
	auto push_caret = caret;
	fore = light_left_up;
	line(caret.x + width - 1, caret.y);
	fore = shadow_right_down;
	line(caret.x, caret.y + height - 1);
	line(caret.x - (width - 1), caret.y);
	fore = light_left_up;
	line(caret.x, caret.y - (height - 2));
	fore = light_left_up.mix(shadow_right_down, 128);
	pixel(push_caret.x + width - 1, push_caret.y);
	pixel(push_caret.x, push_caret.y + height - 1);
	caret = push_caret;
	fore = push_fore;
}

static void form_frame() {
	auto push_fore = fore;
	fore = color_form;
	rectf();
	fore = push_fore;
}

static void form_frame_rect() {
	form_frame(color_form_light, color_form_shadow);
	setoffset(1, 1);
}

static void form_frame(int thickness) {
	rectpush push;
	while(thickness > 0) {
		form_frame_rect();
		thickness--;
	}
	form_frame();
}

static void button_press_effect() {
	auto size = sizeof(color) * (width - 1);
	for(auto y = caret.y + height - 2; y >= caret.y; y--) {
		memmove(canvas->ptr(caret.x + 1, y + 1), canvas->ptr(caret.x, y), size);
		*((color*)canvas->ptr(caret.x, y)) = color();
		*((color*)canvas->ptr(caret.x, y + 1)) = color();
	}
	memset(canvas->ptr(caret.x, caret.y), 0, width * sizeof(color));
}

static void rectb_black() {
	auto push_fore = fore;
	fore = colors::black;
	rectb();
	fore = push_fore;
}

static bool button(const char* title, const void* button_data, unsigned key, unsigned flags = TextBold | TextSingleLine, bool allow_set_focus = false) {
	rectpush push;
	draw::height = texth() + 5;
	rectb_black();
	setoffset(1, 1);
	auto push_fore = fore;
	if(!button_data)
		button_data = (void*)title;
	auto run = button_input(button_data, key, allow_set_focus);
	auto pressed = (pressed_focus == button_data);
	if(pressed) {
		rectb_black();
		caret.x++;
		caret.y++;
		width--; height--;
	}
	form_frame(1);
	if(current_focus == button_data)
		fore = colors::active;
	setoffset(1, 1);
	caret.y += 1; height -= 1;
	texta(title, flags);
	fore = push_fore;
	return run;
}

static void button(const char* title, const void* button_data, unsigned key, unsigned flags, bool allow_set_focus, fnevent proc, long param) {
	if(button(title, button_data, key, flags, allow_set_focus))
		execute(proc, param, 0, button_data);
	caret.y += texth() + 4;
}

struct pushscene : pushfocus {
	pushscene() : pushfocus() {
		form_opening_tick = getcputime();
		animate_time = form_opening_tick;
	}
};

bool time_animate(unsigned long& value, unsigned long duration, unsigned long pause = 20) {
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

static point same_point(point v) {
	return {v.x / 2, v.y / 2};
}

static bool mouse_dragged(point mouse, fnevent dropped) {
	switch(hot.key) {
	case MouseLeft:
		if(hot.pressed) {
			if(!drag_begin) {
				drag_begin = true;
				drag_mouse_start = mouse;
			}
		} else {
			if(drag_begin) {
				drag_begin = false;
				drag_mouse_finish = mouse;
				execute(dropped);
			} else
				drag_mouse_start = point(-10000, -10000);
		}
		break;
	}
	if(drag_begin)
		return same_point(drag_mouse_start) != same_point(mouse);
	return false;
}

static bool mouse_hower(unsigned long duration = 1000, bool single_time = true) {
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

static void make_screenshoot() {
	auto index = get_file_number("screenshoots", "scr*.bmp");
	char temp[260]; stringbuilder sb(temp);
	sb.add("screenshoots/scr%1.5i.bmp", index);
	write(temp,
		draw::canvas->ptr(0, 0), canvas->width, canvas->height, canvas->bpp, canvas->scanline, 0);
}

static void common_input() {
	update_tick();
	switch(hot.key) {
	case Ctrl + F5: make_screenshoot(); break;
	}
#ifdef _DEBUG
	view_debug_input();
#endif
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
	auto ps = gres(animate_id);
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
	case LeftUp: return 0;
	case Up: return 1;
	case RightUp: return 2;
	case Right: return 3;
	case RightDown: return 4;
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
	static direction all[] = {LeftUp, Up, RightUp, Right, RightDown, Down, LeftDown, Left};
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
	return {(short)(v.x * area_tile_width + area_screen.x1), (short)(v.y * area_tile_height + area_screen.y1)};
}

static void paint_cursor(int avatar, point offset) {
	auto v = area_spot - area_origin;
	if(!area.isvalid(v))
		return;
	auto pt = map_to_screen(v) + offset;
	image(pt.x, pt.y, gres(MOUSE), avatar, 0);
	if(hot.key == MouseLeft && hot.pressed)
		execute(buttonparam, (long)area_spot);
}

static void paint_main_map_debug() {
	if(!debug_toggle)
		return;
	paint_cursor(5, {8, 8});
	debug_map_message();
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
			image(x * area_tile_width + caret.x, y * area_tile_height + caret.y, ps, i, 0);
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
	case Up: image(ps, frame + 0, 0); break;
	case RightUp: image(ps, frame + 1, 0); break;
	case Right: image(ps, frame + 2, 0); break;
	case RightDown: image(ps, frame + 3, 0); break;
	case Down: image(ps, frame + 4, 0); break;
	case LeftDown: image(caret.x, caret.y, ps, frame + 3, ImageMirrorH); break;
	case Left: image(caret.x, caret.y, ps, frame + 2, ImageMirrorH); break;
	case LeftUp: image(caret.x, caret.y, ps, frame + 1, ImageMirrorH); break;
	default: break;
	}
}

static void paint_unit(const uniti& e, direction move_direction, direction shoot_direction) {
	paint_platform(gres(e.res), e.frame, move_direction);
	if(e.frame_shoot)
		paint_platform(gres(e.res), e.frame_shoot, shoot_direction);
}

static void paint_unit() {
	auto p = static_cast<unit*>(last_object);
	auto& e = p->geti();
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
	rectpush push;
	caret.x = getwidth() - 64 + area_origin.x;
	caret.y = getheight() - 64 + area_origin.y;
	width = area_screen.width() / area_tile_width;
	height = area_screen.height() / area_tile_height;
	rectb();
}

static void paint_radar_screen() {
	auto push_fore = fore;
	auto x1 = getwidth() - 64;
	auto y1 = getheight() - 64;
	for(auto y = 0; y < area.maximum.y; y++) {
		for(auto x = 0; x < area.maximum.x; x++) {
			auto t = area.get(point(x, y));
			if(t > Mountain)
				continue;
			fore = bsdata<terraini>::elements[t].minimap;
			pixel(x1 + x, y1 + y);
		}
	}
	point hot_mouse = hot.mouse;
	hot_mouse.x -= x1;
	hot_mouse.y -= y1;
	if((hot_mouse.x < 64 && hot_mouse.x > 0) && (hot_mouse.y < 64 && hot_mouse.y > 0)) {
		if(hot.key == MouseLeft && hot.pressed)
			execute(set_area_view, (long)(hot_mouse), 1);
	}
	fore = push_fore;
}

static void copybits(int x, int y, int width, int height, int x1, int y1) {
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

static void fillbitsh(int x, int y, int width, int height, int total_width) {
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

static void fillbitsv(int x, int y, int width, int height, int total_height) {
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
		image(0, 0, gres(rid), 0, 0);
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
	human_selected.select(player, rc);
}

static void attack_by_mouse() {
}

static rect drag_finish_rect(int minimal) {
	rect rc;
	rc.x1 = drag_mouse_start.x;
	rc.y1 = drag_mouse_start.y;
	rc.x2 = drag_mouse_finish.x;
	rc.y2 = drag_mouse_finish.y;
	rc.normalize();
	if(rc.width() < minimal && rc.height() < minimal) {
		rc.x1 = rc.centerx() - minimal / 2; rc.x2 = rc.x1 + minimal;
		rc.y1 = rc.centery() - minimal / 2; rc.y2 = rc.y1 + minimal;
	}
	return rc;
}

static void selection_rect_dropped() {
	selection_rect_dropped(drag_finish_rect(area_tile_width));
}

static void rectb_alpha_drag() {
	rectpush push;
	auto start = s2i(drag_mouse_start);
	caret = hot.mouse;
	width = start.x - caret.x;
	height = start.y - caret.y;
	rectb_alpha();
}

static void input_game_map() {
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
	if(mouse_dragged(i2s(hot.mouse), selection_rect_dropped))
		rectb_alpha_drag();
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
	if(area.isvalid(area_spot)) {
		check_mouse_corner_slice();
	}
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
	rectpush push;
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

static void paint_unit_panel() {
	auto push_caret = caret;
	auto push_width = width;
	image(gres(SHAPES), last_unit->geti().frame_avatar, 0);
	caret.x += 32 + 1; width -= 32 + 1;
	paint_health_bar(last_unit->hits, last_unit->getmaximum(Hits));
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
	button(getnm("Cancel"), 0, KeyEscape, AlignCenterCenter, false, buttonparam, cancel_result);
}

static void paint_choose_terrain() {
	paint_choose_panel("ChooseTarget", 17, (long)point(-1000, -1000));
	paint_cursor(5, {8, 8});
}

static void paint_unit_orders() {
	rectpush push;
	setoffset(-1, 0);
	height = 12;
	button("Attack", 0, 'A', AlignCenter, false, human_unit_attack, 0);
	button("Move", 0, 'M', AlignCenter, false, human_unit_move, 0);
	button("Harvest", 0, 0, AlignCenter, false, buttonok, 0);
	button("Stop", 0, 'G', AlignCenter, false, human_unit_stop, 0);
}

static void paint_unit_list() {
	auto push_caret = caret;
	caret = push_caret + point(6, 8);
	for(auto p : human_selected) {
		paint_unit(p->geti(), RightDown, RightDown);
		caret.x += 16;
		if(caret.x >= clipping.x2) {
			caret.x = push_caret.x;
			if(p != *human_selected.end() - 1)
				caret.y += 16;
		}
	}
	caret = push_caret;
}

static void paint_unit_info() {
	rectpush push;
	if(!human_selected)
		return;
	else if(human_selected.count == 1) {
		auto push_unit = last_unit;
		last_unit = human_selected[0];
		texta(last_unit->getname(), AlignCenter | TextSingleLine); caret.y += texth() - 1;
		paint_unit_panel();
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
	rectpush push;
	auto push_font = font;
	auto push_fore = fore;
	paint_map_info_background();
	proc();
	fore = push_fore;
	font = push_font;
}

void paint_main_map() {
	paint_background(SCREEN);
	paint_game_map();
	paint_map_info(paint_unit_info);
	input_game_map();
	paint_radar_screen();
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
	paint_background(SCREEN);
	paint_game_map();
	paint_map_info(paint_choose_terrain);
	paint_radar_screen();
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
	auto ps = gres(animate_id);
	if(!ps || !ps->count)
		return;
	auto push_font = font; font = gres(FONT16);
	auto push_caret = caret;
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
	if(form_header) {
		auto push_palt = palt;
		create_title_font_pallette();
		texta(form_header, AlignCenter | ImagePallette);
		palt = push_palt;
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
	rectpush push;
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

void disappear_scene(unsigned long milliseconds) {
	default_time(milliseconds);
	screenshoot before;
	rectpush push;
	auto push_fore = fore; fore = colors::black;
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
	spot_unit = 0;
	clear_focus_data();
}

void initialize_view(const char* title, fnevent main_scene) {
	draw::create(-1, -1, 320, 200, 0, 32, false);
	draw::setcaption(title);
	draw::settimer(40);
	draw::pbeforemodal = main_beforemodal;
	font = gres(FONT6);
	fore = colors::white;
	set_next_scene(main_scene);
	run_next_scene();
}

BSDATA(drawrenderi) = {
	{"PaintUnit", bsdata<unit>::source, bsdata<unit>::elements, paint_unit},
	{"PaintEffectFix", bsdata<draweffect>::source, bsdata<draweffect>::elements, paint_effect_fix},
};
BSDATAF(drawrenderi)