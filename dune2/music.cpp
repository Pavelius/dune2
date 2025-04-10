#include "bsdata.h"
#include "midi.h"
#include "music.h"
#include "thread.h"

BSDATAC(musici, 512)

static void* current_music;

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

void music_play(void* new_music) {
	if(current_music == new_music)
		return;
	current_music = new_music;
	midi_music_stop();
	if(midi_busy())
		music_tread.join();
	if(current_music) {
		music_tread.close();
		music_tread.start(midi_play_raw, current_music);
	}
}

void music_check_current() {
	if(midi_busy())
		return;
	current_music = 0;
}

void song_play(const char* id) {
	music_play(song_get(id));
}