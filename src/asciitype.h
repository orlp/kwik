#ifndef KWIK_ASCIITYPE_H
#define KWIK_ASCIITYPE_H

// <cctype>'s functions are locale-aware and have undefined behavior for large codepoints.
// So we use our own ASCII versions.

namespace kwik {
#define ASCIIRANGE(c, start, len) (char32_t((c) - (start)) < char32_t(len))
    inline bool aislower(char32_t c)  { return ASCIIRANGE(c, 0x61, 26); }
    inline bool aisupper(char32_t c)  { return ASCIIRANGE(c, 0x41, 26); }
    inline bool aisdigit(char32_t c)  { return ASCIIRANGE(c, 0x30, 10); }
    inline bool aisbdigit(char32_t c) { return ASCIIRANGE(c, 0x30,  2); }
    inline bool aisodigit(char32_t c) { return ASCIIRANGE(c, 0x30,  8); }
    inline bool aisxdigit(char32_t c) { return ASCIIRANGE(c | 32, 0x61, 6) || aisdigit(c); }
    inline bool aisalpha(char32_t c)  { return ASCIIRANGE(c | 32, 0x61, 26); }
    inline bool aisprint(char32_t c)  { return ASCIIRANGE(c, 0x20, 95); } 
    inline bool aisgraph(char32_t c)  { return ASCIIRANGE(c, 0x21, 94); }
    inline bool aisspace(char32_t c)  { return ASCIIRANGE(c, 0x9, 5) || c == 0x20; }
    inline bool aisalnum(char32_t c)  { return aisalpha(c) || aisdigit(c); }
    inline bool aispunct(char32_t c)  { return !aisalnum(c) && aisgraph(c); } 
    inline bool aisblank(char32_t c)  { return c == 0x20 || c ==  0x9; } 
    inline bool aiscntrl(char32_t c)  { return c  < 0x20 || c == 0x7f; } 
    inline char32_t atolower(char32_t c) { return isupper(c) ? c + 0x20 : c; } 
    inline char32_t atoupper(char32_t c) { return islower(c) ? c - 0x20 : c; } 
#undef ASCIIRANGE
}

#endif
