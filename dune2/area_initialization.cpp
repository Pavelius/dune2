#include "area.h"
#include "bsdata.h"
#include "building.h"
#include "terrain.h"
#include "unit.h"

terrainn map_terrain[area_frame_maximum];
featuren map_features[area_frame_maximum];
unsigned short map_alternate[area_frame_maximum];

static terrainn find_terrain_by_frame(int frame) {
	if(frame >= 210)
		return Rock; // All buildings (and their remains) builded on rock only
	if(frame >= 34 && frame <= 126)
		return Rock; // All walls, slabs (and their remains) builded on rock. Fog of war don't count - is newer be terrain frame.
	for(auto& e : bsdata<terraini>()) {
		if(frame >= e.frame && frame < e.frame + e.count)
			return (terrainn)(&e - bsdata<terraini>::elements);
	}
	return Sand;
}

static tilepatch tiles_animation[] = {
	{216, 219},
	{219, 216},
	{242, 243},
	{243, 242},
	{254, 257},
	{257, 254},
	{276, 279},
	{279, 276},
	{283, 284},
	{284, 283},
	{288, 290},
	{290, 288},
	{292, 294},
	{294, 292},
	{302, 303},
	{303, 302},
	{307, 308},
	{308, 307},
	{309, 312},
	{312, 309},
	{333, 335},
	{335, 333},
	{346, 348},
	{348, 346},
	{373, 374},
	{374, 373},
	{380, 381},
	{381, 382},
	{382, 383},
	{383, 380},
	{386, 388},
	{388, 386},
};

static int find_frame(const unsigned short* source, size_t count, unsigned short value) {
	for(size_t i = 0; i < count; i++) {
		if(source[i] == value)
			return i;
	}
	return -1;
}

static featuren find_feature_by_frame(int frame) {
	if(!frame)
		return NoFeature;
	for(auto& e : bsdata<featurei>()) {
		if(frame >= e.frame && frame < e.frame + e.count)
			return (featuren)(&e - bsdata<featurei>::elements);
	}
	for(auto& e : bsdata<buildingi>()) {
		auto& s = bsdata<shapei>::elements[e.shape];
		auto i = find_frame(e.frames, s.count, frame);
		if(i != -1) {
			if(i == 0)
				return BuildingHead;
			else if(i < s.size.x)
				return BuildingLeft;
			else
				return BuildingUp;
		}
	}
	switch(frame) {
	case 213: case 214: case 215:
	case 223: case 224: case 225:
	case 232: case 233: case 234:
	case 240:
	case 246: case 247:
	case 313:
	case 355:
	case 377:case 378:
	case 384:case 385:
		return StructureRemains;
	default: return NoFeature;
	}
}

static void initialize_alternate() {
	memset(map_alternate, 0, sizeof(map_alternate));
	for(auto& e : tiles_animation)
		map_alternate[e.from] = e.to;
}

static void add_feature_frame(int i) {
	if(map_features[i])
		return;
	map_features[i] = find_feature_by_frame(i);
	if(map_features[i] && map_alternate[i])
		map_features[map_alternate[i]] = map_features[i];
}

static void update_building_feature_frames() {
	for(auto& e : bsdata<buildingi>()) {
		for(auto& tp : e.tiles) {
			if(map_features[tp.from])
				map_features[tp.to] = map_features[tp.from];
		}
	}
}

static void update_building_special(buildingn n, size_t count) {
	auto& ei = bsdata<buildingi>::elements[n];
	for(size_t i = 0; i < count; i++)
		map_features[ei.frames[0] + i] = BuildingHead;
}

void area_initialization() {
	initialize_alternate();
	memset(map_terrain, 0, sizeof(map_terrain));
	memset(map_features, 0, sizeof(map_features));
	for(auto i = 0; i < area_frame_maximum; i++) {
		map_terrain[i] = find_terrain_by_frame(i);
		add_feature_frame(i);
	}
	update_building_feature_frames();
	update_building_special(Turret, 8);
	update_building_special(RocketTurret, 8);
}