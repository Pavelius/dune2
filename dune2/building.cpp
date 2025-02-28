#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "fix.h"
#include "game.h"
#include "objecta.h"
#include "pointa.h"
#include "resid.h"

BSDATA(tilepatch) = {
	{334, 342, BoardUnit, Refinery},
	{339, 344, BoardUnit, Refinery},
	{314, 326, BoardUnit, Starport},
	{315, 327, BoardUnit, Starport},
	{319, 330, BoardUnit, Starport},
	{320, 331, BoardUnit, Starport},
	{262, 264, BoardUnit, HeavyVehicleFactory},
	{263, 265, BoardUnit, HeavyVehicleFactory},
	{264, 266, BoardUnit, HeavyVehicleFactory},
	{265, 267, BoardUnit, HeavyVehicleFactory},
	{249, 250, BoardUnit, LightVehicleFactory},
	{350, 352, BoardUnit, RepairFacility},
};
BSDATAF(tilepatch)

BSDATAC(building, 512)
building* last_building;

void building::clear() {
	memset(this, 0, sizeof(*this));
}

void building::updateturrets() {
	if(type == Turret || type == RocketTurret)
		area.set(position, shoot_direction, getframes(type)[0]);
}

objectn get_first_build() {
	for(auto i = ConstructionYard; i <= RocketTank; i = (objectn)(i + 1)) {
		if(last_building->canbuild(i))
			return i;
	}
	return NoObject;
}

void add_building(point pt, objectn type) {
	last_building = bsdata<building>::addz();
	last_building->position = pt;
	last_building->type = type;
	last_building->unit_board = 0xFFFF;
	auto shape = getshape(type);
	last_building->hits = bsdata<shapei>::elements[shape].hits;
	if(last_building->canbuild(Slab))
		last_building->build = Slab;
	else
		last_building->build = get_first_build();
	area.set(last_building->position, shape, getframes(type));
	last_building->scouting();
	last_building->shoot_direction = Up;
	last_building->player = player_index;
	last_building->stop();
	area.set(last_building->getrect(), setnofeature, 0);
}

static const tilepatch* find_patch(objectn id, int action) {
	for(auto& e : bsdata<tilepatch>()) {
		if(e.id == id && e.action == action)
			return &e;
	}
	return 0;
}

static const tilepatch* find_patch_ne(const tilepatch* pb, int action) {
	auto pe = bsdata<tilepatch>::end();
	while(pb < pe) {
		if(pb->action != action)
			return pb;
		pb++;
	}
	return 0;
}

building* find_building(point v) {
	for(auto& e : bsdata<building>()) {
		if(e && e.position == v)
			return &e;
	}
	return 0;
}

building* find_board(const unit* p) {
	if(!p || !p->isboard())
		return 0;
	auto v = p->getindex();
	for(auto& e : bsdata<building>()) {
		if(e && e.unit_board == v)
			return &e;
	}
	return 0;
}

void building::board(unit* p) {
	if(unit_board != 0xFFFF)
		return;
	unit_board = p->getindex();
	set(BoardUnit, true);
	p->position = {-100, -100};
	p->order = p->position;
	p->screen = m2sc(p->position);
}

void building::unboard() {
	if(unit_board == 0xFFFF)
		return;
	auto v = area.nearest(position, isfreetrack, 8);
	if(!area.isvalid(v))
		return;
	set(BoardUnit, false);
	auto p = bsdata<unit>::elements + unit_board;
	p->position = v;
	p->screen = m2sc(p->position);
	p->stopmove();
	unit_board = 0xFFFF;
}

void building::set(buildstaten action, bool apply) {
	auto p1 = find_patch(type, action);
	if(!p1)
		return;
	auto p2 = find_patch_ne(p1, action);
	if(!p2)
		p2 = bsdata<tilepatch>::end();
	area.patch(position, getsize(), p1, p2 - p1, apply);
}

void building::scouting() {
	area.scouting(position, getsize(), player, getlos());
}

void building::cleanup() {
	if(last_building == this)
		last_building = 0;
}

