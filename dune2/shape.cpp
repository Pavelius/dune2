#include "bsdata.h"
#include "shape.h"

BSDATA(shapei) = {
	{"Shape1x1", 1, {1, 1}, {{0, 0}}},
	{"Shape2x2", 4, {2, 2}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}},
	{"Shape3x2", 6, {3, 2}, {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}}},
};
assert_enum(shapei, Shape3x2)