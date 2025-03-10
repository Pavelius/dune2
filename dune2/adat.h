/////////////////////////////////////////////////////////////////////////
// 
// Copyright 2024 Pavel Chistyakov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Add [[nodiscard]] thanks to https://github.com/wonderinglostsoul44

#pragma once

#include "slice.h"
#include "rand.h"

template<class T, size_t count_max = 128>
struct adat {
	typedef T data_type;
	size_t count;
	constexpr static size_t count_maximum = count_max;
	T data[count_max];
	adat() : count(0) {}
	constexpr const T& operator[](unsigned index) const { return data[index]; }
	constexpr T& operator[](unsigned index) { return data[index]; }
	explicit operator bool() const { return count != 0; }
	constexpr operator slice<T> () { return slice<T>(data, count);}
	T* add() { if(count < count_max) return data + (count++); return data; }
	void add(const T& e) { if(count < count_max) data[count++] = e; }
	void addu(const T& e) { if(!have(e)) add(e); }
	T* begin() { return data; }
	const T* begin() const { return data; }
	void clear() { count = 0; }
	T* end() { return data + count; }
	const T* end() const { return data + count; }
	const T* endof() const { return data + count_max; }
	[[nodiscard]] int find(const T t) const { for(auto& e : *this) if(e == t) return &e - data; return -1; }
	[[nodiscard]] T first() const { return count ? data[0] : T(); }
	[[nodiscard]] size_t size() const { return count; }
	[[nodiscard]] size_t capacity() const { return count_max; }
	[[nodiscard]] int indexof(const void* e) const { if(e >= data && e < data + count) return (T*)e - data; return -1; }
	bool have(const T t) const { for(auto& e : *this) if(e == t) return true; return false; }
	bool have(const void* element) const { return element >= data && element < (data + count); }
	T random() const { return count ? data[rand() % count] : T(); }
	void random(size_t new_count) { shuffle(); new_count = (count < new_count) ? count : new_count; count = new_count; }
	void remove(int index, int remove_count = 1) { if(index < 0) return; if(index<int(count - 1)) memcpy(data + index, data + index + 1, sizeof(data[0]) * (count - index - 1)); count--; }
	void remove(const T t) { remove(find(t), 1); }
	void shuffle() { zshuffle(data, count); }
	void top(size_t v) { if(count > v) count = v; }
};
