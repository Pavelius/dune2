#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "game.h"
#include "player.h"
#include "pointa.h"
#include "pushvalue.h"
#include "unit.h"

struct objectunit {
	objectn	type;
	unsigned char count;
};

static pointa points;
static playeri* player_active;
static int explore_demand;
static objectn build_order[] = {Refinery, Windtrap, SpiceSilo, Barracks, RadarOutpost, Wor, LightVehicleFactory};
static objectunit build_units[] = {{LightInfantry, 5}, {HeavyInfantry, 5}, {Quad, 10}, {Tank, 5}, {AssaultTank, 5}, {RocketTank, 5}};

static bool isexploredspice(point v) {
	if(!area.is(v, player_index, Explored))
		return false;
	return isspice(v);
}

static bool isnotexploredborder(point v) {
	if(area.is(v, player_index, Explored))
		return false;
	for(auto d : all_strait_directions) {
		if(area.is(to(v, d), player_index, Explored))
			return true;
	}
	return false;
}

static bool player_own(objectn t, int count = 1) {
	if(!player_active)
		return false;
	if(t < lenghtof(player_active->objects))
		return player_active->objects[t] >= count;
	return false;
}

static unit* find_free_explorer(point near) {
	unit* result = 0;
	int result_range = 256 * 256;
	for(auto& e : bsdata<unit>()) {
		if(!e || e.isboard() || e.isorder() || e.type == Harvester || e.player != player_index)
			continue;
		auto range = e.position.range(near);
		if(result_range > range) {
			result = &e;
			result_range = range;
		}
	}
	return result;
}

static short unsigned find_executor(statn best) {
	auto result_value = 0;
	unit* result = 0;
	for(auto& e : bsdata<unit>()) {
		if(!e || e.type == Harvester || e.isboard() || e.player != player_index)
			continue;
		auto value = e.get(best);
		if(result_value < value) {
			result_value = value;
			result = &e;
		}
	}
	return result ? result - bsdata<unit>::elements : 0xFFFF;
}

static unit* find_scout() {
	if(player_active->scout == 0xFFFF)
		player_active->scout = find_executor(Speed);
	if(player_active->scout == 0xFFFF)
		return 0;
	auto p = bsdata<unit>::elements + player_active->scout;
	if(p->isorder())
		return 0;
	return p;
}

static point enemy_spotted() {
	if(!area.isvalid(player_active->base))
		return {-10000, -10000};
	point result = {-10000, -10000};
	int result_range = 256 * 256 * 256;
	for(auto& e : bsdata<unit>()) {
		if(!e || e.player == player_index || !area.is(e.position, player_index, Visible))
			continue;
		auto range = e.position.range(player_active->base);
		if(result_range > range) {
			result_range = range;
			result = e.position;
		}
	}
	return result;
}

static point enemy_building_spotted() {
	if(!area.isvalid(player_active->base))
		return {-10000, -10000};
	point result = {-10000, -10000};
	int result_range = 256 * 256 * 256;
	for(auto& e : bsdata<building>()) {
		if(!e || e.player == player_index || !area.is(e.position, player_index, Explored))
			continue;
		auto range = e.position.range(player_active->base);
		if(result_range > range) {
			result_range = range;
			result = e.position;
		}
	}
	return result;
}

static bool need_update_enemy_spot(point v) {
	if(!area.isvalid(v))
		return true;
	return (player_active->enemy_spot_turn - game.turn) > 200;
}

static void check_enemy_spotted() {
	if(need_update_enemy_spot(player_active->enemy)) {
		player_active->enemy = enemy_spotted();
		player_active->enemy_spot_turn = game.turn;
	}
	player_active->enemy_base = enemy_building_spotted();
}

static void seek_enemy_base() {
	if(!player_active->objects[RadarOutpost])
		return;
	if(area.isvalid(player_active->enemy_base))
		return;
	explore_demand++;
}

static void explore_area() {
	if(!explore_demand)
		return;
	points.clear();
	points.select(allarea(), isnotexploredborder);
	if(!points)
		return;
	auto v = points.nearest(player_active->base);
	auto p = find_scout();
	if(!p)
		return;
	p->order = area.nearest(v, isfreetrack, 10);
}

