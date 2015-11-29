#ifndef KWIK_AST_H
#define KWIK_AST_H

#include <string>
#include <memory>
#include <vector>
#include "libop/op.h"

#include "type.h"
#include "token.h"

namespace kwik {
    namespace ast {
        struct Node {
            Node(Token token) : token(token) { }
            // Constructor to take ownership of token from parser.
            Node(Token* tokptr) : token(std::move(*tokptr)) { delete tokptr; }
            virtual const char* ast_type() = 0;
            virtual ~Node() { }
            Token token;
        };
        
        struct Environment {
            Environment(Environment* parent) : parent(parent) { }
            Environment* parent;

            Node* lookup(const std::string& name) {
                auto it = symbols.find(name);
                if (it != symbols.end()) return it->second;
                if (parent) return parent->lookup(name);
                return nullptr;
            }

            std::map<std::string, Node*> symbols;
        };

        struct Stmt : Node {
            Stmt(Token token) : Node(std::move(token)) { }
            Stmt(Token* tokptr) : Node(tokptr) { }
            virtual void check(Environment& env) { }
        };

        struct Expr : Stmt {
            Expr(Token token) : Stmt(std::move(token)) { }
            Expr(Token* tokptr) : Stmt(tokptr) { }
            virtual Type type(Environment& env) = 0;
        };

        struct NumberExpr : Expr {
            NumberExpr(Token* tokptr) : Expr(tokptr) { }
            const char* ast_type() override { return "Number"; }

            Type type(Environment& env) override {
                // TODO: support more types.
                assert(!token.number.floating);
                assert(!token.number.suffix_len || token.val.substr(token.val.size() - token.number.suffix_len) == "i64");
                return kwik::Type::I64;
            }
        };

        struct NameExpr : Expr {
            NameExpr(Token* tokptr) : Expr(tokptr) { }
            const char* ast_type() override { return "Name"; }

            void check(Environment& env) override { val(env); }

            Expr* val(Environment& env) {
                auto node = env.lookup(token.val);
                if (!node) {
                    throw SemanticError(op::format("undefined name '{}'", token.val), token.ref);
                }

                Expr* expr = dynamic_cast<Expr*>(node);
                if (!expr) {
                    throw SemanticError(op::format("'{}' is not an expression", token.val), token.ref);
                }

                return expr;
            }
            
            Type type(Environment& env) override {
                return val(env)->type(env);
            }
        };

        struct CompoundStmt : Stmt {
            CompoundStmt(Token* tokptr, std::vector<std::unique_ptr<Stmt>> stmt_list)
                : Stmt(tokptr), stmt_list(std::move(stmt_list)) { }
            CompoundStmt(Token* tokptr) : Stmt(tokptr), stmt_list() { }
            const char* ast_type() override { return "CompoundStmt"; }

            void check(Environment& env) {
                for (auto& stmt : stmt_list) {
                    auto compound_stmt = dynamic_cast<CompoundStmt*>(stmt.get());
                    if (compound_stmt) {
                        Environment new_env(&env);
                        compound_stmt->check(new_env);
                    } else stmt->check(env);
                }
            }

            std::vector<std::unique_ptr<Stmt>> stmt_list;
        };


        struct LetStmt : Stmt {
            LetStmt(Token* tokptr, const std::string& name, const std::string& typedecl, Expr* expr)
            : Stmt(tokptr), name(name), typedecl(typedecl), expr(expr) { }
            const char* ast_type() override { return "LetStmt"; }

            void check(Environment& env) {
                if (env.symbols.find(name) != env.symbols.end()) {
                    throw SemanticError("name defined multiple times in same scope", token.ref);
                }

                expr->check(env);
                if (typedecl.size()) {
                    auto expr_type = type_name(expr->type(env));
                    if (expr_type != typedecl) {
                        throw SemanticError(op::format("wrong type, '{}' != '{}'", expr_type, typedecl), token.ref);
                    }
                }

                env.symbols[name] = expr.get();
            }

            std::string name;
            std::string typedecl;
            std::unique_ptr<Expr> expr;
        };
        
        struct ReturnStmt : Stmt {
            ReturnStmt(Token* tokptr, Expr* expr) : Stmt(tokptr), expr(expr) { }
            const char* ast_type() override { return "ReturnStmt"; }

            void check(Environment& env) {
                expr->check(env);
            }

            std::unique_ptr<Expr> expr;
        };
    }
}

#endif
