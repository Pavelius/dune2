inline unsigned get32be(const unsigned char* p) { return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3]; }
inline unsigned get24be(const unsigned char* p) { return (p[0] << 16) + (p[1] << 8) + p[2]; }
inline unsigned get16be(const unsigned char* p) { return (p[0] << 8) + p[1]; }

inline bool iff(const void* p, const char* n) { return *((unsigned*)p) == *((unsigned*)n); }
inline bool iff(const void* p, const char* n, const char* t) { return iff(p, n) && iff((unsigned char*)p + 8, t); }

inline unsigned iff_lenght(const unsigned char* p) { return get32be(p + 4); }
inline const unsigned char* iff_child(const unsigned char* p) { return p + 4 * 3; }
inline const unsigned char* iff_data(const unsigned char* p) { return p + 4 * 2; }
inline const unsigned char* iff_next(const unsigned char* p) { auto n = iff_lenght(p); return iff_data(p) + n + (n % 2); }