#ifndef KWIK_LEXER_H
#define KWIK_LEXER_H

#include <string>
#include <memory>
#include "utf8cpp/utf8.h"

#include "parser.h"
#include "token.h"
#include "io.h"


namespace kwik {
    class Lexer {
    public:
        Lexer(ParseState& s);
        Token get_token();

    private:
        SourceRef getref(size_t line, size_t col);
        void throw_unexpected_char(uint32_t c, size_t line, size_t col);
        Token lex_num();
        Token lex_ident();

        ParseState& s;
        size_t line, col;
        utf8::unchecked::iterator<const char*> it;
    };
}

#endif
