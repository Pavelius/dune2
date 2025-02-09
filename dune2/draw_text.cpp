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

namespace {
struct fnt {
	unsigned short	Size; // File size.
	unsigned char	DataFormat; // Data format. 0x00 for v3, 0x02 for v4.
	unsigned char	SigByte1; // Signature byte.Always 0x05.
	unsigned char	SigByte2; // Signature byte.Always 0x0E.
	unsigned char	SigByte3; // Signature byte.Always 0x00.
	unsigned short	OffsetsListOffset; // Offset of the array of data offsets.
	unsigned short	WidthsListOffset; // Offset of the array of symbol widths
	unsigned short	FontOffsetStart; // Start address of the font data. Unused in v3, since the addresses in the OffsetsListOffset array are absolute. In v4, offsets are relative to this value.
	unsigned short	HeightsListOffset; // Offset of the array containing the symbol heights and Y - offsets.
	unsigned short	Unknown; // Always 0x1012.
	unsigned char	Unknown1; // Unknown.Always 0x00.
	unsigned char	NrOfChars; // Number of characters. Actually, this is the byte value of the last character in the list, so the real number of characters is this value plus one.
	unsigned char	height; // Overall font symbols maximum height, in pixels.
	unsigned char	width; // Overall font symbols maximum width, in pixels.
	unsigned char* ptr(int v) const { return (unsigned char*)this + v; }
};
}

int glyph_offset_width = -2;

int draw::texth() {
	if(!font)
		return 0;
	return ((fnt*)font)->height;
}

int draw::textw(int sym) {
	if(!font)
		return 0;
	auto f = (fnt*)font;
	auto glyph_width = (unsigned char*)f->ptr(f->WidthsListOffset);
	return glyph_offset_width + glyph_width[(unsigned char)sym];
}

void draw_glyph_zoomed(int sym, int zoom) {
}

static void paint_glyph_shadowed(int x, int y, int y2, int width, unsigned char* line) {
	auto scan_line = (width * 4 + 7) / 8;
	auto main_fore = fore;
	auto half_fore = fore_stroke.mix(fore, 128 + 64 + 32);
	for(; y < y2; y++) {
		for(int w = 0; w < width; w++) {
			auto index = line[w / 2];
			if(w & 1)
				index >>= 4;
			else
				index &= 0x0F;
			switch(index) {
			case 1: fore = main_fore; break;
			case 2: fore = fore_stroke; break;
			case 3: fore = half_fore; break;
			case 5: fore = color(200, 200, 50); break;
			case 8: fore = colors::yellow; break;
			case 9: fore = colors::green; break;
			case 10: fore = colors::blue; break;
			case 12: fore = colors::red; break;
			case 14: fore = colors::gray; break;
			default: continue;
			}
			pixel(caret.x + w, y);
		}
		line += scan_line;
	}
}

void draw::glyph(int sym, unsigned flags) {
	auto push_fore = fore;
	auto f = (fnt*)font;
	auto glyph_data = (unsigned short*)f->ptr(f->OffsetsListOffset);
	auto glyph_width = (unsigned char*)f->ptr(f->WidthsListOffset);
	auto glyph_height = (unsigned char*)f->ptr(f->HeightsListOffset);
	auto y1 = caret.y + glyph_height[sym * 2 + 0];
	paint_glyph_shadowed(caret.y, y1, y1 + glyph_height[sym * 2 + 1], glyph_width[sym], f->ptr(glyph_data[sym]));
	fore = push_fore;
}