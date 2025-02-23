#pragma once

#include "nameable.h"

enum abilityn : unsigned char {
	Credits, Energy, Supply
};
struct abilityi : nameable {
};
struct abilityable {
	unsigned int abilities[Supply + 1];
	unsigned int maximum[Supply + 1];
	void			add(abilityn v, int i) { abilities[v] += i; }
	bool			canafford(abilityn v, unsigned int i) const { i += abilities[v]; return i <= getmaximum(v); }
	unsigned int	get(abilityn v) const { return abilities[v]; }
	unsigned int	getmaximum(abilityn v) const { return maximum[v]; }
};
