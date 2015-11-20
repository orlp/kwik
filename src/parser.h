#ifndef KWIK_PARSER_STATE_H
#define KWIK_PARSER_STATE_H

#include "token.h"
#include "ast.h"
#include "io.h"



namespace kwik {
    struct ParseState {
        ParseState(const Source& src)
            : src(src), num_errors(0), nested_paren(0) { }

        void error_with_context(const std::string& msg, int line, int col) {
            num_errors += 1;

            auto errmsg = op::format("{}:{}:{}: {}\n",
                                     src.name, line, col, msg);
            std::string indent(col - 1 + 4, ' ');
            errmsg += op::format("    {}\n{}^\n", src.lines[line - 1], indent);

            op::fprint(std::cout, errmsg);
        }

        const Source& src;
        int num_errors;
        int nested_paren;
        std::unique_ptr<ast::CompoundStmt> program;
    };

    void parse(const Source& src);
}



#endif
