%name KwikParse
%token_prefix KWIK_TOK_
%extra_argument { ParseState* s }
%stack_size 0

%include {
#include "precompile.h"

#include "exception.h"
#include "parser.h"
#include "lexer.h"
#include "ast.h"

using namespace kwik;

void tok_del(Token tok);
std::string tok_val(Token tok);
}

%code {
    void tok_del(Token tok) {
        if (tok.val) delete tok.val;
    }

    std::string tok_val(Token tok) {
       if (!tok.val) return "";
       std::string copy = *tok.val;
       delete tok.val;
       return copy;
    }
}

%syntax_error {
    s->error_with_context(
        op::format("syntax error: unexpected token '{}'", TOKEN.as_str()), 
                   TOKEN.line, TOKEN.col);

    int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
    for (int i = 0; i < n; ++i) {
        int a = yy_find_shift_action(yypParser, (YYCODETYPE) i);
        if (a < YYNSTATE + YYNRULE) {
            op::printf("possibly expected: {}\n", yyTokenName[i]);
        }
    }
}

%stack_overflow {
    throw InternalCompilerError("parser stack overflow");
}

%token_type { Token }
%token_destructor { tok_del($$); }
%default_type { ast::Node* }
%default_destructor { if ($$) delete $$; }
%type expr { ast::Expr* }
%type atom { ast::Expr* }
%type name { ast::NameExpr* }
%type number { ast::NumberExpr* }
%type stmt { ast::Stmt* }
%type let_stmt { ast::LetStmt* }
%type stmt_list { ast::CompoundStmt* }
%type compound_stmt { ast::CompoundStmt* }



program ::= onl compound_stmt(A) onl. {
    op::printf("Finished parse with {} error(s).\n", s->num_errors);
    s->program.reset(A);
}

nl ::= NL.
nl ::= nl NL.
onl ::= .
onl ::= nl.

// Lemon doesn't execute an action until the next rule/token is matched.
// We need to force this immediately for correct parsing since the lexer depends on this info.
nest ::= . { s->nested_paren++; }
unnest ::= . { s->nested_paren--; }
open_paren ::= nest OPEN_PAREN.
close_paren ::= unnest CLOSE_PAREN.

stmt_list(A) ::= stmt(B).
    { A = new ast::CompoundStmt; A->append_stmt(B); }
stmt_list(A) ::= stmt_list(B) SEMICOLON onl stmt(C).
    { A = B; A->append_stmt(C); }
stmt_list(A) ::= stmt_list(B) nl stmt(C).
    { A = B; A->append_stmt(C); }
compound_stmt(A) ::= OPEN_BRACE onl CLOSE_BRACE.
    { A = new ast::CompoundStmt; }
compound_stmt(A) ::= OPEN_BRACE onl stmt_list(B) onl CLOSE_BRACE. { A = B; }
compound_stmt(A) ::= OPEN_BRACE onl stmt_list(B) SEMICOLON onl CLOSE_BRACE. { A = B; }

stmt(A) ::= compound_stmt(B). { A = B; }
stmt(A) ::= let_stmt(B). { A = B; }
stmt(A) ::= expr(B). { A = B; }

let_stmt(A) ::= LET onl NAME(B) onl EQUALS onl expr(D).
    { A = new ast::LetStmt(tok_val(B), "", D); }
let_stmt(A) ::= LET onl NAME(B) onl COLON onl NAME(C) onl EQUALS onl expr(D).
    { A = new ast::LetStmt(tok_val(B), tok_val(C), D); }

expr(A) ::= open_paren expr(B) close_paren. { A = B; }
expr(A) ::= atom(B). { A = B; }

atom(A) ::= number(B). { A = B; }
atom(A) ::= name(B). { A = B; }
     
name(A) ::= NAME(B). { A = new ast::NameExpr(tok_val(B)); }
number(A) ::= NUM(B). { A = new ast::NumberExpr(tok_val(B)); }
