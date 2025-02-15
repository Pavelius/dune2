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
	const char*		text;
	color			disappear;
	constexpr explicit operator bool() const { return id != NONE; }
	bool			is(videof v) const { return flags.is(v); }
};
extern unsigned long start_video;

void reset_video_time();
void video_play(const slice<videoi>& source);
void video_dump(const char* url, const slice<videoi>& source, unsigned long time);