#ifndef KWIK_TOKEN_H
#define KWIK_TOKEN_H

#include <string>
#include <array>

#include "grammar.h"
#include "io.h"


namespace kwik {
    struct Token {
        Token(int type, SourceRef ref)
        : type(type), val(), ref(ref) { }

        Token(int type, const std::string& val, SourceRef ref)
        : type(type), val(val), ref(ref) { }

        ~Token() { }

        int type;
        std::string val;
        SourceRef ref;

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
