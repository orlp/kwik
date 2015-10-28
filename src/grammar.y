%token_prefix KWIK_TOK_
%token_type double

%include {
#include <cassert>
#include <cmath>
#include <iostream>

#include "parser.h"
}

%extra_argument { ParserState* s }

%syntax_error {
    std::cout << "Syntax error for tok #" << TOKEN << ".\n";
    s->num_errors += 1;
}

%left PLUS MINUS.
%left MUL DIV.
%left POW.

program ::= expr(A). { if (s->num_errors == 0) printf("%f\n", A); }
expr(A) ::= expr(B) PLUS expr(C). { A = B + C; }
expr(A) ::= expr(B) MINUS expr(C). { A = B - C; }
expr(A) ::= MINUS expr(B). { A = -B; }
expr(A) ::= PLUS expr(B). { A = B; }
expr(A) ::= expr(B) MUL expr(C). { A = B * C; }
expr(A) ::= expr(B) DIV expr(C). { A = B / C; }
expr(A) ::= expr(B) POW expr(C). { A = std::pow(B, C); }
expr(A) ::= LPAREN expr(B) RPAREN. { A = B; }
expr(A) ::= NUMBER(B). { A = B; }
