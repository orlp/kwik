#include "precompile.h"

#include <set>
#include <string>
#include <array>
#include <unordered_map>

#include "lexer.h"
#include "exception.h"
#include "grammar.h"
#include "parser.h"

static const std::unordered_map<std::string, int> keywords = {
    {"let", KWIK_TOK_LET}
};

static const std::set<std::string> int_suffixes_set = {
    "f32", "f64",
    "i8", "i16", "i32", "i64",
    "u8", "u16", "u32", "u64",
    "i"
};


static std::array<int, 128> make_simple_tok_table() {
    std::array<int, 128> simple_tok_table = {{0}};
    simple_tok_table['{'] = KWIK_TOK_OPEN_BRACE;
    simple_tok_table['}'] = KWIK_TOK_CLOSE_BRACE;
    simple_tok_table['('] = KWIK_TOK_OPEN_PAREN;
    simple_tok_table[')'] = KWIK_TOK_CLOSE_PAREN;
    simple_tok_table[':'] = KWIK_TOK_COLON;
    simple_tok_table[';'] = KWIK_TOK_SEMICOLON;
    simple_tok_table['='] = KWIK_TOK_EQUALS;
    return simple_tok_table;
}
    
static std::array<int, 128> simple_tok_table = make_simple_tok_table();

enum class LexerJumpIndex : unsigned char {
    ERROR = 0,
    NULL_EOF,
    SPACE,
    NEWLINE,
    COMMENT,
    DIGIT,
    ALPHA,
    DOT,
    SIMPLE_TOK,
};

static std::array<LexerJumpIndex, 128> make_jump_table() {
    using I = LexerJumpIndex;
    std::array<LexerJumpIndex, 128> jump_table;

    for (int c = 0; c < 128; ++c) {
        if (simple_tok_table[c]) jump_table[c] = I::SIMPLE_TOK;
        else if (std::isdigit(c)) jump_table[c] = I::DIGIT;
        else if (std::isalpha(c)) jump_table[c] = I::ALPHA;
        else jump_table[c] = I::ERROR;
    }

    jump_table['_'] = I::ALPHA;
    jump_table[0] = I::NULL_EOF;
    jump_table[' '] = I::SPACE;
    jump_table['\n'] = I::NEWLINE;
    jump_table['#'] = I::COMMENT;
    jump_table['.'] = I::DOT;
    return jump_table;
}

static std::array<LexerJumpIndex, 128> jump_table = make_jump_table();




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

        while (std::isdigit(*it)) { value += *it++; ++col; }

        if (!base) {
            if (*it == '.') {
                floating = true;
                value += *it++; ++col;
            }
            
            while (std::isdigit(*it)) { value += *it++; ++col; }
        }

        std::string suffix;
        size_t suffix_col = col;
        while (std::isalnum(*it)) { suffix += *it++; ++col; }

        if (suffix.size()) {
            if (floating && suffix != "f32" && suffix != "f64") {
                throw SyntaxError("invalid float suffix '" + suffix + "'",
                                  filename, line, suffix_col);
            } else if (!floating && !int_suffixes_set.count(suffix)) {
                throw SyntaxError("invalid integer suffix '" + suffix + "'",
                                  filename, line, suffix_col);
            }
        }

        return {KWIK_TOK_NUM, line, startcol, value + suffix};
    }

    Token Lexer::lex_ident(const ParseState& s) {
        int startcol = col++;
        std::string ident(1, *it++);
        while (std::isalnum(*it) || *it == '_') { ident += *it++; ++col; }

        auto it = keywords.find(ident);
        if (it == keywords.end()) return {KWIK_TOK_NAME, line, startcol, ident};
        return {it->second, line, startcol};
    }

    Token Lexer::get_token(const ParseState& s) {
        while (true) {
            int startcol = col;
            int c = *it;
            if (c < 0 || c >= 128) {
                throw kwik::SyntaxError(op::format("unexpected character: '{}'", c),
                                        filename, line, startcol);
            }

            // For performance it's important that the order here matches the order of
            // the jump table defined above.
            using I = LexerJumpIndex;
            switch (jump_table[c]) {
            case I::ERROR:
                ++it; ++col;
                throw kwik::SyntaxError(op::format("unexpected character: '{}'", c),
                                        filename, line, startcol);
            case I::NULL_EOF:
                ++it; ++col;
                return {0, line, startcol};
            case I::SPACE:
                ++it; ++col;
                break;
            case I::NEWLINE:
                ++it; col = 1; ++line;
                if (s.nested_paren == 0) return {KWIK_TOK_NL, line-1, startcol};
                break;
            case I::COMMENT:
                while (*it && *it != '\n') { it++; col++; }
                break;
            case I::DIGIT:
                return lex_num(s);
            case I::ALPHA:
                 return lex_ident(s);
            case I::DOT:
                if (std::isdigit(it[0])) {
                    return lex_num(s);
                }
                
                throw kwik::SyntaxError(op::format("unexpected character: '{}'", c),
                                        filename, line, startcol);
            case I::SIMPLE_TOK:
                ++it; ++col;
                return {simple_tok_table[c], line, startcol};
            }
        }
    }
}
