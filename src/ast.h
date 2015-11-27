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
            virtual const char* ast_type() = 0;
            virtual ~Node() { }
        };
        
        struct Stmt : Node { };
        struct Expr : Stmt { };

        struct NumberExpr : Expr {
            std::string val;
            int base;
            bool floating; 
            int suffix_len;
            NumberExpr(const std::string& val, int base, bool floating, int suffix_len)
                : val(val), base(base), floating(floating), suffix_len(suffix_len) { }
            const char* ast_type() override { return "Number"; }
        };

        struct NameExpr : Expr {
            std::string val;
            NameExpr(const std::string& val) : val(val) { }
            const char* ast_type() override { return "Name"; }
        };

        struct CompoundStmt : Stmt {
            std::vector<std::unique_ptr<Stmt>> stmt_list;
            void append_stmt(Stmt* stmt) { stmt_list.emplace_back(stmt); }
            const char* ast_type() override { return "CompoundStmt"; }
        };

        struct LetStmt : Stmt {
            std::string name;
            std::string typedecl;
            std::unique_ptr<Expr> expr;
            LetStmt(const std::string& name, const std::string& typedecl, Expr* expr)
            : name(name), typedecl(typedecl), expr(expr) { }
            const char* ast_type() override { return "LetStmt"; }
        };
        
        struct ReturnStmt : Stmt {
            std::unique_ptr<Expr> expr;
            ReturnStmt(Expr* expr) : expr(expr) { }
            const char* ast_type() override { return "ReturnStmt"; }
        };
    }
}

#endif
