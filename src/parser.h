#ifndef KWIK_PARSER_STATE_H
#define KWIK_PARSER_STATE_H

#include "token.h"

namespace kwik {
    struct ParseState {
        ParseState(std::string filename)
        : filename(filename), num_errors(0), nested_paren(0) { }

        std::string filename;
        int num_errors;
        int nested_paren;
    };

    void parse(const std::string& src, const std::string& filename);
}



#endif
