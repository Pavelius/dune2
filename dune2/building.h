#pragma once

#include "nameable.h"
#include "player.h"
#include "shape.h"
#include "typeable.h"

enum buildingn : unsigned char {
	ConstructionYard, SpiceSilo, Starport, Windtrap, Refinery, RadarOutpost, RepairFacility, HouseOfIX, Palace,
	Barracks, WOR, LightVehicleFactory, HeavyVehicleFactory, HighTechFacility,
	Turret, RocketTurret,
};
struct buildingi : nameable {
	shapen			shape;
	short unsigned	frames[16], ruined[16];
};
struct building : playerable, typeable<buildingi, buildingn> {
	point			position;
	explicit operator bool() const;
};
extern building* last_building;

void addobj(point pt, buildingn id);
building* find_building(point v);