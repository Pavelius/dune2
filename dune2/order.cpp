#include "bsdata.h"
#include "order.h"

BSDATA(orderi) = {
	{"Stop"},
	{"Move"},
	{"Attack"},
	{"Retreat"},
	{"Harvest"},
	{"SmartMove"},
};
assert_enum(orderi, SmartMove)
