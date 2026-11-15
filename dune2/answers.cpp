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

#include "answers.h"
#include "rand.h"

bool show_interactive = true;
answers an;

static unsigned anhotkey(int index) {
	static char hotkeys[] = {
		'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
		'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
		'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
		'V', 'W', 'X', 'Y', 'Z'
	};
	if((size_t)index > sizeof(hotkeys) / sizeof(hotkeys[0]) - 1)
		index = sizeof(hotkeys) / sizeof(hotkeys[0]) - 1;
	return hotkeys[index];
}

int answers::compare(const void* v1, const void* v2) {
	return szcmp(((answers::element*)v1)->text, ((answers::element*)v2)->text);
}

void answers::addv(const void* value, const char* text, const char* format, unsigned key) {
	auto p = elements.add();
	p->value = value;
	p->text = sc.get();
	p->key = key;
	sc.addv(text, format);
	sc.addsz();
}

void answers::sort() {
	qsort(elements.data, elements.count, sizeof(elements.data[0]), compare);
}

void* answers::random() const {
	if(!elements.count)
		return 0;
	return (void*)elements.data[xrand() % elements.count].value;
}

const char* answers::getname(void* v) {
	for(auto& e : elements) {
		if(e.value == v)
			return e.text;
	}
	return 0;
}

void answers::clear() {
	elements.clear();
	sc.clear();
}

size_t answers::findvalue(const void* pv) const {
	for(auto& e : elements) {
		if(e.value == pv)
			return &e - elements.data;
	}
	return -1;
}

void answers::checkkeys() {
	auto index = 0;
	for(auto& e : elements) {
		if(!e.key) {
			if(!e.value)
				e.key = 27;
			else
				e.key = anhotkey(index);
		}
		index++;
	}
}