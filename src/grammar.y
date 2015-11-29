%name KwikParse
%token_prefix KWIK_TOK_
%stack_size 0

%include {
#include "precompile.h"

#include "exception.h"
#include "parser.h"
#include "lexer.h"
#include "ast.h"

using namespace kwik;
}

%code {
}

%syntax_error {
    s->errors.emplace_back(new SyntaxError(
        op::format("unexpected token '{}'", TOKEN->as_str()), TOKEN->ref));
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

%extra_argument { ParseState* s }
%token_type { Token* }
%token_destructor { delete $$; }
%default_type { ast::Node* }
%default_destructor { if ($$) delete $$; }
%type expr { ast::Expr* }
%type atom { ast::Expr* }
%type name { ast::NameExpr* }
%type number { ast::NumberExpr* }
%type stmt { ast::Stmt* }
%type let_stmt { ast::LetStmt* }
%type return_stmt { ast::ReturnStmt* }
%type stmt_list { std::vector<std::unique_ptr<ast::Stmt>>* }
%type compound_stmt { ast::CompoundStmt* }
%type open_paren { Token* }
%type close_paren { Token* }
%destructor open_paren { delete $$; }
%destructor close_paren { delete $$; }

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
open_paren(A) ::= nest OPEN_PAREN(T). { A = T; }
close_paren(A) ::= unnest CLOSE_PAREN(T). { A = T; }

stmt_list(A) ::= stmt(B).
    { A = new std::vector<std::unique_ptr<ast::Stmt>>; A->emplace_back(B); }
stmt_list(A) ::= stmt_list(B) SEMICOLON onl stmt(C).
    { A = B; A->emplace_back(C); }
stmt_list(A) ::= stmt_list(B) nl stmt(C).
    { A = B; A->emplace_back(C); }

compound_stmt(A) ::= OPEN_BRACE(T) onl CLOSE_BRACE.
    { A = new ast::CompoundStmt(T); }
compound_stmt(A) ::= OPEN_BRACE(T) onl stmt_list(B) onl CLOSE_BRACE.
    { A = new ast::CompoundStmt(T, std::move(*B)); delete B; }
compound_stmt(A) ::= OPEN_BRACE(T) onl stmt_list(B) SEMICOLON onl CLOSE_BRACE.
    { A = new ast::CompoundStmt(T, std::move(*B)); delete B; }

stmt(A) ::= compound_stmt(B). { A = B; }
stmt(A) ::= let_stmt(B). { A = B; }
stmt(A) ::= return_stmt(B). { A = B; }
stmt(A) ::= expr(B). { A = B; }

let_stmt(A) ::= LET(T) onl NAME(B) onl EQUALS onl expr(D).
    { A = new ast::LetStmt(T, B->val, "", D); delete B; }
let_stmt(A) ::= LET(T) onl NAME(B) onl COLON onl NAME(C) onl EQUALS onl expr(D). {
    A = new ast::LetStmt(T, B->val, C->val, D);
    delete B; delete C;
}

return_stmt(A) ::= RETURN(T) expr(B). { A = new ast::ReturnStmt(T, B); }

expr(A) ::= open_paren expr(B) close_paren. { A = B; }
expr(A) ::= atom(B). { A = B; }

atom(A) ::= number(B). { A = B; }
atom(A) ::= name(B). { A = B; }
     
name(A) ::= NAME(B). { A = new ast::NameExpr(B); }
number(A) ::= NUM(B). { A = new ast::NumberExpr(B); }
