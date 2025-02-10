#pragma once

#include "direction.h"
#include "feature.h"
#include "point.h"
#include "rect.h"
#include "shape.h"
#include "terrain.h"

const int area_screen_x1 = 0;
const int area_screen_y1 = 40;
const int area_screen_width = 15;
const int area_screen_height = 10;
const int area_tile_width = 16;
const int area_tile_height = 16;
const int area_frame_maximum = 389;

struct areai {
	typedef void(*fnsetarea)(point v, int value);
	static constexpr int mx = 128;
	static constexpr int my = 128;
	point			maximum;
	void			clear();
	point			correct(point v) const;
	void			decoy(point v);
	void			decoy();
	terrainn		get(point v) const;
	featuren		getfeature(point v) const;
	unsigned short	getframe(point v) const { return frames[v.y][v.x]; }
	unsigned		getframeside(point v, terrainn t) const;
	unsigned short	getframefeature(point v) const { return frames_overlay[v.y][v.x]; }
	bool			isvalid(int x, int y) const { return x >= 0 && x < maximum.x && y >= 0 && y < maximum.y; }
	bool			isvalid(point v) const { return isvalid(v.x, v.y); }
	bool			is(point v, terrainn t) const;
	bool			isbuilding(point v) const;
	bool			isn(point v, terrainn t) const;
	void			set(point v, terrainn t);
	void			set(point v, featuren t, int ft = 0);
	void			set(point v, shapen t, short unsigned* frame_list);
	void			set(rect v, fnsetarea proc, int value);
	void			setcamera(point v, bool center_view);
	void			random(rect r, fnsetarea proc, int value);
	void			random(rect r, fnsetarea proc, int value, int count);
private:
	short unsigned	frames[my][mx];
	short unsigned	frames_overlay[my][mx];
	void			setnu(point v, terrainn t);
	void			update(point v);
};
extern areai area;
extern point area_origin;
extern point area_spot;
extern direction all_strait_directions[4];
extern unsigned short map_alternate[area_frame_maximum];

void area_initialization();

point getpoint(direction d);
inline point m2s(point v) { return point(v.x * area_tile_width, v.y * area_tile_height); }
inline point m2sc(point v) { return point(v.x * area_tile_width + area_tile_width / 2, v.y * area_tile_height + area_tile_height / 2); }
inline point s2m(point v) { return point(v.x / area_tile_width, v.y / area_tile_height); }
inline point to(point v, direction d) { return v + getpoint(d); }