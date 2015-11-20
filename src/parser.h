#ifndef KWIK_PARSER_STATE_H
#define KWIK_PARSER_STATE_H

#include "token.h"
#include "ast.h"

namespace kwik {
    struct ParseState {
        ParseState(std::shared_ptr<std::string> src, std::string filename)
        : src(src), filename(filename), num_errors(0), nested_paren(0) {
            const char* it = src->data();
            const char* line_start = it;
            while (*it) {
                if (*it == '\n') {
                    lines.emplace_back(line_start, it);
                    line_start = it + 1;
                }
                ++it;
            }

            if (it != line_start) lines.emplace_back(line_start, it);
        }

        std::shared_ptr<std::string> src;
        std::vector<std::string> lines;
        std::string filename;
        int num_errors;
        int nested_paren;
        std::unique_ptr<ast::CompoundStmt> program;
    };

    void parse(std::shared_ptr<std::string> src, const std::string& filename);
}



#endif
