#pragma once

#include "manager.h"

struct musici : manageri {
};

extern bool music_disabled;

void music_clear();
void music_initialize();
void music_play(void* mid_raw_data);
bool music_played();
void song_play(const char* id);

void* song_get(const char* id);
