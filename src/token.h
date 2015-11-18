#ifndef KWIK_TOKEN_H
#define KWIK_TOKEN_H

#include <string>
#include <array>


namespace kwik {
    struct Token {
        int type;
        int line;
        int col;
        // We don't use shared_ptr because LEMON stores our type in an union.
        std::string* val;

        std::string as_str();
    };
    
}

#endif
