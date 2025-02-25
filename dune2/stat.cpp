#include "bsdata.h"
#include "stat.h"

BSDATA(stati) = {
	{"Credits"},
	{"Energy"},
	{"Supply"},
	{"Hits"},
	{"Attacks"},
	{"Speed"},
	{"Armor"},
	{"Range"},
	{"LoS"},
};
assert_enum(stati, LoS)