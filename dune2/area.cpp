#include "area.h"
#include "bsdata.h"
#include "slice.h"
#include "rand.h"

areai area;
pointc area_origin, area_spot;

static terrainn map_terrain[area_frame_maximum];
static featuren map_features[area_frame_maximum];
static unsigned char map_count[area_frame_maximum];
static unsigned short map_alternate[area_frame_maximum];

static pointc stack[256 * 16];
static size_t push_stack, pop_stack;
direction all_strait_directions[4] = {Up, Right, Down, Left};

static pointc pop_value() {
	if(pop_stack >= sizeof(stack) / sizeof(stack[0]))
		pop_stack = 0;
	return stack[pop_stack++];
}

static void push_value(pointc v) {
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
		return Rock; // All buildings builded on rock only
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

static featuren find_feature_by_frame(int frame) {
	for(auto& e : bsdata<featurei>()) {
		if(frame >= e.frame && frame < e.frame + e.count)
			return (featuren)(&e - bsdata<featurei>::elements);
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

unsigned short areai::getframe(pointc v) const {
	if(!isvalid(v))
		return 0;
	return frames[v.y][v.x];
}

static unsigned short get_decoy_frame(terrainn t, int b, int i, int s) {
	if(t == Rock)
		return b;
	return b + i / s;
}

unsigned areai::getframeside(pointc v, terrainn t) const {
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

bool areai::is(pointc v, terrainn t) const {
	if(!isvalid(v))
		return false;
	return map_terrain[frames[v.y][v.x]] == t;
}

int	areai::surround(pointc v, terrainn t) const {
	auto r = 0;
	for(auto d : all_strait_directions) {
		if(isn(to(v, d), t))
			r++;
	}
	return r;
}

bool areai::isn(pointc v, terrainn t) const {
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

pointc areai::correct(pointc v) const {
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

void areai::set(pointc v, featuren f, int ft) {
	if(!isvalid(v))
		return;
	auto i = variable[v.y][v.x];
	auto t = get(v);
	if(t == Mountain)
		return;
	switch(f) {
	case Explosion:
		if(features[v.y][v.x] != f) {
			features[v.y][v.x] = f;
			variable[v.y][v.x] = rand() % 2;
		} else if(i < 4)
			variable[v.y][v.x] += 2;
		else
			variable[v.y][v.x] = 4 + rand() % 2;
		break;
	case Blood:
		if(features[v.y][v.x] != f) {
			features[v.y][v.x] = f;
			variable[v.y][v.x] = rand() % 2;
		}
		break;
	case Trail:
		if(t >= Rock)
			return;
		if(features[v.y][v.x] != f) {
			features[v.y][v.x] = f;
			variable[v.y][v.x] = ft;
		}
		break;
	default:
		if(features[v.y][v.x] != f) {
			features[v.y][v.x] = f;
			variable[v.y][v.x] = ft;
		}
		break;
	}
}

unsigned short areai::getframefeature(pointc v) const {
	if(!isvalid(v))
		return 0;
	auto t = get(v);
	auto f = features[v.y][v.x];
	auto i = variable[v.y][v.x];
	switch(f) {
	case Explosion:
		switch(t) {
		case Rock: return 1 + (i % 6);
		case Sand: case Dune: case Spice: case SpiceRich: case SpiceBlow: return 7 + (i % 6);
		default: return 0;
		}
	case AircraftRemains: return get_decoy_frame(t, 13, i, 10);
	case CarRemains: return get_decoy_frame(t, 16, i, 10);
	case Body: return get_decoy_frame(t, 19, i, 15);
	case Bodies: return get_decoy_frame(t, 21, i, 15);
	case Blood: return 23 + i % 2;
	case Slab: return 126;
	}
	return 0;
}

void areai::decoy(pointc v) {
	auto f = features[v.y][v.x];
	auto& e = bsdata<featurei>::elements[f];
	if(e.decoy) {
		if(variable[v.y][v.x] < 30) {
			if(d100() < e.decoy) {
				if(e.count)
					variable[v.y][v.x] += e.count;
				else
					variable[v.y][v.x]++;
			}
		}
		// Finally decoying
		if(variable[v.y][v.x] >= 30)
			set(v, NoFeature);
	}
}

void areai::decoy() {
	for(auto y = 0; y < maximum.y; y++)
		for(auto x = 0; x < maximum.x; x++)
			decoy(pointc(x, y));
}

void areai::setcamera(pointc v, bool center_view) {
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

pointc getpoint(direction d) {
	switch(d) {
	case Left: return {-1, 0};
	case LeftUp: return {-1, -1};
	case LeftDown: return {-1, 1};
	case Right: return {1, 0};
	case RightUp: return {1, -1};
	case RightDowm: return {1, 1};
	case Up: return {0, -1};
	case Down: return {0, 1};
	default: return {0, 0};
	}
}

void areai::set(rect r, fnsetarea proc, int value) {
	for(auto y = r.y1; y <= r.y2; y++) {
		for(auto x = r.x1; x <= r.x2; x++) {
			if(isvalid(x, y))
				proc(pointc(x, y), value);
		}
	}
}

void areai::random(rect r, fnsetarea proc, int value) {
	auto x = xrand(r.x1, r.x2);
	auto y = xrand(r.y1, r.y2);
	if(isvalid(x, y))
		proc(pointc(x, y), value);
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

terrainn areai::get(pointc v) const {
	if(!isvalid(v))
		return Sand;
	return map_terrain[frames[v.y][v.x]];
}

void areai::set(pointc v, terrainn t) {
	if(!isvalid(v))
		return;
	// First we set default frame
	frames[v.y][v.x] = bsdata<terraini>::elements[t].frame;
	// Second we change frame around
	for(auto d : all_strait_directions) {
		auto n = to(v, d);
		setnu(n, get(n));
	}
	// Third update original frame
	setnu(v, t);
}

void areai::setnu(pointc v, terrainn t) {
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