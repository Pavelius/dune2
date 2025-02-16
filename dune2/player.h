#pragma once

#include "fraction.h"
#include "nameable.h"

enum abilityn : unsigned char {
	Credits, Energy, SpiceCapacity, Rounds,
};
struct abilityi : nameable {
};
struct playeri : fractionable {
	unsigned char	color_index;
	unsigned int	abilities[Rounds + 1];
	void			add(abilityn v, int i) { abilities[v] += i; }
	unsigned int	get(abilityn v) const { return abilities[v]; }
	unsigned char	getindex() const;
};
struct playerable {
	unsigned char	player;
	playeri&		getplayer() const;
	void			setplayer(const playeri* v);
};
extern playeri* player;