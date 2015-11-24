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


static std::array<unsigned char, 128> make_simple_tok_table() {
    std::array<unsigned char, 128> simple_tok_table = {{0}};
    simple_tok_table['{'] = KWIK_TOK_OPEN_BRACE;
    simple_tok_table['}'] = KWIK_TOK_CLOSE_BRACE;
    simple_tok_table['('] = KWIK_TOK_OPEN_PAREN;
    simple_tok_table[')'] = KWIK_TOK_CLOSE_PAREN;
    simple_tok_table[':'] = KWIK_TOK_COLON;
    simple_tok_table[';'] = KWIK_TOK_SEMICOLON;
    simple_tok_table['='] = KWIK_TOK_EQUALS;
    return simple_tok_table;
}
    
static auto simple_tok_table = make_simple_tok_table();

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

static auto jump_table = make_jump_table();




namespace kwik {
    Lexer::Lexer(ParseState& s)
        : s(s), line(1), col(1), it(s.src.code.data()) { }

    void Lexer::throw_unexpected_char(uint32_t c, int line, int col) {
        std::string errmsg = "unexpected character: '";
        utf8::append(c, std::back_inserter(errmsg));
        errmsg += "'";
        throw SyntaxError(errmsg, s.src.name, line, col, s.src.lines[line - 1]);
    }

    uint32_t Lexer::assert_ascii(uint32_t c, int line, int col) {
        if (c >= 128) throw_unexpected_char(c, line, col);
        return c;
    }

    Token Lexer::lex_num() {
        bool base = false;
        bool floating = false;
        int startcol = col;

        std::string value;
        auto n = *std::next(it);
        if (*it == '0' && (n == 'b' || n == 'o' || n == 'x')) {
            base = true;
            value += *it++;
            value += *it++;
            col += 2;
        }

        while (std::isdigit(assert_ascii(*it, line, col))) { value += *it++; ++col; }

        if (!base) {
            if (*it == '.') {
                floating = true;
                value += *it++; ++col;
            }
            
            while (std::isdigit(assert_ascii(*it, line, col))) { value += *it++; ++col; }
        }

        std::string suffix;
        size_t suffix_col = col;
        while (std::isalnum(assert_ascii(*it, line, col))) { suffix += *it++; ++col; }

        if (suffix.size()) {
            if (floating && suffix != "f32" && suffix != "f64") {
                throw SyntaxError("invalid float suffix '" + suffix + "'",
                                  s.src.name, line, suffix_col, s.src.lines[line - 1]);
            } else if (!floating && !int_suffixes_set.count(suffix)) {
                throw SyntaxError("invalid integer suffix '" + suffix + "'",
                                  s.src.name, line, suffix_col, s.src.lines[line - 1]);
            }
        }

        return {KWIK_TOK_NUM, line, startcol, value + suffix};
    }

    Token Lexer::lex_ident() {
        int startcol = col++;
        std::string ident(1, *it++);
        while (std::isalnum(assert_ascii(*it, line, col)) || *it == '_') { ident += *it++; ++col; }

        auto it = keywords.find(ident);
        if (it == keywords.end()) return {KWIK_TOK_NAME, line, startcol, ident};
        return {it->second, line, startcol};
    }

    Token Lexer::get_token() {
        while (true) {
            int startcol = col;
            uint32_t c = assert_ascii(*it, line, startcol);

            // For performance it's important that the order here matches the order of
            // the jump table defined above.
            using I = LexerJumpIndex;
            switch (jump_table[c]) {
            case I::ERROR:
                ++it; ++col;
                throw_unexpected_char(c, line, startcol);
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
                do { ++it; ++col; } while (*it && *it != '\n');
                break;
            case I::DIGIT:
                return lex_num();
            case I::ALPHA:
                return lex_ident();
            case I::DOT:
                if (std::isdigit(assert_ascii(*std::next(it), line, col + 1))) {
                    return lex_num();
                }
                
                ++it; ++col;
                throw_unexpected_char(c, line, startcol);
            case I::SIMPLE_TOK:
                ++it; ++col;
                return {simple_tok_table[c], line, startcol};
            }
        }
    }
}
