#pragma once

#include "nameable.h"
#include "shape.h"

struct buildingi : nameable {
	shapen			shape;
	short unsigned	frames[16], ruined[16];
};