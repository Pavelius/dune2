#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "shape.h"
#include "slice.h"
#include "rand.h"

areai area;
point area_origin, area_spot;

static terrainn map_terrain[area_frame_maximum];
static featuren map_features[area_frame_maximum];
static unsigned char map_count[area_frame_maximum];
unsigned short map_alternate[area_frame_maximum];

static point stack[256 * 16];
static size_t push_stack, pop_stack;
direction all_strait_directions[4] = {Up, Right, Down, Left};

static point pop_value() {
	if(pop_stack >= sizeof(stack) / sizeof(stack[0]))
		pop_stack = 0;
	return stack[pop_stack++];
}

static void push_value(point v) {
	if(push_stack >= sizeof(stack) / sizeof(stack[0]))
		push_stack = 0;
	stack[push_stack++] = v;
}

static bool stack_valid() {
	return push_stack != pop_stack;
}

static void clear_stack() {
	push_stack = pop_stack = 0;
}

static terrainn find_terrain_by_frame(int frame) {
	if(frame >= 210)
		return Rock; // All buildings (and their remains) builded on rock only
	if(frame >= 34 && frame <= 126)
		return Rock; // All walls, slabs (and their remains) builded on rock. Fog of war don't count - is newer be terrain frame.
	for(auto& e : bsdata<terraini>()) {
		if(frame >= e.frame && frame < e.frame + e.count)
			return (terrainn)(&e - bsdata<terraini>::elements);
	}
	return Sand;
}

static short unsigned get_alternate_frame(int frame) {
	switch(frame) {
	case 216: return 219;
	case 242: return 243;
	case 254: return 257;
	case 276: return 279;
	case 283: return 284;
	case 288: return 290;
	case 292: return 294;
	case 302: return 303;
	case 307: return 308;
	case 309: return 312;
	case 333: return 335;
	case 346: return 348;
	case 373: return 374;
	case 380: return 381;
	case 381: return 382;
	case 382: return 383;
	case 386: return 388;
	default: return 0;
	}
}

static int find_frame(const unsigned short* source, size_t count, unsigned short value) {
	for(size_t i = 0; i < count; i++) {
		if(source[i] == value)
			return i;
	}
	return -1;
}

static featuren find_feature_by_frame(int frame) {
	for(auto& e : bsdata<featurei>()) {
		if(frame >= e.frame && frame < e.frame + e.count)
			return (featuren)(&e - bsdata<featurei>::elements);
	}
	for(auto& e : bsdata<buildingi>()) {
		auto& s = bsdata<shapei>::elements[e.shape];
		auto i = find_frame(e.frames, s.count, frame);
		if(i != -1) {
			if(i == 0)
				return BuildingHead;
			else if(i < s.size.x)
				return BuildingLeft;
			else
				return BuildingUp;
		}
	}
	switch(frame) {
	case 213: case 214: case 215:
	case 223: case 224: case 225:
	case 232: case 233: case 234:
	case 240:
	case 246: case 247:
	case 313:
	case 355:
	case 377:case 378:
	case 384:case 385:
		return StructureRemains;
	default: return NoFeature;
	}
}

void area_initialization() {
	for(auto i = 0; i < area_frame_maximum; i++) {
		map_terrain[i] = find_terrain_by_frame(i);
		map_features[i] = find_feature_by_frame(i);
		map_alternate[i] = get_alternate_frame(i);
	}
}

void areai::clear() {
	memset(this, 0, sizeof(*this));
	maximum.x = 120;
	maximum.y = 120;
	// Fill all field by sand
	for(auto y = 0; y < maximum.y; y++) {
		for(auto x = 0; x < maximum.x; x++)
			frames[y][x] = 127;
	}
}

static unsigned short get_decoy_frame(terrainn t, int b, int i, int s) {
	if(t == Rock)
		return b;
	return b + i / s;
}

