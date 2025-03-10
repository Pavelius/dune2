#pragma once

enum fixn : unsigned char;
enum movementn : unsigned char;
enum resid : unsigned short;
enum shapen	: unsigned char;

enum objectn : unsigned char {
	NoObject, Briefing, House, Building, Unit, Special, Objective,
	Advice, Orders,
	Atreides, Harkonens, Ordos,
	ConstructionYard, SpiceSilos, Starport, Windtrap, Refinery, RadarOutpost, RepairFacility, HouseOfIX, Palace,
	Barracks, Wor, LightVehicleFactory, HeavyVehicleFactory, HighTechFactory,
	Slab, Slab4, Turret, RocketTurret,
	Harvester, LightInfantry, HeavyTroopers, Trike, Quad, CombatTank, SiegeTank, RocketTank,
	SandWorm, Carryall, Fregate, Ornithopter,
	LastObject,
};

typedef int(*fngeto)(objectn type);

bool ismentat(objectn type);

int getarmor(objectn type);
int getattacks(objectn type);
int getcreditscost(objectn type);
int getdefaultcolor(objectn type);
int getenergycost(objectn type);
int getenergyprofit(objectn type);
int gethits(objectn type);
int geticonavatar(objectn type);
int getlos(objectn type);
int getrange(objectn type);
int getspeed(objectn type);
int getspicecap(objectn type);
// int getstat(objectn type, statn i);
int getspeedfp(int v);

shapen getshape(objectn type);

objectn getbuild(objectn type);
objectn getparent(objectn type);
objectn getrequired(objectn type);

resid getavatar(objectn type);
resid getres(objectn type);

movementn getmove(objectn type);

fixn getweapon(objectn type);
fixn getheavyweapon(objectn type);

unsigned short* getframes(objectn type);

const char* getnmo(objectn type);
const char* getido(objectn type);

struct objectable {
	objectn		type;
	explicit operator bool() const { return type != NoObject; }
	const char*	getname() const { return getnmo(type); }
};