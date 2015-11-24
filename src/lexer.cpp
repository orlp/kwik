#include "precompile.h"

#include <set>
#include <string>
#include <array>
#include <unordered_map>

#include "lexer.h"
#include "exception.h"
#include "grammar.h"
#include "parser.h"
#include "asciitype.h"





static const std::unordered_map<std::string, int> keywords = {
    {"let", KWIK_TOK_LET},
    {"return", KWIK_TOK_RETURN}
};

static const std::set<std::string> int_suffixes_set = {
    "i8", "i16", "i32", "i64",
    "u8", "u16", "u32", "u64",
    "i"
};


static std::array<unsigned char, 128> make_simple_tok_table() {
    std::array<unsigned char, 128> simple_tok_table = {{0}};
    simple_tok_table[U'{'] = KWIK_TOK_OPEN_BRACE;
    simple_tok_table[U'}'] = KWIK_TOK_CLOSE_BRACE;
    simple_tok_table[U'('] = KWIK_TOK_OPEN_PAREN;
    simple_tok_table[U')'] = KWIK_TOK_CLOSE_PAREN;
    simple_tok_table[U':'] = KWIK_TOK_COLON;
    simple_tok_table[U';'] = KWIK_TOK_SEMICOLON;
    simple_tok_table[U'='] = KWIK_TOK_EQUALS;
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

    jump_table[U'_'] = I::ALPHA;
    jump_table[0] = I::NULL_EOF;
    jump_table[U' '] = I::SPACE;
    jump_table[U'\n'] = I::NEWLINE;
    jump_table[U'#'] = I::COMMENT;
    jump_table[U'.'] = I::DOT;
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

    Token Lexer::lex_num() {
        int base = 10;
        bool floating = false;
        int startcol = col;

        auto n = *std::next(it);
        if (*it == U'0' && (n == U'b' || n == U'o' || n == U'x')) {
            std::advance(it, 2);
            col += 2;
            switch (n) {
            case U'b': base =  2; break;
            case U'o': base =  8; break;
            case U'x': base = 16; break;
            }
        }

        std::string value;
        if (base == 10) {
            while (aisdigit(*it)) { value += *it++; ++col; }
        } else if (base == 2) {
            while (aisbdigit(*it)) { value += *it++; ++col; }
        } else if (base == 8) {
            while (aisodigit(*it)) { value += *it++; ++col; }
        } else if (base == 16) {
            while (aisxdigit(*it)) { value += *it++; ++col; }
        }

        if (base == 10) {
            if (*it == U'.') {
                floating = true;
                value += *it++; ++col;
            }
            
            while (aisdigit(*it)) { value += *it++; ++col; }
        }

        std::string suffix;
        size_t suffix_col = col;
        while (aisalnum(*it)) { suffix += *it++; ++col; }

        if (suffix.size()) {
            if (suffix == "f32" || suffix == "f64") {
                if (base != 10) {
                    throw SyntaxError("invalid base for suffix '" + suffix + "'",
                                      s.src.name, line, startcol, s.src.lines[line - 1]);
                }
                floating = true;
            } else if (floating) {
                throw SyntaxError("invalid float suffix '" + suffix + "'",
                                  s.src.name, line, suffix_col, s.src.lines[line - 1]);
            } else if (!int_suffixes_set.count(suffix)) {
                throw SyntaxError("invalid integer suffix '" + suffix + "'",
                                  s.src.name, line, suffix_col, s.src.lines[line - 1]);
            }
        }

        Token tok = {KWIK_TOK_NUM, line, startcol, value};
        tok.number.base = base;
        tok.number.floating = floating;
        tok.number.suffix = suffix.size() ? new std::string(suffix) : nullptr;
        return tok;
    }

    Token Lexer::lex_ident() {
        int startcol = col++;
        std::string ident(1, *it++);
        while (aisalnum(*it) || *it == U'_') { ident += *it++; ++col; }

        auto it = keywords.find(ident);
        if (it == keywords.end()) return {KWIK_TOK_NAME, line, startcol, ident};
        return {it->second, line, startcol};
    }

    Token Lexer::get_token() {
        while (true) {
            int startcol = col;
            uint32_t c = *it;
            if (c >= 128) throw_unexpected_char(c, line, startcol);

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
                do { ++it; ++col; } while (*it && *it != U'\n');
                break;
            case I::DIGIT:
                return lex_num();
            case I::ALPHA:
                return lex_ident();
            case I::DOT:
                if (aisdigit(*std::next(it))) return lex_num();
                ++it; ++col;
                throw_unexpected_char(c, line, startcol);
            case I::SIMPLE_TOK:
                ++it; ++col;
                return {simple_tok_table[c], line, startcol};
            }
        }
    }
}
