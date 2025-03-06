#pragma once

#include "adat.h"

enum objectn : unsigned char;

struct objecta : adat<objectn, 256> {
	typedef bool(*fnallow)(objectn v);
	void addchild(objectn parent);
	void addchild(objectn parent, fnallow proc);
	void sort();
};
extern objecta subjects;
