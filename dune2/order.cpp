#include "bsdata.h"
#include "order.h"

BSDATA(orderi) = {
	{"Stop"},
	{"Move"},
	{"Attack"},
	{"Retreat"},
	{"Harvest"},
};
assert_enum(orderi, Harvest)
