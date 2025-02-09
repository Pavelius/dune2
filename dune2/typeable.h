#pragma once

template<typename T>
struct bsdata;

template<class T>
struct typeable {
	short unsigned	type;
	const T&		geti() const { return bsdata<T>::elements[type]; }
};
