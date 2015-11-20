#ifndef KWIK_LEXER_H
#define KWIK_LEXER_H

#include <string>
#include <memory>

#include "parser.h"


namespace kwik {
    class Lexer {
    public:
        Lexer(std::shared_ptr<std::string> src, const std::string& filename);
        Token get_token(ParseState& s);

    private:
        Token lex_num(ParseState& s);
        Token lex_ident(ParseState& s);

        std::shared_ptr<std::string> src;
        std::string filename;
        int line;
        int col;
        const char* it;
        const char* end;
    };
}

#endif
