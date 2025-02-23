#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "fix.h"
#include "game.h"
#include "pointa.h"
#include "resid.h"
#include "topicablea.h"

BSLINK(buildingn, buildingi)
BSLINK(unitn, uniti)

static topicable* base_produce[] = {
	BS(Slab), BS(Slab4),
	BS(RadarOutpost), BS(Barracks), BS(Wor), BS(LightVehicleFactory), BS(Turret), BS(RocketTurret),
	BS(Refinery), BS(SpiceSilo),
	BS(Windtrap),
};
static topicable* barrac_produce[] = {
	BS(LightInfantry),
};
static topicable* wor_produce[] = {
	BS(HeavyInfantry),
};
static topicable* lftr_produce[] = {
	BS(Trike),
	BS(Quad),
};
static tilepatch refinery_tiles[] = {
	{334, 342, BoardUnit},
	{339, 344, BoardUnit},
};

BSDATA(buildingi) = {
	{"ConstructionYard", CONSTRUC, 60, 0, Shape2x2, {292, 293, 295, 296}, base_produce, {}, {}},
	{"SpiceSilo", STORAGE, 69, 100, Shape2x2, {372, 373, 375, 376}, {}, {0}, {1000}},
	{"Starport", STARPORT, 57},
	{"Windtrap", WINDTRAP, 61, 100, Shape2x2, {304, 305, 306, 307}, {}, {}, {0, 1000}},
	{"Refinery", REFINERY, 64, 500, Shape3x2, {332, 333, 334, 337, 338, 339}, {}, {0, 1000}, {1000}, refinery_tiles},
	{"RadarOutpost", HEADQRTS, 70, 500, Shape2x2, {379, 380, 386, 387}, {}, {}, {}, {}, Refinery},
	{"RepairFacility", REPAIR},
	{"HouseOfIX"},
	{"Palace", PALACE, 54},
	{"Barracks", BARRAC, 62, 300, Shape2x2, {299, 300, 301, 302}, barrac_produce, {0, 200}},
	{"WOR", WOR, 59, 500, Shape2x2, {285, 286, 288, 289}, wor_produce, {0, 300}, {}, {}, Barracks},
	{"LightVehicleFactory", LITEFTRY, 55, 1000, Shape2x2, {241, 242, 248, 249}, lftr_produce, {0, 500}, {}, {}, RadarOutpost},
	{"HeavyVehicleFactory", HVYFTRY, 56, 2000},
	{"HighTechFacility"},
	{"Slab", SLAB, 53, 2, Shape1x1, {126}},
	{"Slab4", SLAB4, 71, 5, Shape2x2},
	{"Turret", TURRET, 67, 300, Shape1x1, {356}, {}, {}, {}, {}, RadarOutpost},
	{"RocketTurret", RTURRET, 68, 500, Shape1x1, {364}, {}, {}, {}, {}, RadarOutpost}
};
assert_enum(buildingi, RocketTurret)

BSDATAC(building, 512)
building* last_building;

void building::clear() {
	memset(this, 0, sizeof(*this));
}

buildingn buildingi::getindex() const {
	if(!this)
		return Slab;
	return (buildingn)(this - bsdata<buildingi>::elements);
}

void building::updateturrets() {
	if(type == Turret || type == RocketTurret)
		area.set(position, shoot_direction, geti().frames[0]);
}

void add_building(point pt, buildingn id) {
	last_building = bsdata<building>::addz();
	last_building->position = pt;
	last_building->type = id;
	last_building->unit_board = 0xFFFF;
	auto& e = last_building->geti();
	last_building->hits = bsdata<shapei>::elements[e.shape].hits;
	area.set(last_building->position, e.shape, e.frames);
	last_building->scouting();
	last_building->shoot_direction = Down;
	last_building->player = player_index;
	last_building->stop();
	area.set(last_building->getrect(), setnofeature, 0);
}

static const tilepatch* find_patch(const tilepatch* pb, const tilepatch* pe, int action) {
	while(pb < pe) {
		if(pb->action == action)
			return pb;
		pb++;
	}
	return 0;
}

static const tilepatch* find_patch_ne(const tilepatch* pb, const tilepatch* pe, int action) {
	while(pb < pe) {
		if(pb->action != action)
			return pb;
		pb++;
	}
	return 0;
}

building* find_building(point v) {
	for(auto& e : bsdata<building>()) {
		if(e && e.position == v)
			return &e;
	}
	return 0;
}

