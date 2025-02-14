#include "area.h"
#include "bsdata.h"
#include "building.h"

BSDATA(buildingi) = {
	{"ConstructionYard"},
	{"SpiceSilo"},
	{"Starport"},
	{"Windtrap", Shape2x2, {304, 305, 306, 307}},
	{"Refinery", Shape3x2, {332, 333, 334, 337, 338, 339}},
	{"RadarOutpost", Shape2x2, {}},
	{"RepairFacility", Shape3x2, {}},
	{"HouseOfIX"},
	{"Palace"},
	{"Barracks"},
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

void addobj(point pt, buildingn id) {
	last_building = bsdata<building>::addz();
	last_building->position = pt;
	last_building->type = id;
	auto& e = last_building->geti();
	area.set(last_building->position, e.shape, e.frames);
}

building* find_building(point v) {
	for(auto& e : bsdata<building>()) {
		if(e && e.position == v)
			return &e;
	}
	return 0;
}