#include "game.h"
#include "scenario.h"

scenarioi scenarios[3 * 9] = {
	// Atreides 1
	{{RadarOutpost, Refinery, Windtrap, HeavyTroopers, LightInfantry}, 1000,
	{{Atreides, 0, {13, 7, 20, 14, 21, 22}}, {Harkonens, 2, {6, 5, 4, 10, 3, 9}}, {Ordos, 1, {19, 27, 26, 25, 24, 23}}},
	{{8, 3, 80, 80}, {15, 2, 101, 118}, {23, 3, 152, 136}},
	},
};

scenarioi* current_scenario() {
	return scenarios + (game.house - Atreides) * 3 + (game.scenario - 1);
}