unsigned areai::getframeside(point v, terrainn t) const {
	unsigned r = 0;
	if(isn(v + getpoint(Left), t))
		r |= 1;
	r <<= 1;
	if(isn(v + getpoint(Down), t))
		r |= 1;
	r <<= 1;
	if(isn(v + getpoint(Right), t))
		r |= 1;
	r <<= 1;
	if(isn(v + getpoint(Up), t))
		r |= 1;
	return r;
}

bool areai::is(point v, terrainn t) const {
	if(!isvalid(v))
		return false;
	return map_terrain[frames[v.y][v.x]] == t;
}

bool areai::isn(point v, terrainn t) const {
	if(!isvalid(v))
		return true;
	auto n = map_terrain[frames[v.y][v.x]];
	if(n == t)
		return true;
	auto same = bsdata<terraini>::elements[t].same;
	if(same && same.is(n))
		return true;
	return false;
}

point areai::correct(point v) const {
	if(v.x < 0)
		v.x = 0;
	else if(v.y < 0)
		v.y = 0;
	else if(v.x > maximum.x)
		v.x = maximum.x;
	else if(v.y > maximum.y)
		v.y = maximum.y;
	return v;
}

featuren areai::getfeature(point v) const {
	if(!isvalid(v))
		return NoFeature;
	auto n = map_features[frames_overlay[v.y][v.x]];
	if(n)
		return n;
	return map_features[frames[v.y][v.x]];
}

static int get_feature_count(int frame) {
	switch(frame) {
	case 1: case 2: case 7: case 8:
		return 1;
	case 3: case 4: case 9: case 10:
		return 2;
	case 5: case 6: case 11: case 12:
		return 3;
	default: return 0;
	}
}

void areai::set(point v, featuren f, int ft) {
	if(!isvalid(v))
		return;
	if(f == NoFeature) { // This variant just clear existing feature
		frames_overlay[v.y][v.x] = NoFeature;
		return;
	}
	auto ct = get(v);
	if(isbuilding(v))
		return; // Features do not appear on building tile.
	if(ct == Mountain)
		return; // Frames overlay newer be on mountains
	else if(ct >= Rock && f == Trail)
		return; // Trails can be leaved only on sand (and like sand)
	auto& e = bsdata<featurei>::elements[f];
	auto cf = getfeature(v);
	if(cf != f) {
		if(e.random)
			frames_overlay[v.y][v.x] = e.frame + rand() % e.random;
		else
			frames_overlay[v.y][v.x] = e.frame;
		if(f == Explosion && ct < Rock)
			frames_overlay[v.y][v.x] += 6;
	} else {
		// If another explosion in same point, increase effect
		auto count = get_feature_count(frames_overlay[v.y][v.x]);
		if(count && count < 3)
			frames_overlay[v.y][v.x] += 2;
	}
}

static int get_next_decoy(terrainn t, int frame) {
	switch(frame) {
	case 13:
		if(t >= Rock)
			return -1;
		return 14;
	case 14: return 15;
	case 15: return -1;
	case 16:
		if(t >= Rock)
			return -1;
		return 17;
	case 17: return 18;
	case 18: return -1;
	case 19:
		if(t >= Rock)
			return -1;
		return 20;
	case 20: return -1;
	case 21:
		if(t >= Rock)
			return -1;
		return 22;
	case 22: return -1;
	case 23: case 24: return -1;
	default: return 0;
	}
}

void areai::decoy(point v) {
	auto cf = getfeature(v);
	if(!cf)
		return;
	auto& e = bsdata<featurei>::elements[cf];
	if(e.decoy && d100() < e.decoy) {
		auto n = get_next_decoy(get(v), frames_overlay[v.y][v.x]);
		if(n == -1)
			frames_overlay[v.y][v.x] = NoFeature;
		else if(n > 0)
			frames_overlay[v.y][v.x] = n;
	}
}

void areai::setcamera(point v, bool center_view) {
	int x = v.x;
	int y = v.y;
	if(center_view) {
		x -= area_screen_width / 2;
		y -= area_screen_height / 2;
	}
	if(x > maximum.x - area_screen_width)
		x = maximum.x - area_screen_width;
	if(y > maximum.y - area_screen_height)
		y = maximum.y - area_screen_height;
	if(x < 0)
		x = 0;
	if(y < 0)
		y = 0;
	area_origin.x = (char)x;
	area_origin.y = (char)y;
}

