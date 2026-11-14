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
	"Raider", "Nuclear Rocket", "Devastator", "Fremens", "Deviator", "Saboteur", "Sonic Tank",
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
	"", // Raider
	"", // Death Hand
	"", // Devastator
	"", // Fremens
	"", // Deviator
	"", // Saboteur
	"", // SonicTank
	"These enormous creatures are attracted by vibrations on the surface and will often attack and devour both personnel and vehicles.\nRock is the only sure defense against these creatures.",
	"The Carryall is capable of transporting the harvester between spice fields and the refinery.\nIt has no offensive capabilities or weapons, and only light armor.",
	"The Carryall is capable of transporting the harvester between spice fields and the refinery.\nIt has no offensive capabilities or weapons, and only light armor.",
	"The Ornithopter, a lightly armored, highly maneuverable aircraft, is excellent for defensive air support of your harvesters, trikes, and quads.",
};
const char* object_h_info[LastObject + 1] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	"The Atreides are spawned from the humid tropical planet of Caladan.\nThey are best known for their weaknesses of honesty and kindness.\nThey do not seem to believe in war and will usually avoid military conflicts.\nThey have befriended the natives of Dune, and may be expected to involve the local patriot Fremen in their defensive plans.", //Atreides
	"We Harkonnen have come from the planet Giedi Prime.\nWe want now to possess Dune.Our weapons are powerful enough to deflect any attacks from our enemies.\nAll of House Harkonnen is concentrating on this task, and only a disaster could stop us now.", //Harkonens
	"This cartel of greedy families has been thrown together by the lust for power.\nThey will most likely be too busy bickering among themselves to be overly concerned with our forces.", //Ordos
	"You must have a construction yard.\nIt will give you the means to construct other buildings.", // ConstructionYard
	"Spice silos must be built to be able to store the harvested spice.\nUnfortunately, they are weak structures, and must be repaired frequently.", // SpiceSilos
	"The Starport is your link to the storehouses beyond the surface of the planet.\nWith a Starport at your disposal, you can purchase powerful weapons and vehicles.", // Starport
	"These are the structures that provide power and water to your installation.\nThey are a bit fragile, and you are well advised to construct more than one.", // Windtrap
	"The refinery converts harvested spice into credits.", // Refinery
	"The outpost houses radar equipment.\nThe radar will help you to keep an eye on your own domain as well as nearby enemy activity.", // RadarOutpost
	"It is impossible to operate machinery in this desert without causing damage.\nYou must have a repair facility to take care of your damaged vehicles.", // RepairFacility
	"IX will provide you with a source of militarily advanced weapons.", // HouseOfIX
	"House Harkonnen may grant a Palace to those who have proved themselves worthy of such an honor.\nIf you lose a Palace, that was given to you, you will have failed your mission.", // Palace
	"These simple little buildings train infantry units.\nThe building itself has fair armor and is relatively cheap to maintain.\nWe Harkonnen, however, do not have use for such weak forces.", // Barracks
	"WOR will train the Heavy Troopers.", // Wor
	"The light factory produces only lightly armored vehicles, and is only a beginning step towards your goal of military superiority.", // LightVehicleFactory
	"You must have a Heavy factory in order to construct our heavy tanks.", // HeavyVehicleFactory
	"The High-Tech factory produces our carryalls.", // HighTechFactory
	"Slabs are used to provide stable building foundations.\nWe have found that anything built on bare rock will deteriorate rapidly.", // Slab
	"Slabs are used to provide stable building foundations.\nWe have found that anything built on bare rock will deteriorate rapidly.", // Slab4
	"Turrets are accurate and effective weapons.\nAlthough they are immobile, you should consider them as required enhancements to your forces.", // Turret
	"The Rocket Turret is an excellent weapon.\nAlthough they cannot be moved, they can be utilized for long and short range defense.", // RocketTurret
	"The Mobile Construction Vehicle will allow you to expand your territories.\nBy using an MCV you will be able to construct distant buildings that need not be adjacent to your principal location.", // MCVehicle
	"This large, slow, and barely maneuverable vehicle is used to harvest raw spice and convey it to the refinery.\nAlthough unarmed, the iron clad harvesters can be used effectively against small groups of infantry by crushing them beneath their treads.", // Harvester
	"The Light Infantry will hardly suit your needs.\nThey are slow, poorly armored, and have a tendency to miss their targets.\nYou should depend upon the more massive troopers.", // LightInfantry
	"The Heavy Troopers are superior fighting units.\nThey are well armored and have powerful weapons.", // HeavyTroopers
	"Although these three-wheeled, lightly armored vehicles are functional, we strongly suggest utilizing the larger, more powerful vehicles.", // Trike
	"The Quad is a much better vehicle than the smaller trike.\nIt has far superior armor and is much more powerful.", // Quad
	"The combat tank will be your first tank, and it has adequate armor and fire power.\nHowever, remember that there are more powerful weapons at your disposal.", // CombatTank
	"The Siege Tank is superior to the Combat tank.\nIt's armor and weaponry has the power you will need to complete your conquest of this planet.", // SiegeTank
	"The rocket tank is effective for long range attacks.\nGuard these well, for they are ineffective at close range.", // RocketTank
	"This is a pitiful little machine.\nFor some reason the Ordos believe that this overblown Trike can out maneuver and out fire our Quads.", // Raider
	"Our Death Hand is a lovely weapon capable of being targeted for long range strikes.\nJust prior to impact it releases a series of explosive fingers that spread destruction over a wide area.", // Death Hand
	"The Devastator is another of the unique and powerful weapons in our awesome arsenal.\nIt's armor and weapons are unequaled.\nThe Devastator need not fear other lesser tanks on the battlefield.", // Devastator
	"The Fremen are little more than slaves to the Atreides.\nIgnorant and aboriginal, they occasionally fight for the Atreides, and are provided food and supplies in return.", // Fremens
	"This tank fires a powerful nerve gas that can cause your weaker-willed soldiers to turn against you.", // Deviator
	"The Ordos depend upon a band of terrorists called Saboteurs.\nIf uncontrolled, these cunning little pests will inflict a great deal of damage to your buildings.\nI suggest eliminating Saboteurs immediately whenever they are encountered.", // Saboteur
	"The Atreides believe that they have found a humane way of eliminating their enemies.\nTheir Sonic Tank fires shock waves instead of rockets.", // SonicTank
	"These huge native worms are almost impossible to kill.\nThey are attracted by surface vibrations, and all ground based units are at risk of being devoured.\n Rock is the only sure defense against these creatures.", //SandWorm
	"This transport aircraft can carry large vehicles, such as harvesters, to their destinations more efficiently than letting them wander about on their own.", // Carryall
	"This transport aircraft can carry large vehicles, such as harvesters, to their destinations more efficiently than letting them wander about on their own.", // Fregate
	"This fragile little aircraft is merely an annoyance.\nTheir delicate armor is no match for our weapons of war.", // Ornithopter
};
const char* object_o_info[LastObject + 1] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	"The Atreides come from Caladan, a planet whose warm climate and over - productive agriculture undoubtedly make the citizens kind and non - violent.\nThis is their weakness.", // Atreides
	"Giedi Prime is the home to the Harkonnen.\nDo not be fooled by their blind hatred and bloodlust.\nThey are devious, shrewd, and must be watched carefully!", // Harkonens
	"The Ordos come from many different homes.\nBecause of this diversity, we are strong and highly organized.\nWe know when it is necessary to utilize resources and when to appropriate the required materials.\nOur organization is highly efficient and effective.", // Ordos
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
	"", // Raider
	"", // Death Hand
	"", // Devastator
	"", // Fremens
	"", // Deviator
	"", // Saboteur
	"", // SonicTank
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