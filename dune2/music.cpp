#include "bsdata.h"
#include "midi.h"
#include "music.h"
#include "thread.h"

static void* current_music;
static volatile bool thread_music_player;

BSDATAD(musici)

io::thread music_tread;

void music_clear() {
	manager_clear(bsdata<musici>::source);
}

void music_initialize() {
	manager_initialize(bsdata<musici>::source, "music", "*.mid");
}

void* song_get(const char* id) {
	return manager_get(bsdata<musici>::source, id, "mid");
}

static void music_play_background(void* music_data) {
	thread_music_player = true;
	midi_play_raw(music_data);
	thread_music_player = false;
}

bool music_played() {
	return thread_music_player;
}

void music_play(void* new_music) {
	if(current_music == new_music)
		return;
	current_music = new_music;
	midi_music_stop();
	while(midi_busy())
		midi_sleep(10);
	if(new_music) {
		music_tread.close();
		music_tread.start(music_play_background, new_music);
	}
}

void song_play(const char* id) {
	music_play(song_get(id));
}