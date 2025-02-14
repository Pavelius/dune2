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
struct playerable {
	unsigned char	player;
	playeri*		getplayer() const;
	void			setplayer(const playeri* v);
};
extern playeri* player;