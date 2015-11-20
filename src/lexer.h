#ifndef KWIK_LEXER_H
#define KWIK_LEXER_H

#include <string>
#include <memory>

#include "parser.h"
#include "utf8cpp/utf8.h"


namespace kwik {
    class Lexer {
    public:
        Lexer(ParseState& s);
        Token get_token();

    private:
        Token lex_num();
        Token lex_ident();

        ParseState& s;
        int line, col;
        utf8::unchecked::iterator<const char*> it;
    };
}

#endif
