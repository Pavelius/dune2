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
const unsigned short TargetArea = 0xFFFE;

enum areaf : unsigned char {
	Explored, Visible, Control,
};
enum areasizen : unsigned char {
	SmallMap, MediumMap, LargeMap,
};
struct tilepatch {
	short unsigned	from;
	short unsigned	to;
	unsigned char	action;
	unsigned char	id;
};
struct areai {
	typedef void(*fnset)(point v, int value);
	typedef bool(*fntest)(point v);
	static constexpr int mx = 64;
	static constexpr int my = 64;
	static constexpr int region_maximum = 16;
	rect			regions[region_maximum];
	point			maximum;
	areasizen		sizetype;
	void			blockbuildingne(unsigned char player) const;
	void			blockbuildings() const;
	void			blockbuildings(unsigned char player) const;
	void			blockland(movementn mv) const;
	void			blockland(movementn mv, unsigned char player) const;
	void			blockland(terrainn v) const;
	void			blockset(point start, point size, unsigned short value) const;
	void			blockunits() const;
	void			blockunits(point exclude) const;
	void			changealternate();
	point			center() const { return point(maximum.x / 2, maximum.y / 2); }
	void			clear(areasizen v);
	point			correct(point v) const;
	void			controlwave(point start, fntest proc, int range) const;
	void			decoy(point v);
	terrainn		get(point v) const;
	point			getcorner(point v) const;
	featuren		getfeature(point v) const;
	unsigned short	getframe(point v) const { return frames[v.y][v.x]; }
	unsigned		getframeside(point v, terrainn t) const;
	unsigned		getframefow(point v, unsigned player, areaf t) const;
	unsigned short	getframefeature(point v) const { return frames_overlay[v.y][v.x]; }
	point			getregion(int n) const { return ::center(regions[n]); }
	point			m2r(point v) const;
	bool			isblocked(point v, movementn move) const;
	bool			isexist(point v, int range, fntest proc) const;
	bool			isrock(point v) const { return get(v) >= Rock; }
	bool			issand(point v) const { return get(v) < Rock; }
	bool			isvalid(int x, int y) const { return x >= 0 && x < maximum.x && y >= 0 && y < maximum.y; }
	bool			isvalid(point v) const { return isvalid(v.x, v.y); }
	bool			is(point v, terrainn t) const;
	bool			is(point v, unsigned char player, areaf t) const { return (flags[player][v.y][v.x] & (1 << t)) != 0; }
	bool			isbuilding(point v) const;
	bool			isn(point v, terrainn t) const;
	bool			isn(point v, unsigned char player, areaf t) const;
	void			movewave(point v, movementn mv) const;
	direction		moveto(point start, direction wanted_direction = Center) const;
	point			nearest(point v, fntest proc, int radius) const;
	point			nearest(point start, point goal, movementn move, int range, unsigned char player) const;
	point			nearestpf(point v, fntest proc, int radius) const;
	void			patch(point v, const tilepatch* tiles, size_t count, bool apply);
	void			patch(point v, point size, const tilepatch* tiles, size_t count, bool apply);
	void			random(rect r, fnset proc, int value);
	void			random(rect r, fnset proc, int value, int count);
	void			random(point v, int s, int r, fnset proc, int value);
	void			random(point v, int s, int r, fnset proc, int value, size_t count);
	void			remove(unsigned char player, areaf t);
//	int				scan(point v, int range, fntest proc) const;
	void			scouting(point v, unsigned char player, int radius);
	void			scouting(point v, point size, unsigned char player, int radius);
	void			set(point v, terrainn t);
	void			set(point v, featuren t, int param = 0);
	void			set(point v, shapen t, const short unsigned* frame_list);
	void			set(point v, shapen t, featuren f);
	void			set(point v, direction d, short unsigned frame);
	void			set(point v, point s, fnset proc, int value);
	void			set(rect v, fnset proc, int value);
	void			set(point v, unsigned char player, areaf t) { flags[player][v.y][v.x] |= (1 << t); }
private:
	unsigned char	flags[6][my][mx]; // Flags for each player (0 - neutral and 1-6)
	short unsigned	frames[my][mx];
	short unsigned	frames_overlay[my][mx];
	void			setnu(point v, terrainn t);
	void			update(point v);
};
extern areai area;
extern point area_spot;
extern rect area_screen;
extern terrainn map_terrain[area_frame_maximum];
extern featuren map_features[area_frame_maximum];
extern unsigned short map_alternate[area_frame_maximum];
extern unsigned short path_map[areai::my][areai::mx];
extern unsigned short path_map_copy[areai::my][areai::mx];
extern unsigned char region_central[16];
extern unsigned char region_near_h[16];
extern unsigned char region_near_v[16];

void area_initialization();
void area_generate(areasizen n, int number_of_players);
bool allowcontrol(point v);
bool allowbuild(point v);
void blockarea(areai::fntest proc);
void blockarea(areai::fntest proc, point size);
void blockareaor(areai::fntest proc, point size);
void blockclear();
void copypath();
bool isspice(point v);
void setareascout(point v, int player_index);
void setnofeature(point v, int param);

rect allarea();

point find_smallest_position();
inline point m2s(point v) { return point(v.x * area_tile_width, v.y * area_tile_height); }
inline point m2sc(point v) { return point(v.x * area_tile_width + area_tile_width / 2, v.y * area_tile_height + area_tile_height / 2); }
inline point s2m(point v) { return point(v.x / area_tile_width, v.y / area_tile_height); }
inline point to(point v, direction d) { return v + getpoint(d); }
inline rect s2r(point s) { return {s.x, s.y, s.x + area_tile_width, s.y + area_tile_height}; }
inline int r2i(point s) { return s.y * 4 + s.x; }