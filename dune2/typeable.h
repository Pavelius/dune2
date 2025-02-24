#pragma once

template<typename T>
struct bsdata;

template<typename T, typename V>
struct typeable {
	V				type;
	const T&		geti() const { return bsdata<T>::elements[type]; }
	const char*		getname() const { return geti().getname(); }
};
