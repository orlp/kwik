#include "precompile.h"

#include "token.h"
#include "exception.h"
#include "grammar.h"


namespace kwik {
    std::string Token::as_str() {
        switch (type) {
        case KWIK_TOK_NL: return "\\n";
        case KWIK_TOK_OPEN_PAREN: return "(";
        case KWIK_TOK_CLOSE_PAREN: return ")";
        case KWIK_TOK_OPEN_BRACE: return "{";
        case KWIK_TOK_CLOSE_BRACE: return "}";
        case KWIK_TOK_SEMICOLON: return ";";
        case KWIK_TOK_NUM: return *val;
        }

        throw InternalCompilerError("unknown token type");
    }
}
