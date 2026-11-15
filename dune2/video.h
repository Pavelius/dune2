#pragma once

#include "slice.h"
#include "flagable.h"

enum resid : unsigned char;

enum videof : unsigned char {
	Appearing, Disappearing, ContinueToNext, Repeated,
};
struct videoi {
	resid			id;
	unsigned long	time_lenght, frame_rate;
	flag8			flags;
	unsigned		subtitle = - 1;
	constexpr explicit operator bool() const { return id != (resid)0; }
	bool			is(videof v) const { return flags.is(v); }
};
extern unsigned long start_video;

void reset_video_time();
void video_play(const slice<videoi>& source, const char** subtitles);
void video_dump(const char* url, const slice<videoi>& source, unsigned long time);