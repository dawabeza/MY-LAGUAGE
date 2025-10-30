#pragma once
#include "ast_node.h"
#include "ast_visitor.h"
#include "expr_nodes.h"
#include "token.h"
#include <vector>

class ExprStmt : public Stmt {
public:
    ExprStmt(Expr* expression) : expression(expression) {}
    ~ExprStmt() { delete expression; }

    Expr* expression;

    void accept(AstVisitor& visitor) override { visitor.visitExprStmt(this); }
};

class PrintStmt : public Stmt {
public:
    PrintStmt(Expr* expression) : expression(expression) {}
    ~PrintStmt() { delete expression; }

    Expr* expression;

    void accept(AstVisitor& visitor) override { visitor.visitPrintStmt(this); }
};

class ReturnStmt : public Stmt {
public:
    ReturnStmt(Expr* value) : value(value) {}
    ~ReturnStmt() { delete value; }

    Expr* value;

    void accept(AstVisitor& visitor) override { visitor.visitReturnStmt(this); }
};

class BreakStmt : public Stmt {
public:
    BreakStmt() = default;
    ~BreakStmt() = default;

    void accept(AstVisitor& visitor) override { visitor.visitBreakStmt(this); }
};

class ContinueStmt : public Stmt {
public:
    ContinueStmt() = default;
    ~ContinueStmt() = default;

    void accept(AstVisitor& visitor) override { visitor.visitContinueStmt(this); }
};

class BlockStmt : public Stmt {
public:
    BlockStmt(std::vector<Declaration*> statements) : statements(std::move(statements)) {}
    ~BlockStmt(); // Requires definition in .cpp

    std::vector<Declaration*> statements;

    void accept(AstVisitor& visitor) override { visitor.visitBlockStmt(this); }
};

class IfStmt : public Stmt {
public:
    IfStmt(Expr* condition, Stmt* thenBranch, Stmt* elseBranch)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {
    }
    ~IfStmt() { delete condition; delete thenBranch; delete elseBranch; }

    Expr* condition;
    Stmt* thenBranch;
    Stmt* elseBranch;

    void accept(AstVisitor& visitor) override { visitor.visitIfStmt(this); }
};

class WhileStmt : public Stmt {
public:
    WhileStmt(Expr* condition, Stmt* body) : condition(condition), body(body) {}
    ~WhileStmt() { delete condition; delete body; }

    Expr* condition;
    Stmt* body;

    void accept(AstVisitor& visitor) override { visitor.visitWhileStmt(this); }
};

class DoWhileStmt : public Stmt {
public:
    DoWhileStmt(Stmt* body, Expr* condition) : body(body), condition(condition) {}
    ~DoWhileStmt() { delete body; delete condition; }

    Stmt* body;
    Expr* condition;

    void accept(AstVisitor& visitor) override { visitor.visitDoWhileStmt(this); }
};

class ForStmt : public Stmt {
public:
    ForStmt(Declaration* initializer, Expr* condition, Expr* increment, Stmt* body)
        : initializer(initializer), condition(condition), increment(increment), body(body) {
    }
    ~ForStmt() { delete initializer; delete condition; delete increment; delete body; }

    Declaration* initializer;
    Expr* condition;
    Expr* increment;
    Stmt* body;

    void accept(AstVisitor& visitor) override { visitor.visitForStmt(this); }
};

struct CaseStmt {
    ~CaseStmt(); // Requires definition in .cpp
    Expr* value;
    std::vector<Stmt*> body;
};

class SwitchStmt : public Stmt {
public:
    SwitchStmt(Expr* condition, std::vector<CaseStmt*> cases)
        : condition(condition), cases(std::move(cases)) {
    }
    ~SwitchStmt(); // Requires definition in .cpp

    Expr* condition;
    std::vector<CaseStmt*> cases;

    void accept(AstVisitor& visitor) override { visitor.visitSwitchStmt(this); }
};
