#include "bsdata.h"
#include "building.h"

enum buildingn : unsigned char {
	ConstructionYard, SpiceSilo, Starport, Windtrap, Refinery, RadarOutpost, RepairFacility, HouseOfIX, Palace,
	Barracks, WOR, LightVehicleFactory, HeavyVehicleFactory, HighTechFacility,
	Turret, RocketTurret,
};

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