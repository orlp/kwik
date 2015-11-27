#ifndef KWIK_AST_H
#define KWIK_AST_H

#include <string>
#include <memory>
#include <vector>

#include "libop/op.h"

namespace kwik {
    namespace ast {
        struct Node {
        public:
            virtual std::string ast_type() = 0;
            virtual ~Node() { }
        };
        
        struct Stmt : Node { };
        struct Expr : Stmt { };

        struct NumberExpr : Expr {
            std::string val;
            int base;
            bool floating; 
            std::string suffix;
            NumberExpr(const std::string& val, int base, bool floating, const std::string& suffix)
                : val(val), base(base), floating(floating), suffix(suffix) { }
            virtual std::string ast_type() { return "number"; }
        };

        struct NameExpr : Expr {
            std::string val;
            NameExpr(const std::string& val) : val(val) { }
            virtual std::string ast_type() { return "name"; }
        };

        struct CompoundStmt : Stmt {
            std::vector<std::unique_ptr<Stmt>> stmt_list;
            void append_stmt(Stmt* stmt) { stmt_list.emplace_back(stmt); }
            virtual std::string ast_type() { return "compound_stmt"; }
        };

        struct LetStmt : Stmt {
            std::string name;
            std::string typedecl;
            std::unique_ptr<Expr> expr;
            LetStmt(const std::string& name, const std::string& typedecl, Expr* expr)
            : name(name), typedecl(typedecl), expr(expr) { }
            virtual std::string ast_type() { return "let_stmt"; }
        };
        
        struct ReturnStmt : Stmt {
            std::unique_ptr<Expr> expr;
            ReturnStmt(Expr* expr) : expr(expr) { }
            virtual std::string ast_type() { return "return_stmt"; }
        };
    }
}

#endif
