#pragma once

#include "nameable.h"
#include "typeable.h"

enum statn : unsigned char {
	Hits, Attacks, Speed, Armor, Range,
	LoS,
};
struct stati : nameable {
};
template<typename T, typename V>
struct statable : typeable<T, V> {
	int get(statn i) const;
	int getmaximum(statn i) const;
};
