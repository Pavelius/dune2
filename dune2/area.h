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

enum areaf : unsigned char {
	Explored, Visible,
};
struct areai {
	typedef void(*fnset)(point v, int value);
	typedef bool(*fntest)(point v);
	static constexpr int mx = 64;
	static constexpr int my = 64;
	point			maximum;
	void			clear();
	point			correct(point v) const;
	void			blockland(movementn mv) const;
	void			blockcontrol() const;
	void			controlwave(point start, fntest proc, int range) const;
	void			decoy(point v);
	terrainn		get(point v) const;
	point			getcorner(point v) const;
	featuren		getfeature(point v) const;
	unsigned short	getframe(point v) const { return frames[v.y][v.x]; }
	unsigned		getframeside(point v, terrainn t) const;
	unsigned		getframefow(point v, unsigned player, areaf t) const;
	unsigned short	getframefeature(point v) const { return frames_overlay[v.y][v.x]; }
	bool			isrock(point v) const { return get(v) >= Rock; }
	bool			issand(point v) const { return get(v) < Rock; }
	bool			isvalid(int x, int y) const { return x >= 0 && x < maximum.x && y >= 0 && y < maximum.y; }
	bool			isvalid(point v) const { return isvalid(v.x, v.y); }
	bool			is(point v, terrainn t) const;
	bool			is(point v, unsigned char player, areaf t) const { return (flags[player][v.y][v.x] & (1 << t)) != 0; }
	bool			isbuilding(point v) const;
	bool			isn(point v, terrainn t) const;
	bool			isn(point v, unsigned char player, areaf t) const;
	void			movewave(point v, movementn mv, point size) const;
	void			movewave(point v, movementn mv) const;
	direction		moveto(point start, direction wanted_direction = Center) const;
	point			nearest(point v, fntest proc, int radius) const;
	void			set(point v, terrainn t);
	void			set(point v, featuren t, int param = 0);
	void			set(point v, shapen t, const short unsigned* frame_list);
	void			set(point v, shapen t, featuren f);
	void			set(rect v, fnset proc, int value);
	void			set(point v, unsigned char player, areaf t) { flags[player][v.y][v.x] |= (1 << t); }
	void			setcamera(point v, bool center_view);
	void			random(rect r, fnset proc, int value);
	void			random(rect r, fnset proc, int value, int count);
	void			remove(unsigned char player, areaf t);
	void			scouting(point v, unsigned char player, int radius);
	void			scouting(point v, point size, unsigned char player, int radius);
private:
	unsigned char	flags[6][my][mx]; // Flags for each player (0 - neutral and 1-6)
	short unsigned	frames[my][mx];
	short unsigned	frames_overlay[my][mx];
	void			setnu(point v, terrainn t);
	void			update(point v);
};
extern areai area;
extern point area_origin;
extern point area_spot;
extern rect area_screen;
extern unsigned short map_alternate[area_frame_maximum];
extern unsigned short path_map[areai::my][areai::mx];

void area_initialization();
bool allowcontrol(point v);
bool allowbuild(point v);
void blockarea(areai::fntest proc);
void setareascout(point v, int player_index);
void setnofeature(point v, int param);

point find_smallest_position();
inline point m2s(point v) { return point(v.x * area_tile_width, v.y * area_tile_height); }
inline point m2sc(point v) { return point(v.x * area_tile_width + area_tile_width / 2, v.y * area_tile_height + area_tile_height / 2); }
inline point s2m(point v) { return point(v.x / area_tile_width, v.y / area_tile_height); }
inline point to(point v, direction d) { return v + getpoint(d); }
inline rect s2r(point s) { return {s.x, s.y, s.x + area_tile_width, s.y + area_tile_height}; }