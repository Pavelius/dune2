#pragma once

enum soundn : unsigned char;

struct soundi {
	unsigned*		data;
	unsigned		size;
	short unsigned	division;
};
extern soundi songs[];

extern bool music_enable;
extern soundn current_music;

void music_setvolume(short unsigned v);
void music_stop();
void music_update();