building* find_board(const unit* p) {
	if(!p || !p->isboard())
		return 0;
	auto v = p->getindex();
	for(auto& e : bsdata<building>()) {
		if(e && e.unit_board == v)
			return &e;
	}
	return 0;
}

void building::board(unit* p) {
	if(unit_board != 0xFFFF)
		return;
	unit_board = p->getindex();
	set(BoardUnit, true);
	p->position = {-100, -100};
	p->order = p->position;
	p->screen = m2sc(p->position);
}

void building::unboard() {
	if(unit_board == 0xFFFF)
		return;
	auto v = area.nearest(position, isfreetrack, 8);
	if(!area.isvalid(v))
		return;
	set(BoardUnit, false);
	auto p = bsdata<unit>::elements + unit_board;
	p->position = v;
	p->screen = m2sc(p->position);
	p->stopmove();
	unit_board = 0xFFFF;
}

void building::set(buildstaten action, bool apply) {
	auto& ei = geti();
	auto pb = ei.tiles.begin();
	auto pe = ei.tiles.end();
	auto p1 = find_patch(pb, pe, action);
	if(!p1)
		return;
	auto p2 = find_patch_ne(p1, pe, action);
	if(!p2)
		p2 = pe;
	area.patch(position, getsize(), p1, p2 - p1, apply);
}

void building::scouting() {
	area.scouting(position, getsize(), player, getlos());
}

void building::cleanup() {
	if(last_building == this)
		last_building = 0;
}

static short unsigned* destroyed_shape(shapen v) {
	static short unsigned d1x1[] = {355};
	static short unsigned d2x2[] = {213, 214, 232, 233};
	static short unsigned d3x2[] = {213, 214, 215, 232, 233, 234};
	static short unsigned d3x3[] = {213, 214, 215, 232, 233, 234, 223, 224, 225};
	switch(v) {
	case Shape1x1: return d1x1;
	case Shape2x2: return d2x2;
	case Shape3x2: return d3x2;
	default: return 0;
	}
}

static void set_terrain(point v, int value) {
	area.set(v, (terrainn)value);
}

static void set_explosion(point v, int value) {
	if(game_chance(20))
		add_effect(m2sc(v), FixBigExplosion);
	else
		add_effect(m2sc(v), FixExplosion);
}

static void apply_destroyed_structures(point position, shapen v) {
	auto p = destroyed_shape(v);
	if(p)
		area.set(position, v, p);
	else
		area.set(position, bsdata<shapei>::elements[v].size, set_terrain, Rock);
	area.set(position, bsdata<shapei>::elements[v].size, set_explosion, 0);
}

void building::destroy() {
	cleanup();
	apply_destroyed_structures(position, geti().shape);
	clear();
}

static point random_point(const building* p) {
	auto m = p->getsize();
	m.x = p->position.x + xrand(0, m.x - 1);
	m.y = p->position.y + xrand(0, m.y - 1);
	return m2sc(m);
}

void building::damage(int value) {
	if(value >= hits) {
		destroy();
		return;
	}
	hits -= value;
	if(hits <= gethitsmax() / 2) {
		if(game_chance(30))
			add_effect(random_point(this), BurningFire);
	}
	if(game_chance(20))
		add_effect(random_point(this), Smoke);
}

int	building::getlos() const {
	switch(type) {
	case RadarOutpost: return 8;
	default: return 2;
	}
}

void building::setbuild(const topicable* v) {
	auto index = 0;
	for(auto p : geti().build) {
		if(p == v) {
			build_index = index;
			return;
		}
		index++;
	}
}

topicable* building::getbuild() const {
	auto& ei = geti();
	if(!ei.build)
		return 0;
	return ei.build[build_index];
}

static void set_control(point v, shapen shape, unsigned char player) {
	auto& ei = bsdata<shapei>::elements[shape];
	for(auto y = 0; y < ei.size.y; y++) {
		for(auto x = 0; x < ei.size.x; x++) {
			auto n = point(x, y) + v;
			if(!area.isvalid(n))
				continue;
			area.set(n, player, Control);
		}
	}
}

void building::construct(point v) {
	if(!area.isvalid(v))
		return;
	auto pe = getbuild();
	if(bsdata<buildingi>::have(pe)) {
		auto pb = ((buildingi*)pe);
		auto t = pb->getindex();
		if(t == Slab || t == Slab4) {
			auto size = bsdata<shapei>::elements[pb->shape].size;
			area.set(v, pb->shape, SlabFeature);
			set_control(v, pb->shape, player);
			area.scouting(v, size, player, getlos());
		} else {
			add_building(v, t);
			set_control(v, pb->shape, player);
		}
	}
	build_spend = 0;
}