static short unsigned* destroyed_shape(shapen v) {
	static short unsigned d1x1[] = {355};
	static short unsigned d2x2[] = {213, 214, 232, 233};
	static short unsigned d3x2[] = {213, 214, 215, 232, 233, 234};
	static short unsigned d3x3[] = {213, 214, 215, 232, 233, 234, 223, 224, 225};
	switch(v) {
	case Shape1x1: return d1x1;
	case Shape2x2: return d2x2;
	case Shape3x2: return d3x2;
	default: return 0;
	}
}

static void set_terrain(point v, int value) {
	area.set(v, (terrainn)value);
}

static void set_explosion(point v, int value) {
	if(game_chance(20))
		add_effect(m2sc(v), FixBigExplosion);
	else
		add_effect(m2sc(v), FixExplosion);
}

static void apply_destroyed_structures(point position, shapen v) {
	auto p = destroyed_shape(v);
	if(p)
		area.set(position, v, p);
	else
		area.set(position, bsdata<shapei>::elements[v].size, set_terrain, Rock);
	area.set(position, bsdata<shapei>::elements[v].size, set_explosion, 0);
}

void building::destroy() {
	cleanup();
	apply_destroyed_structures(position, getshape(type));
	clear();
}

static point random_point(const building* p) {
	auto m = p->getsize();
	m.x = p->position.x + xrand(0, m.x - 1);
	m.y = p->position.y + xrand(0, m.y - 1);
	return m2sc(m);
}

void building::damage(int value) {
	if(value >= hits) {
		destroy();
		return;
	}
	hits -= value;
	if(hits <= gethitsmax() / 2) {
		if(game_chance(30))
			add_effect(random_point(this), BurningFire);
	}
	if(game_chance(20))
		add_effect(random_point(this), Smoke);
}

static void set_control(point v, shapen shape, unsigned char player) {
	auto& ei = bsdata<shapei>::elements[shape];
	for(auto y = 0; y < ei.size.y; y++) {
		for(auto x = 0; x < ei.size.x; x++) {
			auto n = point(x, y) + v;
			if(!area.isvalid(n))
				continue;
			area.set(n, player, Control);
		}
	}
}

void building::construct(point v) {
	if(!area.isvalid(v))
		return;
	if(getparent(build)==Building) {
		auto shape = getshape(build);
		if(build == Slab || build == Slab4) {
			auto size = bsdata<shapei>::elements[shape].size;
			area.set(v, shape, SlabFeature);
			set_control(v, shape, player);
			area.scouting(v, size, player, getlos());
		} else {
			add_building(v, build);
			set_control(v, shape, player);
		}
	}
	build_spend = 0;
}

void building::cancel() {
	if(build_spend && canbuild()) {
		getplayer().add(Credits, build_spend);
		build_count = 0;
		build_spend = 0;
	}
}

bool building::canbuild(objectn build) const {
	if(getbuild(build) != type)
		return false;
	auto parent = getparent(build);
	if(parent == Building) {
		auto required = getrequired(build);
		if(required && !getplayer().objects[required])
			return false;
	}
	return true;
}

bool building::canbuild() const {
	switch(type) {
	case ConstructionYard:
	case LightVehicleFactory:
	case HeavyVehicleFactory:
	case Barracks:
	case Wor:
	case Starport:
	case HighTechFacility:
		return true;
	default:
		return false;
	}
}

static point get_unit_place_point(point start, int range) {
	area.blockland(Tracked);
	blockunits();
	return area.nearest(start, isnonblocked, range);
}

bool building::autoproduct() {
	if(!build)
		return false;
	if(build_spend < getcreditscost(build))
		return false;
	if(getparent(build) != Unit)
		return false;
	auto v = get_unit_place_point(position, 32);
	if(area.isvalid(v))
		add_unit(v, build, to(position, v), player);
	build_spend = 0;
	return true;
}

void building::update() {
	const auto shoot_range = 8;
	tracking();
	if(isworking()) {
		progress();
		if(autoproduct()) {
			if(build_count) {
				progress();
				build_count--;
			}
		}
	} else if(shoot(m2sc(position), Shoot20mm, 2, shoot_range)) {
		// Nothing to do
	} else if(area.isvalid(target_position) && !canshoot(shoot_range)) {
		stop();
	} else if(seeking(shoot_range)) {
		// Nothing to do
	}
	updateturrets();
}

