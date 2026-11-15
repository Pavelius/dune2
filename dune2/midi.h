#pragma once

enum musicn : unsigned char;

struct musici {
	unsigned*		data;
	unsigned		size;
	short unsigned	division;
};
extern musici songs[];

extern bool music_enable;
extern musicn current_music;

void music_setvolume(short unsigned v);
void music_stop();
void music_update();