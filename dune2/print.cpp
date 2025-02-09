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

#include "stringbuilder.h"
#include "print.h"

fnprintcallback print_proc;

void printv(const char* format) {
	if(print_proc)
		print_proc(format);
}

void printv(const char* format, const char* format_param) {
	char temp[4192]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	printv(temp);
}

void println() {
	printv("\n");
}

void print(const char* format, ...) {
	XVA_FORMAT(format);
	printv(format, format_param);
}

void println(const char* format, ...) {
	XVA_FORMAT(format);
	printv(format, format_param);
	println();
}

void printpad(const char* format, int maximum) {
	int count = zlen(format);
	printv(format);
	while(count < maximum) {
		count++;
		printv(" ");
	}
}