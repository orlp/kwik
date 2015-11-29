#include "precompile.h"

#include "token.h"
#include "parser.h"
#include "lexer.h"

void* KwikParseAlloc(void* (*alloc_proc)(size_t));
void KwikParse(void* state, int token_id, kwik::Token* token_data, kwik::ParseState* s);
void KwikParseFree(void*, void(*free_proc)(void*));



namespace kwik {
    void parse(const Source& src) {
        auto parser = KwikParseAlloc(malloc);
        OP_SCOPE_EXIT { KwikParseFree(parser, free); };

        auto state = ParseState{src};
        auto lex = Lexer{state};

        while (true) {
            try {
                auto token = lex.get_token();
                KwikParse(parser, token.type, new Token(token), &state);
                if (token.type == 0) break;
            } catch (const CompilationError& e) {
                state.errors.emplace_back(e.clone());
            }
        }

        ast::Environment global_env(nullptr);
        try {
            state.program->check(global_env);
        } catch (const CompilationError& e) {
            state.errors.emplace_back(e.clone());
        }

        op::printf("Finished parse with {} error(s).\n", state.errors.size());
        for (auto& error : state.errors) {
            op::print(error->what());
        }
    }
}
