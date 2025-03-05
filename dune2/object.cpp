#include "bsdata.h"
#include "fix.h"
#include "resid.h"
#include "movement.h"
#include "shape.h"
#include "stat.h"
#include "stringbuilder.h"
#include "object.h"

struct objecti : nameable {
	objectn			parent;
	resid			mentat_avatar;
	short unsigned	icon_avatar;
	resid			res;
	short unsigned	frames[16];
};

BSDATA(objecti) = {
	{"NoObject"},
	{"Houses"},
	{"Buildings"},
	{"Units"},
	{"Special"},
	{"Objectives"},
	{"Atreides", House, FARTR, 0, MENSHPA},
	{"Harkonnen", House, FHARK, 1, MENSHPH},
	{"Ordos", House, FORDOS, 2, MENSHPO},
	{"ConstructionYard", Building, CONSTRUC, 60, ICONS, {292, 293, 295, 296}},
	{"SpiceSilos", Building, STORAGE, 69, ICONS, {372, 373, 375, 376}},
	{"Starport", Building, STARPORT, 63, ICONS, {309, 310, 311, 314, 315, 316, 319, 320, 321}},
	{"Windtrap", Building, WINDTRAP, 61, ICONS, {304, 305, 306, 307}},
	{"Refinery", Building, REFINERY, 64, ICONS, {332, 333, 334, 337, 338, 339}},
	{"RadarOutpost", Building, HEADQRTS, 70, ICONS, {379, 380, 386, 387}},
	{"RepairFacility", Building, REPAIR, 65, ICONS, {345, 346, 347, 349, 350, 351}},
	{"HouseOfIX", Building, IX, 58, ICONS, {280, 281, 282, 283}},
	{"Palace", Building, PALACE, 54, ICONS, {216, 217, 218, 226, 227, 228, 235, 236, 237}},
	{"Barracks", Building, BARRAC, 62, ICONS, {299, 300, 301, 302}},
	{"Wor", Building, WOR, 59, ICONS, {285, 286, 288, 289}},
	{"LightVehicleFactory", Building, LITEFTRY, 55, ICONS, {241, 242, 248, 249}},
	{"HeavyVehicleFactory", Building, HVYFTRY, 56, ICONS, {254, 255, 256, 261, 262, 263}},
	{"HighTechFactory", Building, HITCFTRY, 57, ICONS, {270, 271, 272, 276, 277, 278}},
	{"Slab", Building, SLAB, 53, ICONS, {126}},
	{"Slab4", Building, SLAB4, 71, ICONS},
	{"Turret", Building, TURRET, 67, ICONS, {356}},
	{"RocketTurret", Building, RTURRET, 68, ICONS, {364}},
	{"Harvester", Unit, HARVEST, 88, UNITS, {10}},
	{"LightInfantry", Unit, INFANTRY, 81, UNITS, {91}},
	{"HeavyInfantry", Unit, HYINFY, 91, UNITS, {103}},
	{"Trike", Unit, TRIKE, 80, UNITS, {5}},
	{"Quad", Unit, QUAD, 74, UNITS, {0}},
	{"CombatTank", Unit, LTANK, 78, UNITS2, {0, 5}},
	{"SiegeTank", Unit, HTANK, 72, UNITS2, {10, 15}},
	{"RocketTank", Unit, RTANK, 73, UNITS2, {0, 35}},
	{"SandWorm", Special, WORM, 93},
	{"Carryall", Unit, CARRYALL, 77, UNITS, {45}},
	{"Fregate", Unit, CARRYALL, 77, UNITS, {60}},
	{"Ornithopter", Unit, ORNI, 85, UNITS, {51, 3}},
	{"BuildStructure", Objective},
	{"BuildUnit", Objective, LITEFTRY},
	{"EarnCredits", Objective, REFINERY},
	{"FindSpiceField", Objective, TRIKE},
	{"ExploreArea", Objective, TRIKE},
	{"EngageEnemyArmy", Objective, INFANTRY},
	{"EngageEnemyHarvesters", Objective, HARVEST},
	{"EngageEnemyBase", Objective, WIN1},
};
assert_enum(objecti, EngageEnemyBase)

