#pragma once

#include "player.h"
#include "rect.h"
#include "slice.h"
#include "shape.h"
#include "topicable.h"
#include "typeable.h"
#include "unit.h"

enum buildingn : unsigned char {
	ConstructionYard, SpiceSilo, Starport, Windtrap, Refinery, RadarOutpost, RepairFacility, HouseOfIX, Palace,
	Barracks, Wor, LightVehicleFactory, HeavyVehicleFactory, HighTechFacility,
	Slab, Slab4, Turret, RocketTurret,
};
struct buildingi : topicable {
	short			hits;
	shapen			shape;
	short unsigned	frames[16], ruined[16];
	slice<topicable*> build;
	short unsigned	abilities[SpiceCapacity + 1];
	buildingn		getindex() const;
};
struct building : playerable, typeable<buildingi, buildingn> {
	point			position;
	unsigned char	build_index, build_count;
	unsigned short	build_spend;
	short			hits;
	explicit operator bool() const;
	void			buildlist() const;
	bool			canbuild() const { return geti().build.operator bool(); }
	void			cancel();
	void			construct(point v);
	void			destroy();
	int				getlos() const;
	int				getprogress() const;
	topicable*		getbuild() const;
	point			getbuildsize() const;
	rect			getrect() const;
	point			getsize() const;
	bool			isworking() const { return build_spend != 0; }
	bool			progress();
	void			scouting();
	void			update();
private:
	bool			autoproduct();
};
extern building* last_building;

void add_building(point pt, buildingn id);
building* find_building(point v);