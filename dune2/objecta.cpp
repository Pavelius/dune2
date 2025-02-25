#include "bsdata.h"
#include "object.h"
#include "objecta.h"
#include "stringbuilder.h"

objecta subjects;

void objecta::addchild(objectn parent) {
	for(auto i = NoObject; i < LastObject; i = (objectn)(i+1)) {
		if(getparent(i) != parent)
			continue;
		add(i);
	}
}

static int getpriority(objectn type) {
	switch(type) {
	case Slab: case Slab4: return 5;
	case Turret: case RocketTurret: return 6;
	default: return 10;
	}
}

static int compare_object_priority(const void* v1, const void* v2) {
	auto o1 = *((objectn*)v1);
	auto o2 = *((objectn*)v2);
	auto p1 = getpriority(o1);
	auto p2 = getpriority(o2);
	if(p1 != p2)
		return p1 - p2;
	return szcmp(getnmo(o1), getnmo(o2));
}


void objecta::sort() {
	qsort(data, count, sizeof(data[0]), compare_object_priority);
}