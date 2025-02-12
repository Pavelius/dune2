#pragma once

#include "nameable.h"

enum abilityn : unsigned char {
	Credits, Rounds,
};
struct abilityi : nameable {
};
struct playeri {
	unsigned		abilities[Rounds + 1];
	unsigned short	active_unit, active_platoon;
	unsigned char	getindex() const;
};
extern playeri* player;