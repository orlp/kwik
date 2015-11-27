#ifndef KWIK_TOKEN_H
#define KWIK_TOKEN_H

#include <string>
#include <array>

#include "grammar.h"


namespace kwik {
    struct Token {
        Token(int type, int line, int col)
        : type(type), line(line), col(col), val() { }

        Token(int type, int line, int col, const std::string& val)
        : type(type), line(line), col(col), val(val) { }

        ~Token() { }

        int type;
        int line;
        int col;
        std::string val;

        union {
            struct {
                int base;
                bool floating;
                int suffix_len;
            } number;
        };

        std::string as_str();
    };
    
}

#endif
