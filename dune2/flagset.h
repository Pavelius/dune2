#pragma once

#include "my_initialize_list.h"

class flagset {
	unsigned data;
public:
	constexpr flagset() : data(0) {}
	template<class T> constexpr flagset(const std::initializer_list<T>& source) : data(0) {
		for(auto n : source)
			data |= 1 << n;
	}
	constexpr explicit operator bool() const { return data != 0; }
	constexpr void clear() { data = 0; }
	constexpr bool is(short unsigned v) const { return (data & (1 << v)) != 0; }
	constexpr void remove(short unsigned v) { data &= ~(1 << v); }
	constexpr void set(short unsigned v) { data |= (1 << v); }
	constexpr void set(short unsigned v, bool apply) { if(apply) set(v); else remove(v); }
};