int getcreditscost(objectn type) {
	switch(type) {
		// Buildings
	case Barracks: return 300;
	case HeavyVehicleFactory: return 2000;
	case HighTechFactory: return 2500;
	case Starport: return 2000;
	case HouseOfIX: return 2000;
	case Palace: return 3000;
	case LightVehicleFactory: return 1000;
	case RadarOutpost: return 500;
	case Refinery: return 500;
	case RocketTurret: return 500;
	case Slab: return 2;
	case Slab4: return 5;
	case SpiceSilos: return 100;
	case Turret: return 300;
	case Wor: return 500;
	case Windtrap: return 150;
		// Units
	case LightInfantry: return 40;
	case HeavyInfantry: return 70;
	case Trike: return 150;
	case Quad: return 250;
	case CombatTank: return 350;
	case SiegeTank: return 600;
	case RocketTank: return 500;
	default: return 0;
	}
}

int getenergycost(objectn type) {
	switch(type) {
	case LightVehicleFactory: return 1500;
	case HeavyVehicleFactory: return 1500;
	case HighTechFactory: return 1000;
	case Starport: return 1500;
	case RocketTurret: return 500;
	case Turret: return 300;
	case Refinery: case ConstructionYard: case Windtrap: return 0;
	case Slab: case Slab4: return 0;
	default:
		if(type >= Harvester)
			return 0;
		return 500;
	}
}

int getenergyprofit(objectn type) {
	switch(type) {
	case Windtrap: return 1000;
	default: return 0;
	}
}

int getspicecap(objectn type) {
	switch(type) {
	case Refinery: return 1000;
	case SpiceSilos: return 1500;
	default: return 0;
	}
}

shapen getshape(objectn type) {
	switch(type) {
	case Barracks:
	case ConstructionYard:
	case LightVehicleFactory:
	case RadarOutpost:
	case SpiceSilos:
	case Windtrap:
	case Wor:
	case Slab4:
	case HouseOfIX:
		return Shape2x2;
	case Refinery:
	case HeavyVehicleFactory:
	case HighTechFactory:
	case RepairFacility:
		return Shape3x2;
	case Starport:
	case Palace:
		return Shape3x3;
	default:
		return Shape1x1;
	}
}

fixn getweapon(objectn type) {
	switch(type) {
	case LightInfantry: return ShootAssaultRifle;
	case HeavyInfantry: return ShootRotaryCannon;
	case Quad: return Shoot30mm;
	case Trike: return Shoot20mm;
	case RocketTank: return FireRocket;
	case SiegeTank: case CombatTank: return Shoot155mm;
	case Turret: return Shoot20mm;
	case RocketTurret: return FireRocket;
	default: return NoEffect;
	}
}

fixn getheavyweapon(objectn type) {
	switch(type) {
	case HeavyInfantry: return HandRocket;
	default: return NoEffect;
	}
}

