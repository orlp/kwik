#ifndef KWIK_TOKEN_H
#define KWIK_TOKEN_H

#include <string>
#include <array>

#include "grammar.h"


namespace kwik {
    struct Token {
        // Required by LEMON.
        Token() = default;

        Token(int type, int line, int col)
        : type(type), line(line), col(col), val(nullptr) { }

        Token(int type, int line, int col, const std::string& val)
        : type(type), line(line), col(col), val(new std::string(val)) { }

        // We don't use a destructor because LEMON stores us in an union.
        void del() const noexcept {
            if (val) delete val;
            switch (type) {
            case KWIK_TOK_NUM:
                if (number.suffix) delete number.suffix;
                break;
            }
        }

        int type;
        int line;
        int col;
        // We don't use a smart pointer because LEMON stores our type in an union.
        std::string* val;

        union {
            struct {
                int base;
                bool floating;
                std::string* suffix;
            } number;
        };

        std::string as_str();
    };
    
}

#endif
