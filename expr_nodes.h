#pragma once
#include "ast_node.h"
#include "ast_visitor.h"
#include "token.h"
#include <vector>

class PrimaryExpr : public Expr {
public:
    PrimaryExpr(Token value) : value(value) {}
    ~PrimaryExpr() = default;

    Token value;
    void accept(AstVisitor& visitor) override { visitor.visitPrimaryExpr(this); }
};

struct PostfixTail {
    PostfixTail(Token op) :
        op{ op } {
    }

    ~PostfixTail(); // Requires definition in .cpp

    Token op;
	std::vector<Expr*> arguments; // For function calls
    Expr* indexOrCondition = nullptr;
};

class PostfixExpr : public Expr {
public:
    PostfixExpr(Expr* primary) : primary(primary) {}
    ~PostfixExpr(); // Requires definition in .cpp

    Expr* primary;
    std::vector<PostfixTail*> tails;

    void accept(AstVisitor& visitor) override { visitor.visitPostfixExpr(this); }
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(Token op, Expr* right) : op(op), right(right) {}
    ~UnaryExpr() { delete right; }

    Token op;
    Expr* right;

    void accept(AstVisitor& visitor) override { visitor.visitUnaryExpr(this); }
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(Expr* left, Token op, Expr* right) : left(left), op(op), right(right) {}
    ~BinaryExpr() { delete left; delete right; }

    Expr* left;
    Token op;
    Expr* right;

    void accept(AstVisitor& visitor) override { visitor.visitBinaryExpr(this); }
};

class LogicalExpr : public Expr {
public:
    LogicalExpr(Expr* left, Token op, Expr* right) : left(left), op(op), right(right) {}
    ~LogicalExpr() { delete left; delete right; }

    Expr* left;
    Token op; // Should only be AMP_AMP or PIPE_PIPE
    Expr* right;

    void accept(AstVisitor& visitor) override { visitor.visitLogicalExpr(this); }
};

class ConditionalExpr : public Expr {
public:
    ConditionalExpr(Expr* condition, Expr* thenExpr, Expr* elseExpr)
        : condition(condition), thenExpr(thenExpr), elseExpr(elseExpr) {
    }
    ~ConditionalExpr() { delete condition; delete thenExpr; delete elseExpr; }

    Expr* condition;
    Expr* thenExpr;
    Expr* elseExpr;

    void accept(AstVisitor& visitor) override { visitor.visitConditionalExpr(this); }
};

class AssignmentExpr : public Expr {
public:
    AssignmentExpr(Expr* left, Token op, Expr* right) : left(left), op(op), right(right) {}
    ~AssignmentExpr() { delete left; delete right; }

    Expr* left;
    Token op;
    Expr* right;

    void accept(AstVisitor& visitor) override { visitor.visitAssignmentExpr(this); }
};

class GroupingExpr : public Expr {
public:
    GroupingExpr(Expr* expression) : expression(expression) {}
    ~GroupingExpr() { delete expression; }
    void accept(AstVisitor& visitor) override { visitor.visitGroupingExpr(this); }
    Expr* expression;
};