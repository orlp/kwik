#ifndef KWIK_TOKEN_H
#define KWIK_TOKEN_H

#include <string>
#include <array>


namespace kwik {
    struct Token {
        // Required by LEMON.
        Token() = default;

        Token(int type, int line, int col)
        : type(type), line(line), col(col), val(nullptr) { }

        Token(int type, int line, int col, const std::string& val)
        : type(type), line(line), col(col), val(new std::string(val)) { }

        int type;
        int line;
        int col;
        // We don't use shared_ptr because LEMON stores our type in an union.
        std::string* val;


        std::string as_str();
    };
    
}

#endif