static void check_spice_area() {
	if(area.isexist(player_active->spice, 4, isspice))
		return;
	points.clear();
	points.select(allarea(), isexploredspice);
	if(!points) {
		explore_demand++;
		return;
	}
	player_active->spice = points.nearest(player_active->base);
}

static void harvester_commands() {
	if(!area.isvalid(player_active->spice))
		return;
	for(auto& e : bsdata<unit>()) {
		if(!e || e.isboard() || e.player != player_index || e.type != Harvester || area.isvalid(e.target_position))
			continue;
		e.target_position = player_active->spice;
	}
}

static point choose_placement(point factory, objectn t, bool full_slab_size) {
	auto size = bsdata<shapei>::elements[getshape(t)].size;
	markbuildarea(factory, size, t, full_slab_size);
	points.clear();
	points.select(allarea(), isnonblocked);
	if(!points)
		return {-10000, -10000};
	return points.nearest(area.center());
}

static void build_placement(building& e) {
	auto v = choose_placement(e.position, e.build, true);
	if(!area.isvalid(v)) {
		// TODO: check if we can remove units
		e.cancel();
		return;
	}
	pushvalue push(last_building);
	e.construct(v);
}

static void check_build_placement() {
	for(auto& e : bsdata<building>()) {
		if(!e || e.player != player_index || !e.isbuildplacement())
			continue;
		build_placement(e);
	}
}

static bool build_structrure(objectn t) {
	auto factory = getbuild(t);
	auto pb = find_base(factory, player_index);
	if(!pb)
		return false;
	if(pb->isworking())
		return false;
	if(t != Slab && t != Slab4) {
		auto v = choose_placement(pb->position, t, true);
		if(!area.isvalid(v)) {
			t = Slab4;
			v = choose_placement(pb->position, t, true);
			if(!area.isvalid(v)) {
				t = Slab;
				v = choose_placement(pb->position, t, true);
			}
		}
		if(!area.isvalid(v)) {
			// TODO: there is no building place anymore.
		}
	}
	pb->build = t;
	pb->progress();
	return true;
}

static void check_build_order() {
	int objects[64] = {};
	for(auto t : build_order) {
		objects[t]++;
		if(!player_own(t, objects[t])) {
			build_structrure(t); // Try build first that match
			break;
		}
	}
}

static void build_unit(objectn t) {
	auto build = getbuild(t);
	if(!player_own(build))
		return;
	for(auto& e : bsdata<building>()) {
		if(!e || e.player != player_index || e.type!=build || e.isworking())
			continue;
		e.build = t;
		e.progress();
	}
}

static void check_build_units() {
	for(auto& e : build_units) {
		if(!player_own(e.type, e.count))
			build_unit(e.type);
	}
}

static bool player_gain_army() {
	for(auto& e : build_units) {
		if(!player_own(e.type, e.count))
			return false;
	}
	return true;
}

static areai::fntest get_move(movementn n) {
	switch(n) {
	case Footed: return isfreefoot;
	default: return isfreetrack;
	}
}

static point get_near(objectn type, point v) {
	return area.nearest(v, get_move(getmove(type)), 15);
}

static void give_order_army(point v) {
	if(!area.isvalid(v))
		return;
	for(auto& e : bsdata<unit>()) {
		if(!e || e.player != player_index || e.isorder() || getweapon(e.type)==NoEffect)
			continue;
		e.order = get_near(e.type, v);
	}
}

static void check_army() {
	if(!player_gain_army())
		return;
	give_order_army(player_active->enemy_base);
}

static void active_player_update() {
	explore_demand = 0;
	check_spice_area();
	check_enemy_spotted();
	seek_enemy_base();
	explore_area();
	harvester_commands();
	check_build_order();
	check_build_placement();
	check_build_units();
	check_army();
}

void update_ai_commands(unsigned char player) {
	if(player >= bsdata<playeri>::source.count)
		return;
	pushvalue push_active(player_active, bsdata<playeri>::elements + player);
	pushvalue push_player(player_index, player);
	active_player_update();
}