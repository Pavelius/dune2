#pragma once

#include "nameable.h"
#include "slice.h"

enum resid : unsigned short;

struct topicable : nameable {
	resid			mentat_avatar;
	short unsigned	frame_avatar;
	short			cost;
};