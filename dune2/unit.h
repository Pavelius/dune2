#pragma once

#include "direction.h"
#include "drawable.h"
#include "fix.h"
#include "flagable.h"
#include "player.h"
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
struct unit : drawable, playerable, typeable<uniti, unitn> {
	unsigned long	ready_time; // Start action
	point			position, order, guard, order_attack;
	squadn			squad;
	direction		move_direction, shoot_direction, path_direction;
	unsigned char	attacks;
	short unsigned	target; // Enemy target
	short unsigned	hits;
	explicit operator bool() const { return hits > 0; }
	void			apply(ordern type, point v);
	void			clear();
	void			damage(int value);
	int				get(statn v) const { return geti().stats[v]; }
	unit*			getenemy() const;
	int				getmaximum(statn v) const;
	short unsigned	getindex() const;
	int				getlos() const { return 2; }
	const char*		getname() const { return geti().getname(); }
	ordern			getpurpose() const;
	int				getshootrange() const { return 3; }
	int				getspeed() const;
	bool			isattacking() const;
	bool			isboard() const { return position.x < 0; }
	bool			isenemy() const;
	bool			ismoveorder() const { return position != order; }
	bool			ismoving() const;
	bool			isnoweapon() const { return geti().stats[Attacks] == 0; }
	bool			isready() const;
	bool			isturret() const { return geti().frame_shoot != 0; }
	bool			isharvest() const;
	void			scouting();
	void			set(point v);
	void			stop();
	void			update();
	void			wait(unsigned long n);
private:
	void			blockland() const;
	bool			canshoot() const;
	void			cantdothis();
	void			cleanup();
	void			fixshoot(int chance_miss);
	bool			harvest();
	bool			istrallfull() const;
	void			leavetrail();
	void			movescreen();
	direction		nextpath(point v);
	bool			releasetile();
	bool			returnbase();
	void			tracking();
	bool			shoot();
	void			startmove();
	void			stopattack();
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
