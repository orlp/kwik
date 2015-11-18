%token_prefix KWIK_TOK_
%token_type { kwik::Token }
%name KwikParse

%include {
#include "precompile.h"

#include <cassert>
#include <cmath>
#include <iostream>

#include "libop/op.h"
#include "parser.h"
#include "lexer.h"
}

%token_destructor { if ($$.val) delete $$.val; }

%extra_argument { kwik::ParseState* s }

%syntax_error {
    op::printf("{}:{}:{}: syntax error: unexpected token '{}'\n",
    s->filename, TOKEN.line, TOKEN.col, TOKEN.as_str());
    s->num_errors += 1;

}

%stack_overflow {
    op::print("Parser stack overflow.");
    std::exit(1);
}


program ::= onl compound_statement onl. {
    op::printf("Finished parse with {} error(s).\n", s->num_errors);
}

nl ::= NL.
nl ::= nl NL.
onl ::= .
onl ::= nl.

open_paren ::= OPEN_PAREN. { s->nested_paren++; }
close_paren ::= CLOSE_PAREN. { s->nested_paren--; }

compound_statement ::= OPEN_BRACE onl CLOSE_BRACE.
compound_statement ::= OPEN_BRACE onl statement_list onl CLOSE_BRACE.

statement_list ::= statement.
statement_list ::= statement_list SEMICOLON onl statement.
statement_list ::= statement_list nl statement.

statement ::= expr.
statement ::= compound_statement.

expr ::= NUM(a). { op::print(*a.val); }
expr ::= open_paren expr close_paren.
