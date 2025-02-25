#pragma once

#include "nameable.h"
#include "order.h"
#include "object.h"
#include "player.h"

enum objectn : unsigned char;
enum unitn : unsigned char;

enum objectiven : unsigned char {
	NoObjective,
	BuildStructure, BuildUnit,
	EarnCredits, FindSpiceField,
	EngageEnemyArmy, EngageEnemyHarvesters, EngageEnemyBase,
};
struct objectivei : nameable {
};
struct objective : objectable, playerable {
	short unsigned param, count;
	explicit operator bool() const { return type != NoObject; }
};