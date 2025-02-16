#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "game.h"
#include "resid.h"
#include "topicablea.h"

static buildingn base_produce[] = {Barracks, Windtrap, Refinery, SpiceSilo};

BSDATA(buildingi) = {
	{"ConstructionYard", CONSTRUC, 60, 0, 1000, Shape2x2, {292, 293, 295, 296}, {}, base_produce},
	{"SpiceSilo", STORAGE, 69, 100, 500, Shape2x2, {372, 373, 375, 376}},
	{"Starport"},
	{"Windtrap", WINDTRAP, 61, 100, 500, Shape2x2, {304, 305, 306, 307}},
	{"Refinery", REFINERY, 64, 500, 1500, Shape3x2, {332, 333, 334, 337, 338, 339}},
	{"RadarOutpost"},
	{"RepairFacility"},
	{"HouseOfIX"},
	{"Palace"},
	{"Barracks", BARRAC, 62, 300, 1500, Shape2x2, {285, 286, 288, 289}},
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
}

building* find_building(point v) {
	for(auto& e : bsdata<building>()) {
		if(e && e.position == v)
			return &e;
	}
	return 0;
}

void building::destroy() {

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