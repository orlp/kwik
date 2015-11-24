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
            virtual std::string type_name() = 0;
            virtual ~Node() { }
        };
        
        struct Stmt : Node { };
        struct Expr : Stmt { };

        struct NumberExpr : Expr {
            std::string val;
            NumberExpr(const std::string& val) : val(val) { }
            virtual std::string type_name() { return "number"; }
        };

        struct NameExpr : Expr {
            std::string val;
            NameExpr(const std::string& val) : val(val) { }
            virtual std::string type_name() { return "name"; }
        };

        struct CompoundStmt : Stmt {
            std::vector<std::unique_ptr<Stmt>> stmt_list;
            void append_stmt(Stmt* stmt) { stmt_list.emplace_back(stmt); }
            virtual std::string type_name() { return "compound_stmt"; }
        };

        struct LetStmt : Stmt {
            std::string name;
            std::string type;
            std::unique_ptr<Expr> expr;
            LetStmt(const std::string& name, const std::string& type, Expr* expr)
            : name(name), type(type), expr(expr) { }
            virtual std::string type_name() { return "let_stmt"; }
        };
    }
}

#endif
