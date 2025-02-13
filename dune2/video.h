#pragma once

#include "resid.h"

struct videoi {
	resid			id;
	unsigned long	delay, delay_total;
	const char*		music_start;
	constexpr explicit operator bool() const { return id != NONE; }
};
void play_video(const videoi* source);