#ifndef KWIK_LEXER_H
#define KWIK_LEXER_H

#include "parser.h"


namespace kwik {
    class Lexer {
    public:
        Lexer(const std::string& src, const std::string& filename);
        Token get_token(const ParseState& s);

    private:
        Token lex_num(const ParseState& s);

        std::string src;
        std::string filename;
        int line;
        int col;
        const char* it;
        const char* end;
    };
}

#endif
