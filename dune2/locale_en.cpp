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
	"Wall", "Concrete Slab", "Concrete Slab Blocks", "Turret", "Rocket Turret",
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
	"Walls are the perfect passive defense system.\nThey should be used to fortify silos and other weak structures.", // Wall
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
	"The Ordos produce a Trike that they call the \"Raider\".\n The \"Raider\" is much like our own Trike, only it is faster, more maneuverable, and highly agile, yet it has less armor.", // Raider
	"One of the most destructive weapons on Dune is the Harkonnen missile called the Death Hand.\nThis multiple explosive warhead affects a wide area and can cause much greater damage than a conventional missile.", // Death Hand
	"This massive tank is one of the newest war machines developed by the Harkonnen.\nIt has nearly impenetrable armor, enormous firepower, and is capable of destroying nearly any obstacle in its path.", // Devastator
	"The native peoples of this planet are excellent guerrilla fighters.\nThe Fremen are friendly toward us because House Atreides has always dealt with them fairly, and as equals.", // Fremens
	"The Deviator fires missiles containing dangerous nerve gas that can seriously distort the perceptions of affected personnel, and may even incite full scale mutinies.", // Deviator
	"The Ordos often employ a band of local terrorist fighters called Saboteurs.\nThese men are well versed in sabotage, and can easily penetrate defensive perimeters en route to the buildings they intend to destroy.\nThey should be attacked and killed promptly whenever discovered.", // Saboteur
	"The Sonic Tank is one of our unique weapons.\nIt fires a blast of sonic energy rather than projectile weapons.\nAn excellent weapon for defensive missions, it is especially effective against enemy infantry.", // SonicTank
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
	"Walls are an excellent choice for defensive reinforcement.\nLike the concrete slabs, walls are inexpensive to produce.", // Wall
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
	"The construction yard will allow you to erect other buildings.", // ConstructionYard
	"Silos are used for the storage of spice.\nThey are weaker and more easily destroyed than harvesters or refineries.\nUse this knowledge to your advantage.", // SpiceSilos
	"The intergalactic purchasing network accessible through the starport will allow you to acquire weaponry in bulk orders.", // Starport
	"Windtraps are the only effective source of power on Dune.\nAlthough the traps are fairly strong, they tend to be easily damaged, and it would be wise to always keep more than one wind trap in operation.", // Windtrap
	"The refinery is necessary for the conversion of spice into credits.\nWithout a refinery, there is no way for you to obtain new credits by harvesting spice.", // Refinery
	"The radar capabilities of the outpost are vital to your operations.\nWith radar, you will be able to monitor the movements of the other Houses and prepare defensive and attack strategies.", // RadarOutpost
	"If you do not have a repair facility, it will not be possible for you to fix damaged vehicles.\nIf your damaged vehicles do not receive repairs, you will lose them completely.", // RepairFacility
	"IX will allow you to create better, faster, and more efficient machines.\nIt will provide the means to produce some of our most special vehicles.", // HouseOfIX
	"When the House grants you a Palace, you will know that you have pleased them.\nIf you are ever to lose your Palace to the enemy, your mission will surely be lost.", // Palace
	"These buildings produce the light infantry, which are some of the most important fighting units in our military system.", // Barracks
	"WOR is the means of producing the heavy troopers.", // Wor
	"The light factory produces our Raider Trikes as well as Quads.", // LightVehicleFactory
	"The heavy factory produces all large tracked vehicles, like tanks and Harvesters.", // HeavyVehicleFactory
	"The High-Tech Factory utilizes technology to produce advanced aircraft.", // HighTechFactory
	"Walls may provide for a fair defense around the perimeter of a compound.", // Wall
	"Concrete slabs are an effective and inexpensive foundation material.\nStructures without foundations will require frequent repair, and waste valuable time and money.", // Slab
	"Concrete slabs are an effective and inexpensive foundation material.\nStructures without foundations will require frequent repair, and waste valuable time and money.", // Slab4
	"The turrets are highly accurate weapons.\nTheir only drawback is that once established, they cannot be moved.", // Turret
	"The Rocket turret, like the regular turret, cannot be moved.\nHowever, it has greater range capabilities than the regular turret.\nTogether, these should provide excellent long and short range firing capabilities.", // RocketTurret
	"The Mobile Construction Vehicle offers an efficient means of expanding the territory under your control.\nIt can provide support for the construction of new facilities at some distance from your existing structures.", // MCVehicle
	"Harvesters are large, slow vehicles that scout for spice fields and begin harvesting whenever raw spice is found.\nObviously this is a highly inefficient process.\nWe encourage our field commanders to also appropriate previously harvested materials, which is certainly a faster and more efficient method of spice acquisition.", // Harvester
	"The light infantry allows for an expendable force of men.\nIf used in large numbers, they are almost unbeatable.", // LightInfantry
	"The heavy troopers carry more armor and yet are still faster than the light infantry.\nTheir abilities are well suited for ground assaults and reconnaissance missions.", // HeavyTroopers
	"The three wheeled vehicles that other Houses use is lacking in many aspects.\nUnlike our Raider, Trikes are unable to maintain the speeds that we demand.\nUnfortunately, we had to sacrifice armor for this increase of speed.", // Trike
	"This four-wheeled vehicle is not well suited to our purposes.\nAlthough it has good armor, it is not maneuverable enough and it lacks the speeds we require.", // Quad
	"This slow, single barreled tank is adequate in its performance.\nHowever, there are much faster and more efficient tanks available.", // CombatTank
	"The Siege Tank possesses more than twice the firepower of the combat tank.\nYet, despite all the power and armor, it is quite slow.", // SiegeTank
	"Although the Rocket Tank is faster than the combat tank and has long range firing capabilities, we prefer more useful tanks.\nI suggest sending your \"Raiders\" against rocket tanks.", // RocketTank
	"The Raider is far better than a standard trike.\nIt is faster and has excellent weapons.\nBecause of its maneuverability, it is the best vehicle for those missions where you must rely on swiftness and agility.", // Raider
	"The Harkonnen possess these missiles that are capable of delivering multiple warheads to a single strike area.\nAlthough fearfully named, fortunately they are also highly inaccurate.", // DeathHand
	"The Harkonnen have created this new tank that is capable of inflicting severe damage to even the most well protected vehicles and buildings.\nFor all its firepower and armor, however, it is extremely slow and may be easily outmaneuvered.", // Devastator
	"The native Fremen of Dune are simple but skilled fighters.\nThey are currently utilized by the Atreides for combat purposes.\nPerhaps, with the right incentives, they could be persuaded to join our forces instead.", // Fremens
	"Our Deviator fires a nerve gas that is capable of twisting the minds of enemy soldiers.\nSome of our best regiments are primarily composed of formerly hostile military personnel who have been reprogrammed by exposure to this gaseous argument.", // Deviator
	"The Saboteur is the group name given to this unit of soldiers specializing in sabotage.\nThey are effective for specific target destruction, and are based at all of our Palaces.", // Saboteur
	"The Atreides have developed a Sonic Tank that fires sonic blasts instead of projectiles.\nBe careful around these, their blasts have long range effects.", // SonicTank
	"Indigenous to the sands of Dune, these monsters are attracted by vibrations, and can destroy large vehicles in a single bite.\n Rock is the only sure defense against these creatures.", // SandWorm
	"The carryall is capable of deploying and retrieving the large tracked vehicles like harvesters.\nUsing carryalls is far quicker than waiting for the harvester to return on its own.", // Carryall
	"The carryall is capable of deploying and retrieving the large tracked vehicles like harvesters.\nUsing carryalls is far quicker than waiting for the harvester to return on its own.", // Fregate
	"The ornithopter is a fast and maneuverable, aircraft.\nIt is effective for strategic strikes, but armor has been sacrificed for speed and agility.", // Ornithopter
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
	"", // Wall
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
	"Dune II: The Building of A Dynasty", "Cancel", "Yes", "No",
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
	"Do you want to join %1?",
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
const char* intro_subtitle[] = {
	"The planet Arrakis, known as Dune.",
	"Land of sand.",
	"Home of the Spice Melange.",
	"The Spice controls the Empire.",
	"Whoever controls Dune",
	"controls the Spice.",
	"The Emperor has proposed a challenge",
	"to each of the Houses.",
	"The House that produces the most Spice",
	"will control Dune.",
	"There are no set territories.",
	"And no rules of engagement.",
	"Vast armies have arrived.",
	"Now three Houses fight for control of Dune.",
	"The noble Atreides.",
	"The insidious Ordos.",
	"And the evil Harkonnen.",
	"Only one House will prevail.",
	"Your battle for Dune begins",
	"NOW",
};
const char* scenario_a_text[40] = {
	"Test scenario win text.",
	"Test scenario lose text.",
	"Test scenario advice text.",
	"Greetings, I am your Mentat Cyril.\nTogether we will purge this planet of the foulness of the other Houses.\nThe High Command wishes you to produce 1000 credits.\nYou may earn credits by building a refinery and harvesting spice.",
	"Congratulations! I knew that you would achieve your goal with very little trouble.\nI look forward to assisting you in future missions.",
	"This is very disappointing.\nWe had such high hopes for you.\nWe must insist that you try again.\nOur confidence is surely not misplaced, and we look forward to the successful completion of your first test.",
	"First build a two by two group of concrete slabs.\n Then build a windtrap and place it upon the concrete.\nDo the same for a refinery so that you can reach your quota of 1000 credits.",
	"Greetings, I am honored to see you again.\nThe High Command now requires that you produce 2700 credits in a new harvesting area.\nUnfortunately, we have confirmed the presence of an Ordos base in this region.\nGood luck!",
	"Well done! Our effort to maintain fair play on this planet is opposed by both the Ordos and the Harkonnen.\nThe spice that you collected will help us greatly in our efforts.",
	"The loss of this region to the Ordos will hurt House Atreides greatly.\nPlease try harder next time.\nThe seriousness of your mission cannot be overemphasized!",
	"If you have a windtrap then build a barracks in order to produce soldiers.\nI would do so to provide a greater defense for your harvesting operation.",
	"The battle with the other Houses has intensified, and we are now forced to engage in some selected offensive maneuvers.\nThe Harkonnen are being extremely troublesome in your next region, and we must ask that you remove their presence from the area.",
	"Hurrah!  Although I prefer defense, a fine offensive against such a deserving foe as the Harkonnen is a joy to behold and a reaffirmation of all that is good and right.\nHouse Atreides thanks you for your efforts!",
	"It is a cruel world, is it not? I'm not sure if I was expecting divine intervention, but as I watched your defeat I could not help thinking how unfair it was!",
	"You need to build a Light Factory to produce the weapons necessary to complete your mission.",
	"Your demonstration of military skills now forces us to assign you to another offensive campaign against House Harkonnen.\nThey have continued to attack our peaceful harvesters, and must be removed from the area!",
	"Well done! No task seems too difficult for you.\nThe Harkonnen will think twice before attacking our harvesters again!",
	"Although I understand the problems confronting you, I will confess that I am disappointed by your defeat.\nI am not sure what went wrong, but please do not let it happen again!",
	"The addition of tanks in your forces is essential to your victory in this region.",
	"Welcome.\nThe rules seem to have changed.\nAs you have witnessed, the Emperor himself has been aiding the efforts of our competitors!  As a part of our new strategy, we must ask that you eliminate the treacherous Ordos forces that presently control this region.",
	"Congratulations again!  There seems to be no limit to your talents.\nI have to think that some divine force motivated your decision to serve within House Atreides.\nOur gain was certainly the others' loss!",
	"This defeat of yours will not look good in my reports.\nI am not sure how much longer we can afford to support you.\nThere are many others clamoring to take your place, so I suggest that you do whatever is required to be victorious in your next encounters!",
	"The production of Carryalls will greatly speed up your harvesting operations.",
	"As the battle for this planet intensifies, all efforts must be taken to ensure our success.\nOnce again, we must call upon you to destroy our enemies in a troubled sector.\nHouse Harkonnen must be taught a lesson.\nThank you, and good luck!",
	"Hurrah! We have them on the run now, and it's all because of you.\nYour successes have been remarkable! Are you sure you didn't graduate from one of the intergalactic military academies?",
	"There is a time for excuses, but this is not it!  I don't understand how you could have bungled the entire campaign! You have no choice but to apply yourself more diligently during your next assignment.",
	"I advise you to produce and then place Rocket Turrets at strategic defensive positions around your installation.",
	"The battle goes well, but there is no time to relax.\nWe have an urgent need for you to subdue all Ordos forces in this region promptly.\nOur ongoing negotiations are aided immeasurably by corresponding victories in the field.\nWe are counting on you.",
	"Excellent! Your skill seems to improve with each assignment, and your exploits give great confidence to those on the home front.\nKeep up the good work!",
	"My goodness, what an awful defeat! Perhaps we have given you too much responsibility.\nIf you fail at your next assignment we will have to seriously consider sending you home!",
	"The Sonic Tank is a powerful ally against enemy troops.",
	"Although you have earned a well-deserved rest, I'm afraid the political situation requires that we send you back into the field immediately.\nBoth Ordos and Harkonnen forces have built up to unacceptable levels in this region, and now must be removed completely.",
	"A masterful victory! Frankly, we would not be able to remain on Dune without your military guidance.\nWe have come to rely completely on you sir, and cannot imagine further success without you!",
	"A defeat is never welcome, but this loss is particularly devastating! All of our efforts may soon prove futile unless you can provide a victory in your next campaign!",
	"Utilize your Palace's special option whenever possible, for it costs nothing and it will recharge.",
	"Your next assignment will determine the entire outcome of our efforts here on Dune.\nVictory will not come easily.\nIn addition to destroying all remaining Ordos and Harkonnen troops, you are also instructed to subdue Emperor Frederick's forces.\nAll of our hopes and dreams are riding with you, and we humbly beg of you to provide one final victory for our noble House Atreides.",
	"Good Morning, your lordship, and congratulations! You have served House Atreides well.\nWe will not soon forget our most noble warrior! I go now to relay the news of your most glorious victory, and deliver your terms to the Emperor.",
	"Your defects must have been inherited.\nNobody could possibly learn the colossal stupidity that you have displayed.",
	"Watch for sneak attacks, defend your installations well."
};
const char* scenario_h_text[40] = {
	"Test scenario win text.",
	"Test scenario lose text.",
	"Test scenario advice text.",
	"I am the Mentat, Radnor.\nWith my guidance, you may be able to assist us in conquering this dusty, little planet.\nFor your first test you will be expected to produce 1000 credits, and not a granule less.\nYou may earn credits by harvesting spice, and will need to build a refinery to convert spice to credits.\nIf any of our foolish enemies attempt to attack your base you will have the pleasure of seeing the invincible Harkonnen troops in action.",
	"You have pleased me.\nContinue to serve me well, and I will see to it that you are rewarded.",
	"You are beneath my contempt.\nDo you know what happens to those who have failed House Harkonnen?",
	"First build a two by two group of concrete slabs.\n Then build a windtrap and place it upon the concrete.\n Do the same for a refinery so that you can reach your quota of 1000 credits.",
	"House Harkonnen has generously granted you a new opportunity to serve us.\nWe will now allow you to take command in a more dangerous area, and accumulate 2700 credits.\nAlthough the worthless Atreides you may encounter in this region should always be eliminated as a matter of principle, the spice quota is your objective.",
	"Again you have pleased me.\nYou remind me of myself in my youth.\nYou may indeed have true Harkonnen blood coursing through your veins!",
	"I cannot believe that you were incapable of accumulating your quota.\nIt was a simple task that should have let us examine your leadership capabilities.\nI suggest you do not try my patience further.",
	"If you have a windtrap then build a House of WOR structure in order to produce troopers.\nI would do so to provide a greater defense for your harvesting operation.",
	"The despised Ordos are well established in this region, and are harvesting spice that should rightfully belong to House Harkonnen.\nDestroy the Ordos installations in this area and assert control in the name of House Harkonnen.",
	"Congratulations.\nYou are making progress.\nSeveral of my associates have expressed pessimistic thoughts concerning your abilities, and I am glad that you have proved them wrong.\nLet us see now if my confidence in you is misplaced.",
	"I knew it!  Several of my associates had expressed confidence in your abilities, but I knew that you were too weak to be a Harkonnen commander! Guards! Remove this stench before it fouls the entire planet!",
	"You need to build a Light Factory to produce the weapons necessary to complete your mission.",
	"One small victory does not win the war.\nAnother region has the misfortune to be infested with vermin from House Ordos, and you must now repeat your success.\nI have many delicate political negotiations on my mind, and I don't need to be worrying about loose ends.",
	"Well done.\nThe Ordos dogs make amusing noises when they die, do they not? Indeed, what good is our life if we cannot enjoy the simple pleasures?",
	"Is this the way you repay my faith in you?  Such a serious loss could only have been deliberate!  You are unfit to clean our latrines, much less command our glorious Troopers!",
	"The addition of tanks in your forces is essential to your victory in this region.",
	"So, the Emperor was helping the Ordos in your last mission.\nNever the less, House Atreides has grown stronger due to our negligence, and must now be taught a lesson.\nYou will remove all the Atreides from this region.",
	"In appreciation for your recent performance, I will now allow you to resume the struggle with the Ordos.\nAt least they will put up a fight worthy of our Troopers.",
	"Perhaps you miss your mother?  I am worried about you and cannot let you progress further until you have proved yourself against the Atreides.",
	"The production of Carryalls will greatly speed up your harvesting operations.",
	"You are to proceed into yet another luckless region dominated by those pesky Ordos, and I expect you to overcome this particularly troublesome Ordos group.\nWe have established a good reputation on this planet.\nDo not embarrass me now!",
	"The victor's laurels fit you well my son!  I may even refer other aspirants to study your tactics.\nI would personally apologize for having underestimated the Ordos strength in your last campaign, but that is no longer a relevant concern of ours, is it?",
	"Surely this is a tasteless joke?  How could you manage such a defeat? I may have to slit your throat myself just to calm down!  Did you plan to fail in revenge for some imagined wrong?",
	"I advise you to produce and then place Rocket Turrets at strategic defensive positions around your installation.",
	"Reports of new Atreides activity require that I send you immediately back to the front line.\nYou do not seem to enjoy rest and relaxation anyway.\nI think you would prefer to crush the Atreides.",
	"Congratulations.\nIt is a pleasure to see the weak Atreides dealt with so severely!",
	"Your failure has caused me no end of embarrassment.\nIt will not happen again.\nGuards!",
	"The Devastator is a powerful ally against enemy forces.",
	"I have used my influence to arrange a Palace for you.\nA commander of your status may require relaxation occasionally, but I expect an even greater efficiency on your part will come from our generosity.\nBoth Atreides and Ordos forces exist in this region, and all must be eliminated!",
	"Although praise may be due, you will not have that luxury.\nServing House Harkonnen is reward enough!  If you need flattery, then die in our cause! We will speak fondly of you forever!",
	"Guards! How did this weakling get in here?! He is a Loser, and must not be allowed to corrupt others in our house.\nIf this happens again, I will personally dissect several of you!",
	"Utilize your Palace's special option whenever possible, for it costs nothing and it will recharge.",
	"We have been deceived!  Our bargaining in good faith has only brought a treacherous harvest.\nAll have conspired against us, and so all must die! Your military skills are our last remaining hope for this planet.\nDestroy all remaining Atreides and Ordos forces, and conquer the Emperor's Palace.\nHe has treated us poorly, and must not live another day!",
	"Good Morning, your lordship, and congratulations!  You have served me, I mean House Harkonnen, well.\nOur House will not soon forget our most noble warrior! I go now to relay the news of your most glorious victory, and deliver your terms to the Emperor.",
	"Your defects must have been inherited.\nNobody could possibly learn the colossal stupidity that you have displayed.",
	"Watch for sneak attacks, defend your installations well.",
};
const char* scenario_o_text[40] = {
	"Test scenario win text.",
	"Test scenario lose text.",
	"Test scenario advice text.",
	"Welcome.\nI am your Mentat, and you may call me Ammon.\nTo be of any value to the Cartel, you must provide us with credits.\nAs a test, we will assign you to a region, and ask that you meet a production quota of 1000 credits.\nBuild a refinery and harvest the spice in the area.\nI am very busy, but you may call upon me if you have further questions.",
	"You show potential.\nIf you continue to display good management skills we may find a permanent position for you in our organization.",
	"It is beyond me how you were able to fail such an easy task.\nBecause you come so highly recommended, we may be able to overlook this episode if you are successful with your next assignment.",
	"First build a two by two group of concrete slabs.\n Then build a windtrap and place it upon the concrete.\n Do the same for a refinery so that you can reach your quota of 1000 credits.",
	"Your quota is now 2700 credits, and this spice accumulation is your primary objective.\nWe do not expect you to destroy the Harkonnen forces in the area, however you should consider the time that could be saved by appropriating their silos.",
	"Congratulations.\nThe Cartel is beginning to notice you.\nI think you will definitely have a future with us if you can continue on this track.",
	"Apparently, I was mistaken about your potential.\nIt will cost us a great deal to retake this region.\nOnly complete success with your next assignment could possibly resurrect your career.",
	"If you have a windtrap then build a barracks in order to produce soldiers.\nI would do so to provide a greater defense for your harvesting operation.",
	"We find the activities of Atreides troops in this region inconvenient.\nPlease remove this obstacle.\nWe cannot allow the mythical value of fair play to impede our progress.",
	"Excellent! There were rumors that some sort of moral objections would hinder your effectiveness.\nI am glad to see that you have risen above these petty sensitivities.",
	"Your clumsy management has cost us dearly! This is not a game.\nWe cannot just start over every time you fail to concentrate on the task before you.\nI suggest you rid yourself of all outside interests and devote yourself completely to your next assignment!",
	"You need to build a Light Factory to produce the weapons necessary to complete your mission.",
	"The Atreides forces in this area must be eliminated.\nAs always, we appreciate your capture of any enemy silos or other structures that might be salvageable.",
	"A masterful victory, but be cautious! Your reputation will now precede you in future engagements, and you may find your foes better prepared for your tactics.",
	"How could you possibly fail so quickly? My grandmother could command more efficiently than you! Get back out there and show some initiative, or stop wasting my time!",
	"The addition of tanks in your forces is essential to your victory in this region.",
	"So, our spies reveal why the Sardaukar attacked us in your last mission.\nI will need to look into this.\nAlthough the Harkonnen commanders are laughably stupid, their military strength is a threat to us in this area.\nThey must be eliminated as soon as possible.",
	"Congratulations would be in order, but we expect these results when you are in charge.\nYour continued success brings honor to your family, and all of us in House Ordos.",
	"You bungling fool! Did you expect your enemies to run away at the mere sight of you? Remember: your brain will win the war, not your bullets!",
	"The production of Carryalls will greatly speed up your harvesting operations.",
	"The Atreides have become far too vocal in this sector, and whine constantly about their rights.\nWe Ordos do not have the leisure of pointless conversation, and must ask that you eliminate this distraction.",
	"Well done! Your successful military campaigns have created a political atmosphere well suited to our particular bargaining skills.\nKeep up the good work!",
	"Do you instruct your troops to fire randomly into the air? Is there any plan to your idiotic maneuvers? I suggest that you no longer allow your dog assist in managing your assignments!",
	"I advise you to produce and then place Rocket Turrets at strategic defensive positions around your installation.",
	"Harkonnen forces continue to thwart our efforts in this region, and must be removed completely.\nCrush their beloved troopers and they will run crying back to their ugly mothers!",
	"Excellent! You have won a great victory for House Ordos.\nDoesn't inflicting such a humiliating defeat make your breakfast taste better?",
	"Your failure could not have been timed more poorly! What status and bargaining power can we maintain when you display such ineptitude in the field?",
	"The Deviator is a powerful ally against enemy forces.",
	"Both Atreides and Harkonnen forces oppose our control of this area and must therefore be destroyed.\nThe time for cooperation and compromise has past, and all enemies of House Ordos must be eliminated!",
	"I just received the wonderful news of your stunning victory! I am very pleased, and look forward to telling my grandchildren that it was I who served as Mentat to such a brilliant Ordos commander.",
	"You do not exist in a vacuum! Your ineptitude has endangered everything House Ordos has worked to create here on Arrakis! You had better think about the consequences of your actions, and make whatever amends possible as quickly as you can!",
	"Utilize your Palace's special option whenever possible, for it costs nothing and it will recharge.",
	"Emperor Frederick has joined the list of Ordos' enemies, and must be punished.\nDestroy his troops and any Atreides and Harkonnen remnants that still oppose us on this planet.\nWe have risked everything on this final battle, and cannot tolerate less than your best effort.",
	"Good Morning, your lordship, and congratulations! You have served House Ordos well.\nWe will not soon forget our most noble warrior! I go now to relay the news of your most glorious victory, and deliver your terms to the Emperor.",
	"Your defects must have been inherited.\nNobody could possibly learn the colossal stupidity that you have displayed.",
	"Watch for sneak attacks, defend your installations well.\n",
};
const char* fraction_info[3] = {
	"Caladan, home planet of the Atreides, has a warm, calm climate and the lands are lush and green.\nThe rich soils and mild weather supports an extensive variety of agricultural activities.\nIn recent centuries industrial and technological development has added to the prosperity of the Caladanian peoples.",
	"From the dark world of Giedi Prime the savage House Harkonnen has spread across the universe.\nA cruel people, the Harkonnen are ruthless towards both friend and foe in their fanatical pursuit of power.",
	"The home planet of the Ordos is a frigid and ice-covered world.\nWe presume the Ordos import their agricultural and technological goods from nearby star systems.\nActing as traders and brokers, the Ordos produce no physical products of their own and rely upon their merchandising skills to survive.",
};