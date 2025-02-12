#pragma once

#include "direction.h"
#include "feature.h"
#include "movement.h"
#include "point.h"
#include "shape.h"
#include "terrain.h"

const int area_tile_width = 16;
const int area_tile_height = 16;
const int area_frame_maximum = 389;
const unsigned short BlockArea = 0xFFFF;

struct areai {
	typedef void(*fnsetarea)(point v, int value);
	static constexpr int mx = 64;
	static constexpr int my = 64;
	point			maximum;
	void			clear();
	point			correct(point v) const;
	void			blockland(movementn mv) const;
	void			decoy(point v);
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
	void			makewave(point v, movementn mv) const;
	direction		moveto(point start, direction wanted_direction = Center) const;
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
extern rect area_screen;
extern direction all_strait_directions[4];
extern unsigned short map_alternate[area_frame_maximum];
extern unsigned short path_map[areai::my][areai::mx];

void area_initialization();

point getpoint(direction d);
inline point m2s(point v) { return point(v.x * area_tile_width, v.y * area_tile_height); }
inline point m2sc(point v) { return point(v.x * area_tile_width + area_tile_width / 2, v.y * area_tile_height + area_tile_height / 2); }
inline point s2m(point v) { return point(v.x / area_tile_width, v.y / area_tile_height); }
inline point to(point v, direction d) { return v + getpoint(d); }
inline rect s2r(point s) { return {s.x, s.y, s.x + area_tile_width, s.y + area_tile_height}; }
direction to(direction d, direction s);
