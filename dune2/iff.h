#pragma once

typedef const unsigned char* iffit; // Standart input type

// Get 32 bit big endian integer value from `p`
inline unsigned get32be(iffit p) { return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3]; }
// Get 24 bit big endian integer value from `p`
inline unsigned get24be(iffit p) { return (p[0] << 16) + (p[1] << 8) + p[2]; }
// Get 16 bit big endian integer value from `p`
inline unsigned get16be(iffit p) { return (p[0] << 8) + p[1]; }

// If is header name as `n` in data `p`
inline bool iff(const void* p, const char* n) { return *((unsigned*)p) == *((unsigned*)n); }
// If is header name as `n` and type `t` in data `p`
inline bool iff(const void* p, const char* n, const char* t) { return iff(p, n) && iff((iffit)p + 8, t); }
// Get lenght of chunk
inline unsigned iff_lenght(iffit p) { return get32be(p + 4); }
// Get children chunks
inline iffit iff_child(iffit p) { return p + 4 * 3; }
// Get data of chunk
inline iffit iff_data(iffit p) { return p + 4 * 2; }
// Get next chunk
inline iffit iff_next(iffit p) { auto n = iff_lenght(p); return iff_data(p) + n + (n % 2); }

const char* iff_name(iffit p); // Get name of chunk
const char* iff_type_name(iffit p); // Get type name of chunk

int iff_number(iffit p); // Get number from requisit chunk

void iff_print(iffit p, size_t lenght); // Print data structure




