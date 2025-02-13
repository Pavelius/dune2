#pragma once

#include "resid.h"
#include "slice.h"
#include "flagable.h"

enum videof : unsigned char {
	Appearing, Disappearing, ContinueToNext, Repeated,
};
struct videoi {
	resid			id;
	unsigned long	time_lenght, frame_rate;
	flag8			flags;
	const char*		music_start;
	const char*		text;
	constexpr explicit operator bool() const { return id != NONE; }
	bool			is(videof v) const { return flags.is(v); }
};
extern unsigned long start_video;

void play_video(const slice<videoi>& source);
void reset_video_time();