#pragma once

#include "nameable.h"
#include "order.h"
#include "typeable.h"

enum buildingn : unsigned char;
enum unitn : unsigned char;

enum objectiven : unsigned char {
	BuildStructure, BuildUnit,
	EarnCredits, FindSpiceField,
	EngageEnemyArmy, EngageEnemyHarvesters, EngageEnemyBase,
};
struct objectivei : nameable {
};
struct objective : typeable<objectivei, objectiven> {
	short unsigned		param, count;
};