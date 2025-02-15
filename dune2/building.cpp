#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "resid.h"

BSDATA(buildingi) = {
	{"ConstructionYard", CONSTRUC, 60, 1000, Shape2x2, {292, 293, 295, 296}},
	{"SpiceSilo"},
	{"Starport"},
	{"Windtrap", WINDTRAP, 61, 500, Shape2x2, {304, 305, 306, 307}},
	{"Refinery", NONE, 64, 1000, Shape3x2, {332, 333, 334, 337, 338, 339}},
	{"RadarOutpost"},
	{"RepairFacility"},
	{"HouseOfIX"},
	{"Palace"},
	{"Barracks", BARRAC},
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