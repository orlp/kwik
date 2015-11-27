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

template<class T> static T get(T* p);
}

%code {
    template<class T>
    static T get(T* p) {
        if (!p) return T();
        T copy = *p;
        return copy;
    }
}

%syntax_error {
    s->errors.emplace_back(new SyntaxError(
        op::format("unexpected token '{}'", TOKEN.as_str()), 
        s->src, TOKEN.line, TOKEN.col));
    /* int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]); */
    /* for (int i = 0; i < n; ++i) { */
    /*     int a = yy_find_shift_action(yypParser, (YYCODETYPE) i); */
    /*     if (a < YYNSTATE + YYNRULE) { */
    /*         op::printf("possibly expected: {}\n", yyTokenName[i]); */
    /*     } */
    /* } */
}

%stack_overflow {
    throw InternalCompilerError("parser stack overflow");
}

%token_type { Token }
%token_destructor { $$.del(); }
%default_type { ast::Node* }
%default_destructor { if ($$) delete $$; }
%type expr { ast::Expr* }
%type atom { ast::Expr* }
%type name { ast::NameExpr* }
%type number { ast::NumberExpr* }
%type stmt { ast::Stmt* }
%type let_stmt { ast::LetStmt* }
%type return_stmt { ast::ReturnStmt* }
%type stmt_list { ast::CompoundStmt* }
%type compound_stmt { ast::CompoundStmt* }



program ::= onl compound_stmt(A) onl. {
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
stmt(A) ::= return_stmt(B). { A = B; }
stmt(A) ::= expr(B). { A = B; }

let_stmt(A) ::= LET onl NAME(B) onl EQUALS onl expr(D).
    { A = new ast::LetStmt(get(B.val), "", D); B.del(); }
let_stmt(A) ::= LET onl NAME(B) onl COLON onl NAME(C) onl EQUALS onl expr(D).
    { A = new ast::LetStmt(get(B.val), get(C.val), D); B.del(); C.del(); }

return_stmt(A) ::= RETURN expr(B). { A = new ast::ReturnStmt(B); }

expr(A) ::= open_paren expr(B) close_paren. { A = B; }
expr(A) ::= atom(B). { A = B; }

atom(A) ::= number(B). { A = B; }
atom(A) ::= name(B). { A = B; }
     
name(A) ::= NAME(B). { A = new ast::NameExpr(get(B.val)); B.del(); }
number(A) ::= NUM(B). {
    A = new ast::NumberExpr(get(B.val), B.number.base, B.number.floating, get(B.number.suffix));
    B.del();
}
