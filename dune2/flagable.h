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

#pragma once

// Abstract flag set
template<unsigned N, typename T = unsigned char>
class flagable {
	static constexpr unsigned s = sizeof(T) * 8;
	T data[N];
public:
	constexpr flagable() : data{0} {}
	constexpr explicit operator bool() const { for(auto e : data) if(e) return true; return false; }
	constexpr void add(const flagable<N, T>& v) { for(unsigned i = 0; i < N; i++) data[i] |= v.data[i]; }
	constexpr void clear() { for(auto& v : data) v = 0; }
	constexpr bool is(short unsigned v) const { return (data[v / s] & (1 << (v % s))) != 0; }
	constexpr void remove(short unsigned v) { data[v / s] &= ~(1 << (v % s)); }
	constexpr void set(short unsigned v) { data[v / s] |= 1 << (v % s); }
	constexpr void set(short unsigned v, bool apply) { if(apply) set(v); else remove(v); }
};
// Abstract flag set partial
template<typename T>
class flagable<1, T> {
	static constexpr unsigned s = sizeof(T) * 8;
	T data;
public:
	constexpr flagable() : data(0) {}
	constexpr flagable(T data) : data(data) {}
	constexpr explicit operator bool() const { return data != 0; }
	constexpr void add(const flagable<1, T>& v) { data |= v.data; }
	constexpr void clear() { data = 0; }
	constexpr bool is(short unsigned v) const { return (data & (1 << v)) != 0; }
	constexpr void remove(short unsigned v) { data &= ~(1 << v); }
	constexpr void set(short unsigned v) { data |= (1 << v); }
	constexpr void set(short unsigned v, bool apply) { if(apply) set(v); else remove(v); }
};
typedef flagable<1, unsigned long long> flag64;
typedef flagable<1, unsigned> flag32;
typedef flagable<1, unsigned short> flag16;
typedef flagable<1, unsigned char> flag8;
