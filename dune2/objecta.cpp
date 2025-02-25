#include "bsdata.h"
#include "object.h"
#include "objecta.h"

objecta subjects;

void objecta::addchild(objectn parent) {
	for(auto i = NoObject; i < LastObject; i = (objectn)(i+1)) {
		if(getparent(i) != parent)
			continue;
		add(i);
	}
}