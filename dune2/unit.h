#pragma once

#include "actable.h"
#include "drawable.h"
#include "fix.h"
#include "flagable.h"
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
struct unit : drawable, actable, typeable<uniti, unitn> {
	point			order, guard;
	squadn			squad;
	direction		move_direction, path_direction;
	void			apply(ordern type, point v);
	void			clear();
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
	void			scouting();
	void			set(point v);
	void			stop();
	void			update();
private:
	void			blockland() const;
	void			cantdothis();
	void			cleanup();
	bool			harvest();
	bool			istrallfull() const;
	void			leavetrail();
	void			movescreen();
	direction		nextpath(point v);
	bool			releasetile();
	bool			returnbase();
	bool			seeking();
	bool			shoot() { return actable::shoot(screen, geti().weapon, geti().stats[Attacks], getshootrange()); }
	void			startmove();
	void			synchronize();
};
extern unit *last_unit;

void add_unit(point pt, unitn id, direction d);
void blockunits(const unit* exclude);
bool isnonblocked(point v);
bool isfreetrack(point v);
bool isfreefoot(point v);
bool isspicefield(point v);
bool isunitpossible(point v);

unit* find_unit(point s);
unit* find_unit(point v, const unit* exclude);
point formation(int index);