fixn building::getweapon() const {
	switch(type) {
	case Turret: return Shoot30mm;
	case RocketTurret: return FireRocket;
	default: return NoEffect;
	}
}

int	building::getprogress() const {
	if(!build_spend || !build)
		return 0;
	auto build_cost = getcreditscost(build);
	if(!build_cost)
		return 100;
	if(build_spend >= build_cost)
		return 100;
	return build_spend * 100 / build_cost;
}

int	building::gethitsmax() const {
	return bsdata<shapei>::elements[getshape(type)].hits;
}

point building::getbuildsize() const {
	if(getparent(build)==Building)
		return bsdata<shapei>::elements[getshape(build)].size;
	return {1, 1};
}

point building::getsize() const {
	return bsdata<shapei>::elements[getshape(type)].size;
}

bool building::isbuildplacement() const {
	return build && build_spend && build_spend >= getcreditscost(build);
}

bool building::isnear(point v) const {
	auto size = getsize();
	return v.x >= position.x - 1 && v.x <= position.x + size.x
		&& v.y >= position.y - 1 && v.y <= position.y + size.y;
}

bool building::progress() {
	if(build) {
		unsigned short can_spend = 5;
		auto build_cost = getcreditscost(build);
		if(build_cost <= 10)
			can_spend = 1;
		auto credits = getplayer().get(Credits);
		if(can_spend > credits)
			can_spend = credits;
		if(build_spend + can_spend > build_cost)
			can_spend = build_cost - build_spend;
		if(can_spend > 0) {
			getplayer().add(Credits, -can_spend);
			build_spend += can_spend;
			return true;
		}
	}
	return false;
}

void building::buildlist() const {
	subjects.clear();
	for(auto i = ConstructionYard; i < LastObject; i = (objectn)(i+1)) {
		if(canbuild(i))
			subjects.add(i);
	}
}

rect building::getrect() const {
	auto size = getsize();
	return {position.x, position.y, position.x + size.x, position.y + size.y};
}

building* find_base(objectn type, unsigned char player) {
	for(auto& e : bsdata<building>()) {
		if(!e)
			continue;
		if(e.player == player && e.type == type)
			return &e;
	}
	return 0;
}

void building::setblock(short unsigned n) const {
	auto size = getsize();
	auto x2 = position.x + size.x;
	auto y2 = position.y + size.y;
	for(auto y = position.y; y < y2; y++) {
		for(auto x = position.x; x < x2; x++) {
			if(area.isvalid(x, y))
				path_map[y][x] = n;
		}
	}
}

point building::nearestboard(point v, movementn move) const {
	if(!area.isvalid(v))
		return {-10000, -10000};
	blockland(move);
	blockunits();
	path_map[v.y][v.x] = 0;
	unblock();
	area.movewave(position, move); // Consume time action
	if(path_map[position.y][position.x] == BlockArea)
		return {-10000, -10000};
	block();
	return find_smallest_position();
}

bool isbuildplace(point v) {
	auto f = area.getfeature(v);
	if(f != SlabFeature)
		return false;
	return true;
}

bool isbuildslabplace(point v) {
	if(area.get(v) != Rock)
		return false;
	auto f = area.getfeature(v);
	if(f >= BuildingHead)
		return false;
	return true;
}

bool isbuildslabplacens(point v) {
	if(area.get(v) != Rock)
		return false;
	auto f = area.getfeature(v);
	if(f == SlabFeature)
		return false;
	if(f >= BuildingHead)
		return false;
	return true;
}

static bool iscopycontrol(point v) {
	auto cost = path_map_copy[v.y][v.x];
	return cost != 0 && cost != BlockArea;
}

void markbuildarea(point base, point placement_size, objectn build, bool full_slab_size) {
	area.blockcontrol();
	area.controlwave(base, allowcontrol, 32);
	memcpy(path_map_copy, path_map, sizeof(path_map));
	clearpath();
	if(build == Slab || build == Slab4) {
		if(full_slab_size)
			blockarea(isbuildslabplacens, placement_size);
		else
			blockarea(isbuildslabplace, placement_size);
	} else
		blockarea(isbuildplace, placement_size);
	blockareaor(iscopycontrol, placement_size);
}