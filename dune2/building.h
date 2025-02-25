#pragma once

#include "actable.h"
#include "rect.h"
#include "slice.h"
#include "shape.h"
#include "unit.h"

enum fixn : unsigned char;

enum buildstaten : unsigned char {
	BoardUnit,
};

struct building : actable, objectable {
	objectn			build;
	unsigned char	build_count;
	unsigned short	build_spend;
	unsigned short	unit_board;
	void			board(unit* p);
	void			block() const { setblock(0xFFFF); }
	void			buildlist() const;
	bool			canbuild() const;
	void			cancel();
	void			cleanup();
	void			clear();
	void			construct(point v);
	void			damage(int value);
	void			destroy();
	int				getlos() const { return get(LoS); }
	int				getprogress() const;
	point			getbuildsize() const;
	int				gethitsmax() const;
	rect			getrect() const;
	point			getsize() const;
	fixn			getweapon() const;
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
	void			setblock(short unsigned n) const;
	void			updateturrets();
};
extern building* last_building;

void add_building(point pt, objectn id);
building* find_base(objectn type, unsigned char player);
building* find_building(point v);
building* find_board(const unit* p);

bool isbuildplace(point v);
bool isbuildslabplace(point v);
void markbuildarea(point base, point placement_size, objectn build);