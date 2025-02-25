#pragma once

#include "stat.h"

struct storeable {
	unsigned int abilities[Supply + 1];
	unsigned int maximum[Supply + 1];
	void add(statn v, int i) { abilities[v] += i; }
	bool canafford(statn v, unsigned int i) const { i += abilities[v]; return i <= getmaximum(v); }
	unsigned int get(statn v) const { return abilities[v]; }
	unsigned int getmaximum(statn v) const { return maximum[v]; }
};