point getpoint(direction d) {
	switch(d) {
	case Left: return {-1, 0};
	case LeftUp: return {-1, -1};
	case LeftDown: return {-1, 1};
	case Right: return {1, 0};
	case RightUp: return {1, -1};
	case RightDown: return {1, 1};
	case Up: return {0, -1};
	case Down: return {0, 1};
	default: return {0, 0};
	}
}

void areai::set(rect r, fnsetarea proc, int value) {
	for(auto y = r.y1; y <= r.y2; y++) {
		for(auto x = r.x1; x <= r.x2; x++) {
			if(isvalid(x, y))
				proc(point(x, y), value);
		}
	}
}

void areai::random(rect r, fnsetarea proc, int value) {
	auto x = xrand(r.x1, r.x2);
	auto y = xrand(r.y1, r.y2);
	if(isvalid(x, y))
		proc(point(x, y), value);
}

void areai::random(rect r, fnsetarea proc, int value, int count) {
	if(!count)
		return;
	if(count < 0) {
		if(count <= -100)
			return;
		count = r.width() * r.height() * 100 / (-count);
	}
	for(auto i = 0; i < count; i++)
		random(r, proc, value);
}

terrainn areai::get(point v) const {
	if(!isvalid(v))
		return Sand;
	return map_terrain[frames[v.y][v.x]];
}

void areai::update(point v) {
	for(auto d : all_strait_directions) {
		auto n = to(v, d);
		if(isbuilding(n))
			continue; // Building not change frames
		setnu(n, get(n));
	}
}

void areai::set(point v, terrainn t) {
	if(!isvalid(v))
		return;
	// First we set default frame (neigtboard tiles will be see this terrain)
	frames[v.y][v.x] = bsdata<terraini>::elements[t].frame;
	// Second we change frame around
	update(v);
	// Third update original frame (neightboard already updated)
	setnu(v, t);
}

void areai::setnu(point v, terrainn t) {
	if(!isvalid(v))
		return;
	auto& e = bsdata<terraini>::elements[t];
	if(e.count == 16)
		frames[v.y][v.x] = e.frame + getframeside(v, t);
	else if(e.count) {
		auto t1 = get(v);
		if(t1 != t)
			frames[v.y][v.x] = e.frame + rand() % e.count;
	} else
		frames[v.y][v.x] = e.frame;
}

bool areai::isbuilding(point v) const {
	if(!isvalid(v))
		return false;
	return frames[v.y][v.x] >= 210;
	//auto t = get(v);
	//return t == BuildingHead || t == BuildingLeft || t == BuildingUp;
}

void areai::set(point v, shapen t, short unsigned* frame_list) {
	if(!frame_list)
		return;
	auto& e = bsdata<shapei>::elements[t];
	if(!e.count)
		return;
	for(auto i = 0; i < e.count; i++) {
		auto n = v + e.points[i];
		if(!isvalid(n))
			continue;
		frames[n.y][n.x] = frame_list[i];
	}
	for(auto i = 0; i < e.count; i++) {
		auto n = v + e.points[i];
		if(!isvalid(n))
			continue;
		update(n);
	}
}

direction to(direction d, direction s) {
	switch(s) {
	case Left:
		switch(d) {
		case Left: return LeftDown;
		case LeftUp: return Left;
		case Up: return LeftUp;
		case RightUp: return Up;
		case Right: return RightUp;
		case RightDown: return Right;
		case Down: return RightDown;
		case LeftDown: return Down;
		default: return Center;
		}
	case Right:
		switch(d) {
		case Left: return LeftUp;
		case LeftUp: return Up;
		case Up: return RightUp;
		case RightUp: return Right;
		case Right: return RightDown;
		case RightDown: return Down;
		case Down: return LeftDown;
		case LeftDown: return Left;
		default: return Center;
		}
	default: return Center;
	}
}