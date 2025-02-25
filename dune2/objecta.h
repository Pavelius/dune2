#pragma once

#include "adat.h"

enum objectn : unsigned char;

struct objecta : adat<objectn, 256> {
	void addchild(objectn parent);
};
extern objecta subjects;
