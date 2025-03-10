#pragma once

#include "nameable.h"

enum statn : unsigned char {
	Credits, Energy
};
struct stati : nameable {
};
struct storeable {
	unsigned int abilities[Energy + 1];
	unsigned int maximum[Energy + 1];
	void add(statn v, int i) { abilities[v] += i; }
	bool canafford(statn v, unsigned int i) const { return abilities[v] + i <= maximum[v]; }
	unsigned int get(statn v) const { return abilities[v]; }
	unsigned int getmaximum(statn v) const { return maximum[v]; }
};
