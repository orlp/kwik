#include "precompile.h"

#include <string>
#include <memory>
#include <cstdio>
#include "libop/op.h"
#include "utf8cpp/utf8.h"

#include "io.h"
#include "exception.h"


namespace kwik {
    static void error_with_context(std::string msg, std::string name,
                                   int line, int col,
                                   const std::vector<std::string>& lines) {
        assert(line > 0 && size_t(line) <= lines.size());
        auto errmsg = op::format("{}:{}:{}: {}\n", name, line, col, msg);
        std::string indent(col - 1 + 4, ' ');
        errmsg += op::format("    {}\n{}^\n", lines[line - 1], indent);
        op::fprint(std::cout, errmsg);
    }


    static std::string read_full_stream(std::FILE* file) {
        std::string result;
        std::array<char, 4096> buf;
        while (true) {
            auto bytes_read = std::fread(buf.data(), 1, buf.size(), file);
            if (std::ferror(file)) {
                throw FilesystemError(std::strerror(errno));
            }
            
            if (!bytes_read) break;
            result.insert(result.end(), buf.begin(), buf.begin() + bytes_read);
        }

        return result;
    }


    constexpr int Source::NULL_BYTES_APPENDED;

    Source read_stdin() {
        return make_source(read_full_stream(stdin), "<stdin>");
    }

    Source read_file(const std::string& filename) {
        auto file = std::fopen(filename.c_str(), "r");
        if (!file) throw kwik::FilesystemError(std::strerror(errno));
        OP_SCOPE_EXIT { std::fclose(file); };
        return make_source(read_full_stream(file), filename);
    }


    Source make_source(const std::string& src, const std::string& name) {
        auto cbegin = src.data();
        auto cend = src.data() + src.size();
        if (utf8::starts_with_bom(cbegin, cend)) cbegin += 3;

        utf8::iterator<decltype(cbegin)> it{cbegin, cbegin, cend};
        utf8::iterator<decltype(cbegin)> end{cend, cbegin, cend};

        std::string code; code.reserve(src.size());
        auto code_append = std::back_inserter(code);

        std::vector<std::string> lines;
        auto line_start = code.begin();
        int line = 1;
        int col = 1;
        while (it != end) {
            auto c = *it++;
            // Don't allow null bytes in source.
            if (c == 0) {
                lines.emplace_back(line_start, code.end());
                error_with_context("syntax error: null character encountered",
                                   name, line, col, lines);
                throw StopCompilation();
            }

            // Translate \r and \r\n to \n.
            if (c == '\r' || c == '\n') {
                lines.emplace_back(line_start, code.end());
                utf8::append('\n', code_append);
                line_start = code.end();
                if (c == '\r' && it != end && *it == '\n') ++it;
                line++; col = 1;
            } else {
                utf8::append(c, code_append);
                col++;
            }
        }

        // Append final line.
        if (line_start != code.end()) {
            lines.emplace_back(line_start, code.end());
        }

        // Append null bytes for lexer.
        for (int i = 0; i < Source::NULL_BYTES_APPENDED; ++i) code.push_back(0);

        return {name, std::move(code), std::move(lines)};
    }
}
