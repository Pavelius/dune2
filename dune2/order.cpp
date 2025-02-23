#include "bsdata.h"
#include "order.h"

BSDATA(orderi) = {
	{"Stop"},
	{"Move"},
	{"Seek"},
	{"Attack"},
	{"Retreat"},
	{"Harvest"},
};
assert_enum(orderi, Harvest)
