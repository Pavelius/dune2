#include "bsdata.h"
#include "building.h"
#include "player.h"

BSDATA(abilityi) = {
	{"Credits"},
	{"Energy"},
	{"Supply"},
};
BSDATAF(abilityi)

BSDATAC(playeri, player_maximum)

playeri* player;

unsigned char playeri::getindex() const {
	if(!this)
		return 0xFF;
	return this - bsdata<playeri>::elements;
}

playeri& playerable::getplayer() const {
	return bsdata<playeri>::elements[player];
}

void playerable::setplayer(const playeri* v) {
	if(v)
		player = v - bsdata<playeri>::elements;
}

static void add_abilities(unsigned int* v1, const unsigned int* v2, int multiplier) {
	for(auto i = Credits; i <= Supply; i = (abilityn)(i + 1))
		v1[i] += v2[i] * multiplier;
}

void playeri::update() {
	abilities[Energy] = 0;
	memset(maximum, 0, sizeof(maximum));
	for(size_t i = 0; i < sizeof(buildings) / sizeof(buildings[0]); i++) {
		add_abilities(abilities, bsdata<buildingi>::elements[i].cost, buildings[i]);
		add_abilities(maximum, bsdata<buildingi>::elements[i].surplus, buildings[i]);
	}
}