#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "game.h"
#include "player.h"
#include "pointa.h"
#include "pushvalue.h"
#include "unit.h"

static pointa points;
static playeri* player_active;
static int explore_demand, units_demand;
static objectn build_order[] = {Refinery, Windtrap, SpiceSilo, Barracks, Wor};

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

static short unsigned find_executor(statn best) {
	auto result_value = 0;
	unit* result = 0;
	for(auto& e : bsdata<unit>()) {
		if(!e || e.isboard() || e.player != player_index)
			continue;
		auto value = e.get(best);
		if(result_value < value) {
			result_value = value;
			result = &e;
		}
	}
	return result ? result - bsdata<unit>::elements : 0xFFFF;
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

static void explore_area() {
	if(!explore_demand)
		return;
	points.clear();
	points.select(allarea(), isnotexploredborder);
	if(!points)
		return;
	auto v = points.nearest(player_active->base);
	auto p = find_free_explorer(v);
	if(!p) {
		units_demand++;
		return;
	}
	p->order = v;
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
		if(!e || e.player != player_index || e.type != Harvester || area.isvalid(e.target_position))
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
	return points.random();
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

static void build_structrure(objectn t) {
	auto factory = getbuild(t);
	auto pb = find_base(factory, player_index);
	if(!pb)
		return;
	if(pb->isworking())
		return;
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
}

static void check_build_order() {
	int objects[64] = {};
	for(auto t : build_order) {
		objects[t]++;
		if(player_active->objects[t] < objects[t]) {
			build_structrure(t);
			break;
		}
	}
}

static void active_player_update() {
	units_demand = 0;
	explore_demand = 0;
	check_spice_area();
	explore_area();
	harvester_commands();
	check_build_order();
	check_build_placement();
}

void update_ai_commands(unsigned char player) {
	if(player >= bsdata<playeri>::source.count)
		return;
	pushvalue push_active(player_active, bsdata<playeri>::elements + player);
	pushvalue push_player(player_index, player);
	active_player_update();
}