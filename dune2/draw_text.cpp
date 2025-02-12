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

#include "draw.h"

using namespace draw;

int draw::texth() {
	if(!font)
		return 0;
	return font->height;
}

int draw::textw(int sym) {
	if(!font || sym >= font->count)
		return 0;
	auto& f = font->get(sym);
	return f.sx - f.ox - 2;
}

void draw::glyph(int sym, unsigned flags) {
	// FONT3 - Monochrome.
	// FONT6, FONT8  - Three colors: 1 - main, 2 - right shadow, 3 - left shadow
	// FONT10 - Capital in other color. 15 - shadow, (12, 10, 8) Capital marked
	// FONT16 - Two colors: 1 - main, 5 - second main, 6 - part shadow
	image(caret.x, caret.y, font, sym, flags);
}