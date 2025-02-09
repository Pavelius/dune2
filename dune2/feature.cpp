#include "bsdata.h"
#include "feature.h"
#include "terrain.h"
#include "math.h"

BSDATA(featurei) = {
	{"NoFeature"},
	{"Explosion", 1, 12},
	{"AircraftRemains", 13, 3, 30},
	{"CarRemains", 16, 3, 20},
	{"StructureRemains", 125, 1},
	{"Body", 19, 2, 70},
	{"Bodies", 21, 2, 70},
	{"Blood", 23, 2, 90},
	{"Trail", 25, 8, 30},
	{"Slab", 126},
	{"Wall", 34, 75},
	{"BuildingHead"},
	{"BuildingLeft"},
	{"BuildingUp"},
	{"FogOfWar", 108, 16},
};
assert_enum(featurei, FogOfWar)