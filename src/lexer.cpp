#include "precompile.h"
#include "lexer.h"
#include "exception.h"
#include "grammar.h"
#include "parser.h"


static std::array<int, 128> make_ch_to_tok() {
    std::array<int, 128> ch_to_tok = {0};
    ch_to_tok['{'] = KWIK_TOK_OPEN_BRACE;
    ch_to_tok['}'] = KWIK_TOK_CLOSE_BRACE;
    ch_to_tok['('] = KWIK_TOK_OPEN_PAREN;
    ch_to_tok[')'] = KWIK_TOK_CLOSE_PAREN;
    ch_to_tok[';'] = KWIK_TOK_SEMICOLON;
    return ch_to_tok;
}
    
static std::array<int, 128> ch_to_tok = make_ch_to_tok();

static const std::set<std::string> int_suffixes_set = {
    "f32", "f64",
    "i8", "i16", "i32", "i64",
    "u8", "u16", "u32", "u64",
    "i"
};




namespace kwik {
    Lexer::Lexer(const std::string& src, const std::string& filename)
    : src(src), filename(filename), line(1), col(1) {
        this->src.push_back(0);
        this->src.push_back(0);
        this->src.push_back(0);
        it = src.data();
        end = src.data() + src.size();
    }

    Token Lexer::lex_num(const ParseState& s) {
        bool base = false;
        bool floating = false;
        int startcol = col;

        std::string value;
        if (*it == '0' && (it[1] == 'b' || it[1] == 'o' || it[1] == 'x')) {
            base = true;
            value += *it++;
            value += *it++;
            col += 2;
        }

        while (std::isdigit(*it)) {
            value += *it++; ++col;
        }

        if (!base) {
            if (*it == '.') {
                floating = true;
                value += *it++; ++col;
            }
            
            while (std::isdigit(*it)) {
                value += *it++; ++col;
            }
        }

        std::string suffix;
        size_t suffix_col = col;
        while (std::isalnum(*it)) {
            suffix += *it++; ++col;
        }

        if (suffix.size()) {
            if (floating && suffix != "f32" && suffix != "f64") {
                throw SyntaxError(
                    "invalid float suffix '" + suffix + "'",
                    filename, line, suffix_col
                );
            } else if (!floating && !int_suffixes_set.count(suffix)) {
                throw SyntaxError(
                    "invalid integer suffix '" + suffix + "'",
                    filename, line, suffix_col
                );
            }
        }

        return {KWIK_TOK_NUM, line, startcol, new std::string(value + suffix)};
    }

    Token Lexer::get_token(const ParseState& s) {
        while (true) {
            int startcol = col++;
            char c = *it++;
            switch (c) {
            case 0: return {0, line, startcol, 0};
            case ' ': break;
            case '\n':
                col = 1;
                ++line;
                if (s.nested_paren == 0) return {KWIK_TOK_NL, line-1, startcol, 0};
                break;
            case '#':
                // Comment, skip up to (but not including) newline.
                while (*it && *it != '\n') { it++; col++; }
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                --it; --col;
                return lex_num(s);
            case '.':
                if (std::isdigit(it[0])) {
                    --it; --col;
                    return lex_num(s);
                }

                // Fallthrough.
            default:
                if (c >= 0 && c < 128 && ch_to_tok[int(c)]) {
                    return {ch_to_tok[int(c)], line, startcol, 0};
                }

                throw kwik::SyntaxError(op::format("unexpected character: '{}'", c),
                                        filename, line, startcol);
            }
        }
    }
}
