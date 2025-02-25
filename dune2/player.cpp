#include "bsdata.h"
#include "building.h"
#include "player.h"

BSDATAC(playeri, player_maximum)

unsigned char player_index;

playeri& mainplayer() {
	return bsdata<playeri>::elements[player_index];
}

void playeri::clear() {
	memset(this, 0, sizeof(*this));
}

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
	for(auto i = Credits; i <= Supply; i = (statn)(i + 1))
		v1[i] += v2[i] * multiplier;
}

void playeri::update() {
	abilities[Energy] = 0;
	memset(maximum, 0, sizeof(maximum));
	for(auto i = ConstructionYard; i <= RocketTank; i = (objectn)(i+1)) {
		abilities[Energy] += getenergycost(i) * objects[i];
		maximum[Energy] += getenergyprofit(i) * objects[i];
		maximum[Credits] += getspicecap(i) * objects[i];
	}
}