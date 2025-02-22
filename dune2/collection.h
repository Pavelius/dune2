#pragma once

#include "adat.h"
#include "bsdata.h"

// Add [[nodiscard]] thanks to https://github.com/wonderinglostsoul44

// Basic collection class
struct collectiona : adat<void*, 256> {
	typedef void* fngroup(const void* object);
	typedef int fncompare(const void* v1, const void* v2);
	void add(void* p) { if(p) *adat<void*, 256>::add() = p; }
	void add(const collectiona& source);
	void distinct();
	[[nodiscard]] void* first() const;
	void group(fngroup proc);
	void insert(int index, void* object);
	bool have(const void* object) { return indexof(object) != -1; }
	void match(fnvisible proc, bool keep);
	void match(fnallow proc, int param, bool keep);
	void match(const collectiona& source, bool keep);
	[[nodiscard]] void* random() const;
	[[nodiscard]] void* pick();
	void select(array& source);
	void select(array& source, fnvisible proc, bool keep);
	void select(array& source, fnallow proc, int param, bool keep);
	void shuffle();
	void sort(fngetname proc);
	void sort(fncompare proc);
	void top(int count);
};
template<typename T>
struct collection : collectiona {
	constexpr T* operator[](unsigned i) const { return (T*)data[i]; }
	constexpr operator slice<T*>() const { return slice<T*>((T**)data, count); }
	T** begin() const { return (T**)data; }
	T** end() const { return (T**)data + count; }
	T* first() const { return (T*)collectiona::first(); }
	T* pick() { return (T*)collectiona::pick(); }
	T* random() const { return (T*)collectiona::random(); }
	constexpr slice<T*> records() const { return {(T**)data, count}; }
	void select() { collectiona::select(bsdata<T>::source); }
	void select(fnvisible proc) { collectiona::select(bsdata<T>::source, proc, true); }
};