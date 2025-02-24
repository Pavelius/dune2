#pragma once

#include "moveable.h"
#include "fix.h"
#include "order.h"
#include "resid.h"
#include "topicable.h"
#include "stat.h"

enum unitn : unsigned char {
	Harvester, LightInfantry, HeavyInfantry, Trike, Quad, Tank, AssaultTank, RocketTank
};
enum squadn : unsigned char;
enum movementn : unsigned char;
struct uniti : topicable {
	movementn		move;
	fixn			weapon;
	resid			res;
	unsigned char	frame, frame_shoot;
};
struct unit : moveable, fixable, statable<uniti, unitn> {
	squadn			squad;
	void			clear();
	bool			closing() { return moveable::closing(getshootrange()); }
	bool			crushing();
	void			damage(int value);
	void			destroy();
	void			fixstate(const char* id) const;
	short unsigned	gethitsmax() const { return get(Hits) * 10; }
	short unsigned	getindex() const;
	int				getlos() const { return get(LoS); }
	ordern			getpurpose() const;
	int				getshootrange() const { return get(Range); }
	int				getspeed() const;
	bool			ismoveorder() const { return position != order; }
	bool			ismoving() const;
	bool			isturret() const { return geti().frame_shoot != 0; }
	bool			isharvest() const;
	void			recovery();
	bool			relax();
	void			scouting();
	void			setposition(point v);
	void			setorder(point v);
	void			setorder(ordern type, point v);
	void			update();
	bool			usecrushing();
private:
	void			cantdothis();
	void			cleanup();
	bool			harvest();
	bool			istrallfull() const;
	bool			releasetile();
	bool			returnbase();
	bool			shoot();
};
extern unit *last_unit;

void add_unit(point pt, unitn id, direction d);
bool isnonblocked(point v);
bool isfreetrack(point v);
bool isfreefoot(point v);
bool isspicefield(point v);
bool isunitpossible(point v);

unit* find_unit(point s);
unit* find_unit(point v, const unit* exclude);
unit* find_enemy(point v, unsigned char player, int range);
unit* find_enemy(point v, unsigned char player, int range, movementn move);