int getstat(objectn type, statn i) {
	switch(i) {
	case Hits:
		switch(type) {
		case LightInfantry: case HeavyInfantry: return 4;
		case Trike: case RocketTank: return 5;
		case Quad: case CombatTank: case SiegeTank: return 6;
		case Harvester: return 8;
		case SandWorm: return 10;
		default: return 20;
		}
	case Armor:
		switch(type) {
		case LightInfantry: return 0;
		case HeavyInfantry: case Quad: case Trike: return 1;
		case CombatTank: case RocketTank: case Harvester: return 2;
		case SiegeTank: return 3;
		case SandWorm: return 4;
		default: return 1;
		}
	case Attacks:
		switch(type) {
		case Turret: return 1;
		case RocketTurret: return 2;
		case CombatTank: return 1;
		case LightInfantry: case HeavyInfantry: return 2;
		case SiegeTank: return 2;
		case RocketTank: return 2;
		case Trike: case Quad: return 2;
		default: return 0;
		}
	case LoS:
		switch(type) {
		case RadarOutpost: return 8;
		case Trike: return 3;
		case SandWorm: return 3;
		default: return 2;
		}
	case Range:
		switch(type) {
		case LightInfantry: case HeavyInfantry: return 2;
		case Trike: case Quad: return 2;
		case CombatTank: case SiegeTank: return 4;
		case Turret: return 5;
		case RocketTurret: return 6;
		case RocketTank: return 6;
		default: return 0;
		}
	case Speed:
		switch(type) {
		case Trike: return 10;
		case Quad: return 8;
		case RocketTank: case CombatTank: return 5;
		case SiegeTank: return 4;
		case HeavyInfantry: return 3;
		case LightInfantry: return 2;
		case SandWorm: return 6;
		case Carryall: return 12;
		case Ornithopter: return 20;
		case Fregate: return 6;
		default: return 4;
		}
	default:
		return 0;
	}
}

int getdefaultcolor(objectn type) {
	switch(type) {
	case Atreides: return 2;
	case Harkonens: return 1;
	case Ordos: return 3;
	default: return 0;
	}
}

objectn getparent(objectn type) {
	return bsdata<objecti>::elements[type].parent;
}

objectn getbuild(objectn type) {
	switch(type) {
	case LightInfantry: return Barracks;
	case HeavyInfantry: return Wor;
	case Trike: case Quad:
		return LightVehicleFactory;
	case Carryall: case Ornithopter:
		return HighTechFactory;
	case CombatTank: case RocketTank: case SiegeTank: case Harvester:
		return HeavyVehicleFactory;
	case SpiceSilos: case Starport: case Windtrap: case Refinery: case RadarOutpost:
	case RepairFacility: case HouseOfIX: case Palace: case Barracks: case Wor:
	case LightVehicleFactory: case HeavyVehicleFactory: case HighTechFactory:
	case Slab: case Slab4: case Turret: case RocketTurret:
		return ConstructionYard;
	default:
		return NoObject;
	}
}

objectn getrequired(objectn type) {
	switch(type) {
	case LightVehicleFactory: return RadarOutpost;
	case HeavyVehicleFactory: return LightVehicleFactory;
	case HighTechFactory: return HeavyVehicleFactory;
	case RepairFacility: return LightVehicleFactory;
	case Palace: return HouseOfIX;
	case RadarOutpost: return Refinery;
	case Turret: case RocketTurret: return RadarOutpost;
	case Starport: return HighTechFactory;
	case HouseOfIX: return Starport;
	case Wor: return Barracks;
	case SpiceSilos: return Refinery;
	default: return NoObject;
	}
}

const char* getnmo(objectn type) {
	return getnm(bsdata<objecti>::elements[type].id);
}

const char* getido(objectn type) {
	return bsdata<objecti>::elements[type].id;
}

resid getavatar(objectn type) {
	return bsdata<objecti>::elements[type].mentat_avatar;
}

resid getres(objectn type) {
	return bsdata<objecti>::elements[type].res;
}

int geticonavatar(objectn type) {
	return bsdata<objecti>::elements[type].icon_avatar;
}

unsigned short* getframes(objectn type) {
	return bsdata<objecti>::elements[type].frames;
}

movementn getmove(objectn type) {
	switch(type) {
	case LightInfantry: case HeavyInfantry:
		return Footed;
	case Quad: case Trike:
		return Wheeled;
	case CombatTank: case Harvester: case SiegeTank: case RocketTank:
		return Tracked;
	case SandWorm:
		return Undersand;
	default:
		return NoMovement;
	}
}

int getspeedfp(int n) {
	if(n <= 0)
		return 64 * 4;
	return 64 * 4 / n;
}