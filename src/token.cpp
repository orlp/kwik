#include "precompile.h"

#include "token.h"
#include "exception.h"
#include "grammar.h"


namespace kwik {
    std::string Token::as_str() {
        switch (type) {
        case 0: return "EOF";
        case KWIK_TOK_NL: return "\\n";
        case KWIK_TOK_OPEN_PAREN: return "(";
        case KWIK_TOK_CLOSE_PAREN: return ")";
        case KWIK_TOK_OPEN_BRACE: return "{";
        case KWIK_TOK_CLOSE_BRACE: return "}";
        case KWIK_TOK_COLON: return ":";
        case KWIK_TOK_SEMICOLON: return ";";
        case KWIK_TOK_EQUALS: return "=";
        case KWIK_TOK_LET: return "let";
        case KWIK_TOK_RETURN: return "return";
        case KWIK_TOK_NUM:
        case KWIK_TOK_NAME:
             return val;
        }

        throw InternalCompilerError(op::format("unknown token type {}", type));
    }
}
