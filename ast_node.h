#pragma once
#include <vector>

class AstVisitor;

class AstNode {
public:
    virtual ~AstNode() = default;
    virtual void accept(AstVisitor& visitor) = 0;
};

class Declaration : public AstNode {};

class Stmt : public Declaration {};

class Expr : public AstNode {};