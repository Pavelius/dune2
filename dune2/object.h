#pragma once

enum fixn : unsigned char;
enum movementn : unsigned char;
enum resid : unsigned short;
enum shapen	: unsigned char;
enum statn : unsigned char;

enum objectn : unsigned char {
	NoObject, House, Building, Unit, Special, Objective,
	Atreides, Harkonens, Ordos,
	ConstructionYard, SpiceSilo, Starport, Windtrap, Refinery, RadarOutpost, RepairFacility, HouseOfIX, Palace,
	Barracks, Wor, LightVehicleFactory, HeavyVehicleFactory, HighTechFacility,
	Slab, Slab4, Turret, RocketTurret,
	Harvester, LightInfantry, HeavyInfantry, Trike, Quad, Tank, AssaultTank, RocketTank,
	SandWorm, Carrier, Fregate, Ornitopter,
	BuildStructure, BuildUnit,
	EarnCredits, FindSpiceField, ExploreArea,
	EngageEnemyArmy, EngageEnemyHarvesters, EngageEnemyBase,
	LastObject,
};

int getcreditscost(objectn type);
int getdefaultcolor(objectn type);
int getenergycost(objectn type);
int getenergyprofit(objectn type);
int geticonavatar(objectn type);
int getspicecap(objectn type);
int getstat(objectn type, statn i);

shapen getshape(objectn type);

objectn getbuild(objectn type);
objectn getparent(objectn type);
objectn getrequired(objectn type);

resid getavatar(objectn type);
resid getres(objectn type);

movementn getmove(objectn type);

fixn getweapon(objectn type);

unsigned short* getframes(objectn type);

const char* getnmo(objectn type);
const char* getido(objectn type);

struct objectable {
	objectn		type;
	explicit operator bool() const { return type != NoObject; }
	const char*	getname() const { return getnmo(type); }
	int			get(statn i) const { return getstat(type, i); }
};