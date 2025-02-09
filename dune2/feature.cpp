#include "bsdata.h"
#include "feature.h"
#include "terrain.h"
#include "math.h"

BSDATA(featurei) = {
	{"NoFeature", 0},
	{"Explosion", 1, 12, 2},
	{"AircraftRemains", 13, 3, 0, 30},
	{"CarRemains", 16, 3, 0, 20},
	{"StructureRemains", 125, 1},
	{"Body", 19, 2, 0, 40},
	{"Bodies", 21, 2, 0, 40},
	{"Blood", 23, 2, 2, 10},
	{"Trail", 25, 8, 0, 30},
	{"Slab", 126, 1},
	{"Wall", 34, 75},
	{"BuildingHead"},
	{"BuildingLeft"},
	{"BuildingUp"},
	{"FogOfWar", 108, 16},
};
assert_enum(featurei, FogOfWar)