#pragma once

#include "fraction.h"
#include "nameable.h"

enum abilityn : unsigned char {
	Credits, EnergyCapacity, Energy, Supply, SupplyCapacity, SpiceCapacity
};
struct abilityi : nameable {
};
struct playeri : fractionable {
	unsigned char	color_index;
	unsigned int	abilities[SpiceCapacity + 1];
	unsigned short	buildings[32]; // Count of all buildings by type
	unsigned short	units[32]; // Count of all units by type
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