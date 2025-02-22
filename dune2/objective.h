#pragma once

#include "nameable.h"
#include "order.h"
#include "player.h"
#include "typeable.h"

enum buildingn : unsigned char;
enum unitn : unsigned char;

enum objectiven : unsigned char {
	NoObjective,
	BuildStructure, BuildUnit,
	EarnCredits, FindSpiceField,
	EngageEnemyArmy, EngageEnemyHarvesters, EngageEnemyBase,
};
struct objectivei : nameable {
};
struct objective : typeable<objectivei, objectiven>, playerable {
	short unsigned		param, count;
	explicit operator bool() const { return type != NoObjective; }
};