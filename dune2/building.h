#pragma once

#include "actable.h"
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
enum buildstaten : unsigned char {
	BoardUnit,
};
struct tilepatch;
struct buildingi : topicable {
	shapen			shape;
	short unsigned	frames[16];
	slice<topicable*> build;
	unsigned		cost[Supply + 1], surplus[Supply + 1];
	slice<tilepatch> tiles;
	buildingn		getindex() const;
};
struct building : actable, typeable<buildingi, buildingn> {
	unsigned char	build_index, build_count;
	unsigned short	build_spend;
	unsigned short	unit_board;
	void			board(unit* p);
	void			block() const { setblock(0xFFFF); }
	void			buildlist() const;
	bool			canbuild() const { return geti().build.operator bool(); }
	void			cancel();
	void			cleanup();
	void			clear();
	void			construct(point v);
	void			damage(int value);
	void			destroy();
	int				getlos() const;
	int				getprogress() const;
	topicable*		getbuild() const;
	point			getbuildsize() const;
	int				gethitsmax() const;
	rect			getrect() const;
	point			getsize() const;
	bool			isnear(point v) const;
	bool			isworking() const { return build_spend != 0; }
	point			nearestboard(point v, movementn move) const;
	bool			progress();
	void			scouting();
	void			set(buildstaten action, bool apply);
	void			update();
	void			unblock() const { setblock(0); }
	void			unboard();
private:
	bool			autoproduct();
	void			patchdirection();
	void			setblock(short unsigned n) const;
};
extern building* last_building;

void add_building(point pt, buildingn id);
building* find_base(buildingn type, unsigned char player);
building* find_building(point v);
building* find_board(const unit* p);