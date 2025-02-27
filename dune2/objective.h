#pragma once

#include "object.h"
#include "player.h"

struct objective : objectable, playerable {
	short unsigned	param, count;
	short unsigned	executor; // Unit task executor
	unsigned long	turn;
	short unsigned	parent;
	void			clear();
	void			cleanup();
	short unsigned	getindex() const;
	objective*		getparent() const;
	void			setparent(const objective* v);
	void			update();
private:
	void			done();
	void			thinking();
};

objective* add_objective(objectn type, unsigned char player, short unsigned param, short unsigned count = 0);