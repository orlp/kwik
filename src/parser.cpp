#include "precompile.h"

#include "parser.h"
#include "lexer.h"

void* KwikParseAlloc(void* (*alloc_proc)(size_t));
void KwikParse(void* state, int token_id, kwik::Token token_data, kwik::ParseState* s);
void KwikParseFree(void*, void(*free_proc)(void*));



namespace kwik {
    void parse(const std::string& src, const std::string& filename) {
        auto parser = KwikParseAlloc(malloc);
        OP_SCOPE_EXIT { KwikParseFree(parser, free); };

        kwik::ParseState state {filename};
        auto lex = kwik::Lexer(src, filename);
        while (true) {
            auto token = lex.get_token(state);
            KwikParse(parser, token.type, token, &state);
            if (token.type == 0) break;
        }
    }
}