void building::cancel() {
	if(build_spend && canbuild()) {
		getplayer().add(Credits, build_spend);
		build_count = 0;
		build_spend = 0;
	}
}

static point get_unit_place_point(point start, int range) {
	//pointa points;
	area.blockland(Tracked);
	blockunits();
	//area.controlwave(start, allowcontrol, range);
	//blockarea(isunitpossible);
	//blockunits();
	//points.selectfree();
	//return points.random();
	return area.nearest(start, isnonblocked, range);
}

bool building::autoproduct() {
	auto pe = getbuild();
	if(!pe)
		return false;
	if(build_spend < pe->cost)
		return false;
	if(!bsdata<uniti>::have(pe))
		return false;
	auto v = get_unit_place_point(position, 32);
	if(area.isvalid(v))
		add_unit(v, (unitn)getbsi((uniti*)pe), to(position, v));
	build_spend = 0;
	return true;
}

void building::update() {
	const auto shoot_range = 8;
	tracking();
	if(isworking()) {
		progress();
		if(autoproduct()) {
			if(build_count) {
				progress();
				build_count--;
			}
		}
	} else if(shoot(m2sc(position), Shoot20mm, 2, shoot_range)) {
		// Nothing to do
	} else if(area.isvalid(target_position) && !canshoot(shoot_range)) {
		stop();
	} else if(seeking(shoot_range)) {
		// Nothing to do
	}
	updateturrets();
}

fixn building::getweapon() const {
	switch(type) {
	case Turret: return Shoot30mm;
	case RocketTurret: return FireRocket;
	default: return NoEffect;
	}
}

int	building::getprogress() const {
	if(!build_spend)
		return 0;
	auto pe = getbuild();
	if(!pe)
		return 0;
	auto build_cost = pe->cost;
	if(!build_cost)
		return 100;
	if(build_spend >= build_cost)
		return 100;
	return build_spend * 100 / build_cost;
}

int	building::gethitsmax() const {
	return bsdata<shapei>::elements[geti().shape].hits;
}

point building::getbuildsize() const {
	auto pe = getbuild();
	if(!pe)
		return {};
	if(bsdata<buildingi>::have(pe))
		return bsdata<shapei>::elements[((buildingi*)pe)->shape].size;
	return {1, 1};
}

point building::getsize() const {
	return bsdata<shapei>::elements[geti().shape].size;
}

bool building::isnear(point v) const {
	auto size = getsize();
	return v.x >= position.x - 1 && v.x <= position.x + size.x
		&& v.y >= position.y - 1 && v.y <= position.y + size.y;
}

bool building::progress() {
	auto pe = getbuild();
	if(pe) {
		unsigned short can_spend = 5;
		auto build_cost = pe->cost;
		if(build_cost <= 10)
			can_spend = 1;
		auto credits = getplayer().get(Credits);
		if(can_spend > credits)
			can_spend = credits;
		if(build_spend + can_spend > build_cost)
			can_spend = build_cost - build_spend;
		if(can_spend > 0) {
			getplayer().add(Credits, -can_spend);
			build_spend += can_spend;
			return true;
		}
	}
	return false;
}

void building::buildlist() const {
	subjects.clear();
	for(auto p : geti().build) {
		if(bsdata<buildingi>::have(p)) {
			auto pb = static_cast<buildingi*>(p);
			if(pb->required && !getplayer().buildings[pb->required])
				continue;
		}
		subjects.add(p);
	}
}

rect building::getrect() const {
	auto size = getsize();
	return {position.x, position.y, position.x + size.x, position.y + size.y};
}

building* find_base(buildingn type, unsigned char player) {
	for(auto& e : bsdata<building>()) {
		if(!e)
			continue;
		if(e.player == player && e.type == type)
			return &e;
	}
	return 0;
}

void building::setblock(short unsigned n) const {
	auto size = getsize();
	auto x2 = position.x + size.x;
	auto y2 = position.y + size.y;
	for(auto y = position.y; y < y2; y++) {
		for(auto x = position.x; x < x2; x++) {
			if(area.isvalid(x, y))
				path_map[y][x] = n;
		}
	}
}

point building::nearestboard(point v, movementn move) const {
	if(!area.isvalid(v))
		return {-10000, -10000};
	blockland(move);
	blockunits();
	path_map[v.y][v.x] = 0;
	unblock();
	area.movewave(position, move); // Consume time action
	if(path_map[position.y][position.x] == BlockArea)
		return {-10000, -10000};
	block();
	return find_smallest_position();
}