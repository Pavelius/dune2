#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "game.h"
#include "pointa.h"
#include "resid.h"
#include "topicablea.h"

BSLINK(buildingn, buildingi)
BSLINK(unitn, uniti)

static topicable* base_produce[] = {
	BS(Slab), BS(Slab4),
	BS(RadarOutpost), BS(Barracks), BS(Wor), BS(LightVehicleFactory),
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
	{334, 342, 0},
	{339, 344, 0},
	{342, 334, 1},
	{344, 339, 1},
};

BSDATA(buildingi) = {
	{"ConstructionYard", CONSTRUC, 60, 0, 1000, Shape2x2, {292, 293, 295, 296}, {}, base_produce},
	{"SpiceSilo", STORAGE, 69, 100, 500, Shape2x2, {372, 373, 375, 376}, {}, {}, {0, 0, 0, 0, 1000}},
	{"Starport", STARPORT, 57},
	{"Windtrap", WINDTRAP, 61, 100, 500, Shape2x2, {304, 305, 306, 307}, {}, {}, {0, 0, 0, 0, 0}},
	{"Refinery", REFINERY, 64, 500, 1500, Shape3x2, {332, 333, 334, 337, 338, 339}, {}, {}, {0, 0, 0, 0, 1000}, refinery_tiles},
	{"RadarOutpost", HEADQRTS, 70, 500, 1000, Shape2x2, {379, 380, 386, 387}, {}, {}, {0, 0, 0, 0, 0}},
	{"RepairFacility", REPAIR},
	{"HouseOfIX"},
	{"Palace", PALACE, 54},
	{"Barracks", BARRAC, 62, 300, 1500, Shape2x2, {299, 300, 301, 302}, {}, barrac_produce, {0, 0, 0, 0, 0}},
	{"WOR", WOR, 59, 500, 1500, Shape2x2, {285, 286, 288, 289}, {}, wor_produce, {}},
	{"LightVehicleFactory", LITEFTRY, 55, 1000, 2000, Shape2x2, {241, 242, 248, 249}, {}, lftr_produce},
	{"HeavyVehicleFactory", HVYFTRY, 56, 2000, 2000},
	{"HighTechFacility"},
	{"Slab", SLAB, 53, 2, 0, Shape1x1, {126}},
	{"Slab4", SLAB4, 71, 5, 0, Shape2x2, {}},
	{"Turret", TURRET},
	{"RocketTurret", RTURRET}
};
assert_enum(buildingi, RocketTurret)

BSDATAC(building, 512)
building* last_building;

static tilepatch refinery_unload[] = {
	{334, 342}, {342, 334},
	{339, 344}, {344, 339},
};

building::operator bool() const {
	return area.isvalid(position);
}

buildingn buildingi::getindex() const {
	if(!this)
		return Slab;
	return (buildingn)(this - bsdata<buildingi>::elements);
}

void add_building(point pt, buildingn id) {
	last_building = bsdata<building>::addz();
	last_building->position = pt;
	last_building->type = id;
	last_building->unit_board = 0xFFFF;
	auto& e = last_building->geti();
	last_building->hits = e.hits;
	area.set(last_building->position, e.shape, e.frames);
	last_building->scouting();
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
	p->wait(500);
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
	p->order = p->position;
	p->screen = m2sc(p->position);
	unit_board = 0xFFFF;
}

void building::set(buildstaten action, bool apply) {
	auto& ei = geti();
	auto pb = ei.tilepatches.begin();
	auto pe = ei.tilepatches.end();
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

void building::destroy() {

}

int	building::getlos() const {
	switch(type) {
	case RadarOutpost: return 8;
	default: return 2;
	}
}

topicable* building::getbuild() const {
	auto& ei = geti();
	if(!ei.build)
		return 0;
	return ei.build[build_index];
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
			area.scouting(v, size, player, getlos());
		} else
			add_building(v, t);
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
	pointa points;
	area.blockcontrol();
	area.controlwave(start, allowcontrol, range);
	blockarea(isunitpossible);
	blockunits(0);
	points.selectfree();
	return points.random();
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
	if(isworking()) {
		progress();
		if(autoproduct()) {
			if(build_count) {
				progress();
				build_count--;
			}
		}
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
	for(auto p : geti().build)
		subjects.add(p);
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