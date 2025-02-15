#include "bsdata.h"
#include "building.h"
#include "fraction.h"
#include "topic.h"
#include "unit.h"

BSDATA(topici) = {
	{"Houses", bsdata<fractioni>::source, 1},
	{"Units", bsdata<uniti>::source, 0},
	{"Buildings", bsdata<buildingi>::source, 0},
};
BSDATAF(topici)

int	topici::getindex() const {
	return this - bsdata<topici>::elements;
}

topici* find_topic(void* subject) {
	for(auto& e : bsdata<topici>()) {
		if(e.source.have(subject))
			return &e;
	}
	return 0;
}