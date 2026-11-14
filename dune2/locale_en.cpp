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
const char* object_a_info[LastObject + 1] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Our ancestral home is the lush planet of Caladan.\nWe have created a civilized society and we respect the lives and rights of all people.\nWe do not believe in the use of unnecessary violence, and have crafted a defensive alliance with the native Fremen of Dune.",
	"The Harkonnen come from the dark planet of Giedi Prime.\nThey are cruel and vicious individuals, and do not assign much value to fair play or even human life.\nTheir goal is obviously the military conquest of Dune.",
	"House Ordos was originally formed by a group of insecure wealthy families.\nIt has evolved into a powerful cartel that is driven by a lust for credits and the desire to control and manipulate.\nThey often support themselves by stealing rather than working.",
	"A Construction Yard is required to build any buildings.",
	"The Silos are used to store harvested spice.\nThey are not as strong as some of our other structures, and frequent repairs are advised.",
	"A Starport will provide us with the ability to trade and deal through intergalactic trading channels.\nThis will allow us to enhance our fleets and structures.",
	"The Windtraps are an elegant and efficient means of harnessing the wind's awesome power and extracting precious moisture.\nAlthough they are fairly sturdy, you should make sure that you have backup wind traps in case of accidents or sabotage.",
	"The refinery converts spice into credits.\nWithout the refinery you cannot produce new credits by harvesting spice.",
	"The Outpost should be utilized for its radar capability.\nThe radar will allow you to keep a watchful eye on your harvesters and any enemies in the area.",
	"The Repair facility is used for the repairs of all harvesting and defense vehicles.",
	"IX is the technology center of the world.\nIt is the ultimate source of new, wonderful technology that will help us reach our goals.",
	"Receiving a Palace is an honor, bestowed only upon those who have displayed bravery and diligence in the completion of their missions.\nA Palace is the most finely crafted and well-armored structure you may possess.",
	"The Barracks is the building that trains our courageous light infantry.\nThe structures are quite durable and will provide you with ample means to defend your spice harvesting operations.",
	"WOR is capable of training heavy troopers.\nUnlike the other houses, we do not believe in unnecessary violence to achieve our goals, and therefore  I do not believe that we will require the use of such technology.",
	"The Light Factory produces lightly armored three and four wheeled vehicles, which are used for the defense of our refineries and silos.",
	"The Heavy Factory produces all large vehicles, like the harvester and the heavier machines used for defensive purposes.\nThe building itself is extremely well armored.",
	"The High-Tech Factory produces high technology aircraft like the carryall.",
	"Concrete slabs provide the stable foundations for our buildings that allow them to operate at maximum efficiency.",
	"Concrete slabs provide the stable foundations for our buildings that allow them to operate at maximum efficiency.",
	"Turrets are stationary base defenses that are exceptionally accurate at close range.",
	"The Rocket turret is an improvement on the regular turret.\nLong range rockets have been added to provide for a better perimeter defense system.",
	"The Mobile Construction Vehicle can be employed to erect new structures that need not be connected to the other buildings in your area.\nAdditional refineries in outlying areas, for example, would allow you to harvest more effectively.",
	"Harvesters are large and well armored, and can be produced by a heavy factory.\nVital to your spice harvesting missions, they are the only means of collecting and transporting spice from the field to the refinery.",
	"The light infantry is satisfactorily protected and adequately armed, and is capable of defending our installations.",
	"The Heavy troopers are armed with powerful weapons and strong armor.",
	"The Trike is a three wheeled vehicle produced by the light factory.\nIt is fast, well armored, and carries light weapons for defense.\nTrikes may be utilized for protection against enemy raids and scouting for spice fields.",
	"Quads are four wheeled vehicles with strong armor and powerful weapons.\nLike the Trike, they are an efficient defense system.\nWith quads and trikes, you should be able to defend against most attacks.",
	"The Combat Tank is adequately armed and good for defense.\nIt can be used for perimeter defense and protection of the harvesters.",
	"The Siege Tank is heavily armored and has very powerful weapons.\nIt is more than adequate to defend your harvesters and installations.",
	"The Rocket Tank has good armor, is faster than normal tanks, and has long range strike capabilities.\nIt is excellent for defensive support of the light infantry and your squadrons of trikes and quads.",
	"These enormous creatures are attracted by vibrations on the surface and will often attack and devour both personnel and vehicles.\nRock is the only sure defense against these creatures.",
	"The Carryall is capable of transporting the harvester between spice fields and the refinery.\nIt has no offensive capabilities or weapons, and only light armor.",
	"The Carryall is capable of transporting the harvester between spice fields and the refinery.\nIt has no offensive capabilities or weapons, and only light armor.",
	"The Ornithopter, a lightly armored, highly maneuverable aircraft, is excellent for defensive air support of your harvesters, trikes, and quads.",
};
const char* object_h_info[LastObject + 1] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	//Atreides, Harkonens, Ordos,
	//ConstructionYard, SpiceSilos, Starport, Windtrap, Refinery, RadarOutpost, RepairFacility, HouseOfIX, Palace,
	//Barracks, Wor, LightVehicleFactory, HeavyVehicleFactory, HighTechFactory,
	//Slab, Slab4, Turret, RocketTurret,
	//MCVehicle, Harvester, LightInfantry, HeavyTroopers, Trike, Quad, CombatTank, SiegeTank, RocketTank,
	//SandWorm, Carryall, Fregate, Ornithopter,
};
const char* object_o_info[LastObject + 1] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	//Atreides, Harkonens, Ordos,
	//ConstructionYard, SpiceSilos, Starport, Windtrap, Refinery, RadarOutpost, RepairFacility, HouseOfIX, Palace,
	//Barracks, Wor, LightVehicleFactory, HeavyVehicleFactory, HighTechFactory,
	//Slab, Slab4, Turret, RocketTurret,
	//MCVehicle, Harvester, LightInfantry, HeavyTroopers, Trike, Quad, CombatTank, SiegeTank, RocketTank,
	//SandWorm, Carryall, Fregate, Ornithopter,
};
const char* object_capacity[LastObject + 1] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Home Planet: Caladan", "Home Planet: Giedi Prime", "Home Planet: Unknown",
	"Type: Base Deployment Vehicle\nArmament: none\nSpeed: 36 km/h (tracked)\nArmor: Medium\nCrew: 15",
	"Type: Storehouse\nArmor: Light\nCapacity: 1000 spice units",
	"", // Starport
	"Type: Power Plant\nArmor: Light\nGenerators: ESkort 650 hp EL-2A DT",
	"Type: Industrial Facility\nArmor: Medium\nCapacity: 1000 spice units",
	"", // RadarOutpost
	"", // RepairFacility
	"", // HouseOfIX
	"", // Palace
	"Type: Production Facility\nArmor: Medium", // Barracks
	"", // Wor
	"", // LightVehicleFactory
	"", // HeavyVehicleFactory
	"", // HighTechFactory
	"Type: Foundation\nMaterial: Gravel/Sand base mix\nWeight: 1,469 kg\nArmor: Medium", // Slab
	"Type: Foundation\nMaterial: Gravel/Sand base mix\nWeight: 1,469 kg\nArmor: Medium", // Slab4
	"", // Turret
	"", // RocketTurret
	"Type: Base Deployment Vehicle\nArmament: none\nSpeed: 36 km/h (tracked)\nArmor: Medium\nCrew: 15", // MCVehicle
	"Type: Spice Recovery Vehicle\nArmament: none\nSpeed: 32 km/h\nArmor: Medium\nCrew: 4", // Harvester
	"Type: Ground Based Military Units\nArmament: 9mm RP Assault Rifles\nSpeed: 5 km/h (foot)\nArmor: Light\nCrew: 1 to 3", // LightInfantry
	"Type: Ground Based Military Units\nArmament: 10mm Rotary Cannons\nHeavy weapon: FS Rockets\nSpeed: 12 km/h (foot)\nArmor: Light\nCrew: 1 to 3", // HeavyTroopers
	"Type: Light Recon/Strike Vehicle\nArmament: Dual 20mm cannons\nSpeed: 72km/h (wheel)\nArmor: Light\nCrew: 2", // Trike
	"Type: Light Attack Vehicle\nArmament: Dual 30mm cannons\nSpeed: 59km/h (wheel)\nArmor: Light\nCrew: 2", // Quad
	"Type: Medium Battle Tank\nArmament: 155mm gun\nSpeed: 40 km/h (tracked)\nArmor: Medium\nCrew: 2", // CombatTank
	"Type: Main Battle Tank\nArmament: Daul 155mm cannons\nSpeed: 32 km/h (tracked)\nArmor: Heavy\nCrew: 3", // SiegeTank
	"", // RocketTank
	"No Known Weakness", // SandWorm
	"Type: Airborne Unit\nArmament: none\nSpeed: 160 km/h (airborne)\nArmor: Light\nCrew: 4", // Carryall
	"Type: Airborne Unit\nArmament: none\nSpeed: 160 km/h (airborne)\nArmor: Light\nCrew: 4", // Fregate
	"Type: Close Support Attack Aircraft\nArmament: Battle Support Rockets\nSpeed: 340 km/h (airborne)\nArmor: Light\nCrew: 2", // Ornithopter
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