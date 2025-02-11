#include "area.h"
#include "bsdata.h"
#include "direction.h"
#include "draw.h"
#include "drawable.h"
#include "fix.h"
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

static point drag_mouse_start;

static color color_form = color(186, 190, 150);
static color color_form_light = color(251, 255, 203);
static color color_form_shadow = color(101, 101, 77);

static unsigned long form_opening_tick;
static unsigned long next_turn_tick;
static unsigned long eye_clapping, eye_show_cursor;
unsigned long animate_time;
resid mentat_subject;

static bool debug_toggle;

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
	if(!next_turn_tick || (next_turn_tick < animate_time && (animate_time - next_turn_tick) >= 3000))
		next_turn_tick = animate_time;
	while(next_turn_tick < animate_time) {
		next_turn_tick += 1000;
		update_game_turn();
	}
}

static int get_frame(unsigned long resolution) {
	if(!resolution)
		resolution = 200;
	return (animate_time - form_opening_tick) / resolution;
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
	fore = push_fore;
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
	const sprite* font;
	pushscene() : pushfocus(), font(draw::font) {
		form_opening_tick = getcputime();
		animate_time = form_opening_tick;
	}
	~pushscene() { draw::font = font; }
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
	return {v.x / 4, v.y / 4};
}

static bool mouse_dragged(point mouse) {
	if(!hot.pressed)
		drag_mouse_start = point(-10000, -10000);
	else if(drag_mouse_start == point(-10000, -10000))
		drag_mouse_start = mouse;
	else
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

static void paint_sprites(resid id, color border, point offset, int index, int& focus, int per_line, int image_flags) {
	auto p = gres(id);
	if(!p)
		return;
	auto push_line = caret;
	auto count = per_line;
	while(index < p->count) {
		image(p, index, image_flags);
		if(focus == index) {
			auto push_fore = fore;
			auto push_caret = caret;
			caret = caret - offset;
			fore = border;
			rectb();
			caret = push_caret;
			fore = push_fore;
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

static void show_sprites(resid id, point start, point size, color backgc, color border) {
	rectpush push;
	auto push_fore = fore;
	auto push_font = font;
	set_font(FONT6);
	int focus = 0;
	auto maximum = gres(id)->count;
	auto per_line = 320 / size.x;
	auto maximum_height = (getheight() / size.y);
	auto origin = 0;
	auto image_flags = 0;
	while(ismodal()) {
		if(focus < 0)
			focus = 0;
		else if(focus > maximum - 1)
			focus = maximum - 1;
		if(focus < origin)
			origin = (focus / per_line) * per_line;
		else if(focus > origin + (maximum_height)*per_line)
			origin = ((focus - ((maximum_height - 1) * per_line)) / per_line) * per_line;
		paint_background(backgc);
		width = size.x;
		height = size.y;
		caret = caret + start;
		fore = colors::white;
		paint_sprites(id, border, start, origin, focus, per_line, image_flags);
		caret = {1, getheight() - 8};
		auto& f = gres(id)->get(focus);
		auto pf = const_cast<sprite::frame*>(&f);
		text(str("index %1i (size %2i %3i center %4i %5i)", focus, f.sx, f.sy, f.ox, f.oy), -1, TextBold);
		domodal();
		switch(hot.key) {
		case KeyRight: focus++; break;
		case KeyLeft: focus--; break;
		case KeyDown: focus += per_line; break;
		case KeyUp: focus -= per_line; break;
		case KeyEscape: breakmodal(0); break;
		case 'A': pf->ox--; break;
		case 'S': pf->ox++; break;
		case 'W': pf->oy--; break;
		case 'Z': pf->oy++; break;
		case 'H': image_flags ^= ImageMirrorH; break;
		case 'V': image_flags ^= ImageMirrorV; break;
		}
		focus_input();
	}
	font = push_font;
	fore = push_fore;
}

static void random_explosion() {
	static fixn source[] = {FixBikeExplosion, FixExplosion, FixBigExplosion};
	auto n = source[rand() % sizeof(source) / sizeof(source[0])];
	// auto n = FixHitSand;
	add_area_effect(area_spot, n);
}

static void common_input() {
	update_tick();
	switch(hot.key) {
	case Ctrl + F5: make_screenshoot(); break;
	case Ctrl + 'S': show_sprites(SHAPES, {0, 0}, {32, 24}, color(64, 0, 128), colors::blue); break;
	case Ctrl + 'I': show_sprites(ICONS, {0, 0}, {16, 16}, color(24, 0, 64), colors::white); break;
	case Ctrl + 'A': show_sprites(UNITS1, {8, 8}, {16, 16}, color(24, 0, 64), colors::white); break;
	case Ctrl + 'B': show_sprites(UNITS2, {8, 8}, {16, 16}, color(24, 0, 64), colors::white); break;
	case Ctrl + 'C': show_sprites(UNITS, {8, 8}, {16, 16}, color(24, 0, 64), colors::white); break;
	case Ctrl + 'M': show_sprites(MOUSE, {0, 0}, {16, 16}, color(24, 0, 64), colors::white); break;
	case 'A': area.set(area_spot, d100() < 60 ? CarRemains : AircraftRemains); break;
	case 'B': area.set(area_spot, Blood); break;
	case 'D': debug_toggle = !debug_toggle; break;
	case 'E': random_explosion(); break;
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
	if(!area.isvalid(v))
		return {-1, -1};
	return {(short)(v.x * area_tile_width + area_screen.x1), (short)(v.y * area_tile_height + area_screen.y1)};
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

static void handle_mouse_select() {
	auto area_spot = i2s(hot.mouse);
	if(mouse_dragged(area_spot)) {
		rectpush push;
		auto start = s2i(drag_mouse_start);
		caret = hot.mouse;
		width = start.x - caret.x;
		height = start.y - caret.y;
		rectb();
	}
}

static void paint_game_map() {
	auto push_clip = clipping; setclip(area_screen);
	auto need_check_corner_slice = false;
	camera = m2s(area_origin);
	if(hot.mouse.in(area_screen)) {
		area_spot.x = (hot.mouse.x - area_screen.x1) / area_tile_width + area_origin.x;
		area_spot.y = (hot.mouse.y - area_screen.y1) / area_tile_height + area_origin.y;
		need_check_corner_slice = true;
		handle_main_map_mouse_input();
	} else
		area_spot = 0xFFFF;
	paint_map_tiles();
	paint_map_features();
	paint_objects();
	if(need_check_corner_slice) {
		check_mouse_corner_slice();
		handle_mouse_select();
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
	texta("Dmg", AlignCenter | TextMoveCaret | TextSingleLine);
	caret = push_caret;
	width = push_width;
	caret.y += 24;
}

static void paint_choose_panel(const char* id, int avatar, long cancel_result) {
	auto y2 = caret.y + height;
	texta(getnm(id), AlignCenter | TextSingleLine | TextMoveCaret);
	image(gres(SHAPES), avatar, 0);
	caret.y += 32; height = y2 - caret.y - texth() - 4 - 2;
	// rectb_black();
	texta(getnm(ids(id, "Info")), AlignCenterCenter);
	caret.y += height;
	button(getnm("Cancel"), 0, KeyEscape, AlignCenterCenter, false, buttonparam, cancel_result);
}

static void paint_cursor(int avatar) {
	if(!area.isvalid(area_spot))
		return;
	auto pt = map_to_screen(area_spot - area_origin) + point(8, 8);
	image(pt.x, pt.y, gres(MOUSE), avatar, 0);
}

static void paint_choose_terrain() {
	paint_choose_panel("ChooseTarget", 17, (long)point(-1000, -1000));
	paint_cursor(5);
}

static void paint_unit_orders() {
	rectpush push;
	setoffset(-1, 0);
	height = 12;
	button("Attack", 0, '1', AlignCenter, false, buttonok, 0);
	button("Move", 0, 'M', AlignCenter, false, buttonok, 0);
	button("Harvest", 0, 0, AlignCenter, false, buttonok, 0);
	button("Guard", 0, 'G', AlignCenter, false, buttonok, 0);
}

static void paint_unit_info() {
	if(!last_unit)
		return;
	rectpush push;
	texta(last_unit->getname(), AlignCenter | TextMoveCaret | TextSingleLine);
	paint_unit_panel();
	caret.y += 1;
	paint_unit_orders();
	caret.y += 1;
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
	paint_radar_screen();
	paint_radar_rect();
	update_next_turn();
}

static void mouse_cancel(rect rc) {
	if(hot.mouse.in(rc)) {
		if(hot.key == MouseLeft || hot.key == MouseRight || hot.key == MouseLeftDBL)
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

long show_scene(fnevent before_paint, fnevent input, void* focus) {
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
	return getresult();
}

static void main_beforemodal() {
	clear_focus_data();
}

void initialize_view(const char* title, fnevent main_scene) {
	draw::create(-1, -1, 320, 200, 0, 32);
	draw::setcaption(title);
	draw::settimer(40);
	draw::pbeforemodal = main_beforemodal;
	set_font(FONT8);
	fore = colors::white;
	set_next_scene(main_scene);
	run_next_scene();
}

BSDATA(drawrenderi) = {
	{"PaintUnit", bsdata<unit>::source, bsdata<unit>::elements, paint_unit},
	{"PaintEffectFix", bsdata<draweffect>::source, bsdata<draweffect>::elements, paint_effect_fix},
};
BSDATAF(drawrenderi)