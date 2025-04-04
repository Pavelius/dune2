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

enum seek_flags { SeekSet, SeekCur, SeekEnd };
enum stream_flags {
	StreamRead = 1,
	StreamWrite = 2,
	StreamText = 4,
};
enum class codepage { No, W1251, UTF8, U16BE, U16LE };

namespace io {
// Abstract bi-stream interface
struct stream {
	stream&				operator<<(const char* t); // Post text string into stream data in correct coding.
	stream&				operator<<(const int n); // Post number as string into stream data in correct coding.
	unsigned char		get();
	unsigned short		getLE16();
	unsigned			getLE32();
	virtual int			read(void* result, int count) = 0;
	template<class T> void read(T& object) { read(&object, sizeof(object)); }
	virtual int			seek(int count, int rel = SeekCur) { return 0; };
	int					tell() { return seek(0, SeekCur); };
	virtual int			write(const void* result, int count) = 0;
	template<class T> void write(const T& e) { write(&e, sizeof(e)); }
};
struct file : stream {
	struct find {
		find(const char* url);
		~find();
		const char*	name();
		const char*	fullname(char* result);
		void			next();
		operator bool() const { return handle != 0; }
	private:
		char			path[261];
		char			reserved[512];
		void*			handle;
		void*			handle_ent;
	};
	file();
	file(const char* url, unsigned feats = StreamRead);
	~file();
	operator bool() const { return handle != 0; }
	void				close();
	bool				create(const char* url, unsigned feats);
	static bool			exist(const char* url);
	static char*		getdir(char* url, int size);
	static char*		getmodule(char* url, int size);
	static bool			getfullurl(const char* short_url, char* url, int size);
	static bool			makedir(const char* url);
	int					read(void* result, int count) override;
	static bool			remove(const char* url);
	int					seek(int count, int rel) override;
	static bool			setdir(const char* url);
	int					write(const void* result, int count) override;
private:
	void*				handle;
};
}

unsigned szget(const char** input, codepage code);

char* szput(char* output, unsigned value, codepage code);
char* loadt(const char* url, int* size = 0); // Load text file and decode it to system codepage.

void* loadb(const char* url, int* size = 0, int additional_bytes_alloated = 0); // Load binary file. To free memory use delete char[].
void szencode(char* output, int output_count, codepage output_code, const char* input, int input_count, codepage input_code);

bool isfolderpresent(const char* p);
const char* szext(const char* path);
const char* szfname(const char* path);
const char* szfnamewe(char* result, const char* name);
const char* szfpath(char* result, const char* url);
const char* szurl(char* temp, size_t size, const char* url, const char* folder, const char* subfolder, const char* name, const char* ext);

int get_file_number(const char* url, const char* mask);
