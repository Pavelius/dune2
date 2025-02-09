#pragma once

struct pointc {
	union {
		struct {
			char x, y;
		};
		unsigned short u;
	};
	constexpr pointc() : x(0), y(0) {}
	constexpr pointc(int x, int y) : x((char)x), y((char)y) {}
	constexpr pointc(unsigned short u) : u(u) {}
	constexpr operator unsigned short() const { return u; }
	constexpr void operator+=(const pointc v) { x += v.x; y += v.y; }
	constexpr void operator-=(const pointc v) { x -= v.x; y -= v.y; }
	constexpr pointc operator+(const pointc v) const { return {x + v.x, y + v.y}; }
	constexpr pointc operator-(const pointc v) const { return {x - v.x, y - v.y}; }
	int range(const pointc v) const;
};
