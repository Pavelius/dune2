#pragma once

#include "nameable.h"
#include "typeable.h"

enum statn : unsigned char {
	Hits, Attacks, Speed, Armor, Range,
	LoS,
};
struct stati : nameable {
};
template<typename T> int getstat(T i, statn v);
template<typename T, typename V>
struct statable : typeable<T, V> {
	int get(statn i) const { return getstat<V>(typeable<T, V>::type, i); }
};
