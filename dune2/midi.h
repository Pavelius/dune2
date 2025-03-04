#pragma once

bool midi_busy();
void midi_music_stop();
void midi_open();
bool midi_play(const char* file_name);
void midi_play_raw(void* mid_data);
void midi_sleep(unsigned milliseconds);