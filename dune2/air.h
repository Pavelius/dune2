#pragma once

#include "moveable.h"
#include "object.h"

struct airunit : moveable, objectable {
	short unsigned		board;
};
