#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "game.h"
#include "resid.h"
#include "topicablea.h"

static buildingn base_produce[] = {Barracks, Windtrap, Refinery, SpiceSilo, RadarOutpost};

BSDATA(buildingi) = {
	{"ConstructionYard", CONSTRUC, 60, 0, 1000, Shape2x2, {292, 293, 295, 296}, {}, base_produce},
	{"SpiceSilo", STORAGE, 69, 100, 500, Shape2x2, {372, 373, 375, 376}, {}, {}, {0, 0, 0, 0, 1000}},
	{"Starport"},
	{"Windtrap", WINDTRAP, 61, 100, 500, Shape2x2, {304, 305, 306, 307}, {}, {}, {0, 0, 0, 0, 0}},
	{"Refinery", REFINERY, 64, 500, 1500, Shape3x2, {332, 333, 334, 337, 338, 339}, {}, {}, {0, 0, 0, 0, 1000}},
	{"RadarOutpost", HEADQRTS, 70, 500, 1000, Shape2x2, {379, 380, 386, 387}, {}, {}, {0, 0, 0, 0, 0}},
	{"RepairFacility"},
	{"HouseOfIX"},
	{"Palace"},
	{"Barracks", BARRAC, 62, 300, 1500, Shape2x2, {285, 286, 288, 289}, {}, {}, {0, 0, 0, 0, 0}},
	{"WOR"},
	{"LightVehicleFactory"},
	{"HeavyVehicleFactory"},
	{"HighTechFacility"},
	{"Turret"},
	{"RocketTurret"}
};
assert_enum(buildingi, RocketTurret)

BSDATAC(building, 512)
building* last_building;

building::operator bool() const {
	return area.isvalid(position);
}

void add_building(point pt, buildingn id) {
	last_building = bsdata<building>::addz();
	last_building->position = pt;
	last_building->type = id;
	auto& e = last_building->geti();
	last_building->hits = e.hits;
	if(e.build)
		last_building->build = e.build[0];
	area.set(last_building->position, e.shape, e.frames);
	last_building->scouting();
}

building* find_building(point v) {
	for(auto& e : bsdata<building>()) {
		if(e && e.position == v)
			return &e;
	}
	return 0;
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

void building::construct(point v) {
	if(!area.isvalid(v))
		return;
	add_building(v, build);
	build_spend = 0;
}

void building::cancel() {
	if(build_spend && canbuild()) {
		getplayer().add(Credits, build_spend);
		build_spend = 0;
	}
}

void building::update() {
	if(isworking())
		progress();
}

int	building::getprogress() const {
	if(!build_spend)
		return 0;
	auto build_cost = bsdata<buildingi>::elements[build].cost;
	if(!build_cost)
		return 100;
	if(build_spend >= build_cost)
		return 100;
	return build_spend * 100 / build_cost;
}

point building::getbuildsize() const {
	if(!build)
		return {};
	return bsdata<shapei>::elements[bsdata<buildingi>::elements[build].shape].size;
}

point building::getsize() const {
	return bsdata<shapei>::elements[geti().shape].size;
}

bool building::progress() {
	if(canbuild()) {
		unsigned short can_spend = 10;
		auto build_cost = bsdata<buildingi>::elements[build].cost;
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

void building::canbuildlist() const {
	subjects.clear();
	for(auto n : geti().build)
		subjects.add(bsdata<buildingi>::elements + n);
}

rect building::getrect() const {
	auto size = getsize();
	return {position.x, position.y, position.x + size.x, position.y + size.y};
}