#include "bsdata.h"
#include "stat.h"

BSDATA(stati) = {
	{"Hits"},
	{"Attacks"},
	{"Speed"},
	{"Armor"},
	{"Range"},
	{"LoS"},
};
assert_enum(stati, LoS)