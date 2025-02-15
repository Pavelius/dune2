#pragma once

#include "player.h"
#include "shape.h"
#include "topicable.h"
#include "typeable.h"

enum buildingn : unsigned char {
	ConstructionYard, SpiceSilo, Starport, Windtrap, Refinery, RadarOutpost, RepairFacility, HouseOfIX, Palace,
	Barracks, WOR, LightVehicleFactory, HeavyVehicleFactory, HighTechFacility,
	Turret, RocketTurret,
};
struct buildingi : topicable {
	unsigned char	frame_avatar;
	short			hits;
	shapen			shape;
	short unsigned	frames[16], ruined[16];
};
struct building : playerable, typeable<buildingi, buildingn> {
	point			position;
	short			hits;
	explicit operator bool() const;
	void			destroy();
};
extern building* last_building;

void add_building(point pt, buildingn id);
building* find_building(point v);