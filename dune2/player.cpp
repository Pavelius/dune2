#include "bsdata.h"
#include "building.h"
#include "player.h"

BSDATAC(playeri, player_maximum)

unsigned char player_index, player_human;

playeri& mainplayer() {
	return bsdata<playeri>::elements[player_index];
}

void playeri::clear() {
	memset(this, 0, sizeof(*this));
	spice = {-10000, -10000};
	scout = 0xFFFF;
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

void playeri::update() {
	abilities[Energy] = 0;
	memset(maximum, 0, sizeof(maximum));
	for(auto i = ConstructionYard; i <= RocketTank; i = (objectn)(i+1)) {
		abilities[Energy] += getenergycost(i) * objects[i];
		maximum[Energy] += getenergyprofit(i) * objects[i];
		maximum[Credits] += getspicecap(i) * objects[i];
	}
}

bool playeri::cansupportenergy(objectn type) const {
	auto cost_energy = getenergycost(type);
	if(!cost_energy)
		return true;
	return abilities[Energy] + cost_energy <= maximum[Energy];
}