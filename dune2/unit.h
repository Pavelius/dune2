#pragma once

#include "moveable.h"
#include "fix.h"
#include "order.h"
#include "resid.h"
#include "topicable.h"
#include "typeable.h"

enum unitn : unsigned char {
	Harvester, LightInfantry, HeavyInfantry, Trike, Quad, Tank, AssaultTank
};
enum statn : unsigned char {
	Hits, Attacks, Speed, Armor,
};
enum squadn : unsigned char;
enum movementn : unsigned char;
struct stati : nameable {
};
struct uniti : topicable {
	movementn		move;
	fixn			weapon;
	resid			res;
	unsigned char	frame, frame_shoot;
	unsigned char	stats[Armor + 1];
};
struct unit : moveable, fixable, typeable<uniti, unitn> {
	squadn			squad;
	unsigned char	effect;
	void			clear();
	bool			closing();
	void			damage(int value);
	void			destroy();
	void			fixstate(const char* id) const;
	int				get(statn v) const { return geti().stats[v]; }
	int				getmaximum(statn v) const;
	short unsigned	getindex() const;
	int				getlos() const { return 2; }
	const char*		getname() const { return geti().getname(); }
	ordern			getpurpose() const;
	int				getshootrange() const { return 3; }
	int				getspeed() const;
	bool			isboard() const { return position.x < 0; }
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
private:
	void			cantdothis();
	void			cleanup();
	bool			harvest();
	bool			istrallfull() const;
	bool			releasetile();
	bool			returnbase();
	bool			seeking();
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
point formation(int index);
