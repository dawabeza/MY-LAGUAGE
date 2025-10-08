#pragma once
#include "ast_node.h"
#include "ast_visitor.h"
#include "token.h"
#include "expr_nodes.h"
#include "stmt_nodes.h"
#include <vector>

class VarDecl : public Declaration {
public:
    Token name;
    Expr* initializer;

    VarDecl(Token name, Expr* initializer) : name(name), initializer(initializer) {}
    ~VarDecl() { delete initializer; }
    void accept(AstVisitor& visitor) override { visitor.visitVarDecl(this); }
};

class FuncDecl : public Declaration {
public:
    Token name;
    std::vector<Token> params;
    BlockStmt* body;

    FuncDecl(Token name, std::vector<Token> params, BlockStmt* body)
        : name(name), params(std::move(params)), body(body) {
    }
    ~FuncDecl() { delete body; }
    void accept(AstVisitor& visitor) override { visitor.visitFuncDecl(this); }
};