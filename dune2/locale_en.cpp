#include "game.h"
#include "object.h"
#include "player.h"
#include "terrain.h"

const char* object_names[LastObject + 1] = {
	"No object",
	"Briefing", "House", "Building", "Unit", "Special", "Objective",
	"Advice", "Orders",
	"Atreides", "Harkonens", "Ordos",
	"Base", "SpiceSilos", "Starport", "Wind Trap", "Refinery", "Radar Outpost", "Repair Facility", "House of IX", "Palace",
	"Barracks", "Wor", "Light Vehicle Factory", "Heavy Vehicle Factory", "High-tech Factory",
	"Concrete Slab", "Concrete Slab Blocks", "Turret", "Rocket Turret",
	"MCV", "Harvester", "Infantry", "Troopers", "Trike", "Quad", "Combat Tank", "Siege Tank", "Rocket Tank",
	"Shai-Hulud", "Carrier", "Fregate", "Ornithopter",
};
const char* message_names[LastMessage + 1] = {
	"Dune II: Building Dinasty", "Cancel", "Yes", "No",
	"Target", "Placement", "Select Subject", "Select your house",
	"Load game", "Save this game", "Quit game", "Resume game", "Exit", "Proceed", "Repeat", "Build It",
	"Game controls", "Restart scenario", "Pick another house", "Cost",
	"Done %1i%%", "%Unit is destroyed.", "%Unit is devoured by worm.",
	"There is no room for spice - build more Spice Silos.",
	"%Unit is %UnitThrall percent full.",
	"%Unit can't execute order.",
	"%Unit is unloading spice.",
	"%Unit is return with %UnitThrall percent full.",
	"Choose move target",
	"Place buidling on rock",
	"Do you really want to load game?",
	"Do you really want to end this game and pick another house?",
	"Do you really want to quit this game? All unsaved changes will be lost.",
	"Not enought energy. Build more wind traps.",
};
const char* stat_names[LastStat + 1] = {
	"Spice", "Energy",
};
const char* order_names[LastOrder + 1] = {
	"Stop", "Move", "Move", "Attack", "Retreat", "Harvest",
};
const char* terrain_names[LastTerrain + 1] = {
	"Sand", "Dune", "Spice", "Spice", "Spice Blow", "Rock", "Mountain",
};