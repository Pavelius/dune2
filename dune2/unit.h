#pragma once

#include "moveable.h"
#include "fix.h"
#include "order.h"
#include "resid.h"
#include "object.h"
#include "stat.h"

enum squadn : unsigned char;
enum movementn : unsigned char;
struct unit : moveable, fixable, objectable {
	squadn		squad;
	explicit operator bool() const { return hits > 0; }
	void		clear();
	bool		closing() { return moveable::closing(getshootrange()); }
	void		damage(int value);
	void		destroy();
	bool		devour();
	void		fixstate(const char* id) const;
	short unsigned gethitsmax() const { return get(Hits) * 10; }
	short unsigned getindex() const;
	int			getlos() const { return get(LoS); }
	ordern		getpurpose() const;
	int			getshootrange() const { return get(Range); }
	int			getspeed() const;
	bool		isturret() const { return getframes(type)[1] != 0; }
	bool		isharvest() const;
	void		recovery();
	bool		relax();
	void		scouting();
	void		setposition(point v);
	void		setorder(point v);
	void		setorder(ordern type, point v);
	void		update();
	bool		usecrushing();
private:
	void		cantdothis();
	void		cleanup();
	bool		harvest();
	bool		istrallfull() const;
	bool		releasetile();
	bool		retreat();
	bool		shoot();
};
extern unit *last_unit;

void add_unit(point pt, objectn id, direction d, unsigned char player);
void add_unit_by_air(point pt, objectn id, direction d, unsigned char player);
bool isnonblocked(point v);
bool isfreetrack(point v);
bool isfreefoot(point v);
bool isspicefield(point v);
bool isunitpossible(point v);

unit* find_unit(point s);
unit* find_unit(point v, const unit* exclude);
unit* find_enemy(point v, unsigned char player, int range);
unit* find_enemy(point v, unsigned char player, int range, movementn move);
