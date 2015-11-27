#ifndef KWIK_PARSER_STATE_H
#define KWIK_PARSER_STATE_H

#include "ast.h"
#include "io.h"
#include "exception.h"



namespace kwik {
    struct ParseState {
        ParseState(const Source& src)
            : src(src), nested_paren(0), program(nullptr) { }

        // void error_with_context(const std::string& msg, int line, int col) {
        //     assert(line - 1 >= 0);
        //     assert(size_t(line - 1) < src.lines.size());

        //     auto errmsg = op::format("{}:{}:{}: {}\n",
        //                              src.name, line, col, msg);
        //     std::string indent(col - 1 + 4, ' ');
        //     errmsg += op::format("    {}\n{}^\n", src.lines[line - 1], indent);
        //     op::fprint(std::cout, errmsg);
        // }

        const Source& src;
        int nested_paren;
        std::unique_ptr<ast::CompoundStmt> program;
        std::vector<std::unique_ptr<CompilationError>> errors;
    };

    void parse(const Source& src);
}



#endif
