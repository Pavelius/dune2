#pragma once

#include "player.h"
#include "slice.h"
#include "shape.h"
#include "topicable.h"
#include "typeable.h"

enum buildingn : unsigned char {
	ConstructionYard, SpiceSilo, Starport, Windtrap, Refinery, RadarOutpost, RepairFacility, HouseOfIX, Palace,
	Barracks, WOR, LightVehicleFactory, HeavyVehicleFactory, HighTechFacility,
	Turret, RocketTurret,
};
struct buildingi : topicable {
	short			hits;
	shapen			shape;
	short unsigned	frames[16], ruined[16];
	slice<buildingn> build;
};
struct building : playerable, typeable<buildingi, buildingn> {
	point			position;
	buildingn		build;
	unsigned short	build_spend;
	short			hits;
	explicit operator bool() const;
	bool			canbuild() const { return geti().build.operator bool(); }
	void			canbuildlist() const;
	void			cancel();
	void			construct(point v);
	void			destroy();
	int				getprogress() const;
	bool			isworking() const { return build_spend != 0; }
	bool			progress();
	void			update();
};
extern building* last_building;

void add_building(point pt, buildingn id);
building* find_building(point v);