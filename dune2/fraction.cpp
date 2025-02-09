#include "bsdata.h"
#include "fraction.h"
#include "resid.h"

fractionn last_fraction;

BSDATA(fractioni) = {
	{"NoFraction", 3, MENSHPM},
	{"Atreides", 0, MENSHPA},
	{"Harkonens", 1, MENSHPH},
	{"Ordos", 2, MENSHPO},
};
assert_enum(fractioni, Ordos)