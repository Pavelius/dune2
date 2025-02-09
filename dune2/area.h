#pragma once

#include "direction.h"
#include "feature.h"
#include "pointc.h"
#include "rect.h"
#include "terrain.h"

const int area_screen_x1 = 0;
const int area_screen_y1 = 40;
const int area_screen_width = 15;
const int area_screen_height = 10;
const int area_tile_width = 16;
const int area_tile_height = 16;
const int area_frame_maximum = 389;

struct areai {
	typedef void(*fnsetarea)(pointc v, int value);
	static constexpr int mx = 128;
	static constexpr int my = 128;
	featuren		features[my][mx];
	unsigned char	variable[my][mx];
	short unsigned	frames[my][mx];
	short unsigned	frames_features[my][mx];
	pointc			maximum;
	void			clear();
	pointc			correct(pointc v) const;
	void			decoy(pointc v);
	void			decoy();
	terrainn		get(pointc v) const;
	unsigned short	getframe(pointc v) const;
	unsigned		getframeside(pointc v, terrainn t) const;
	unsigned short	getframefeature(pointc v) const;
	bool			isvalid(int x, int y) const { return x >= 0 && x < maximum.x && y >= 0 && y < maximum.y; }
	bool			isvalid(pointc v) const { return isvalid(v.x, v.y); }
	bool			is(pointc v, terrainn t) const;
	bool			isn(pointc v, terrainn t) const;
	void			set(pointc v, terrainn t);
	void			set(pointc v, featuren t, int ft = 0);
	void			set(rect v, fnsetarea proc, int value);
	void			setnu(pointc v, terrainn t);
	void			setcamera(pointc v, bool center_view);
	int				surround(pointc v, terrainn t) const;
	void			random(rect r, fnsetarea proc, int value);
	void			random(rect r, fnsetarea proc, int value, int count);
};
extern areai area;
extern pointc area_origin;
extern pointc area_spot;
extern direction all_strait_directions[4];

void area_initialization();

pointc getpoint(direction d);
inline pointc to(pointc v, direction d) { return v + getpoint(d); }