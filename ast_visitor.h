#pragma once

// Forward Declarations for Statements and Declarations
class Declaration;
class VarDecl;
class FuncDecl;
class BlockStmt;
class IfStmt;
class ForStmt;
class WhileStmt;
class DoWhileStmt;
class SwitchStmt;
class BreakStmt;
class ContinueStmt;
class ReturnStmt;
class PrintStmt;
class ExprStmt;
struct CaseStmt; // Used inside SwitchStmt

// Forward Declarations for Expressions
class LogicalExpr; // For Logical OR/AND (Short-circuiting)
class BinaryExpr;  // For all other arithmetic, bitwise, comparison, equality, shift
class AssignmentExpr;
class ConditionalExpr;
class UnaryExpr;
class PostfixExpr;
class PrimaryExpr;
struct PostfixTail;
class GroupingExpr;
class AstVisitor {
public:
    virtual ~AstVisitor() = default;

    // DECLARATIONS
    virtual void visitVarDecl(VarDecl* decl) = 0;
    virtual void visitFuncDecl(FuncDecl* decl) = 0;

    // STATEMENTS
    virtual void visitBlockStmt(BlockStmt* stmt) = 0;
    virtual void visitIfStmt(IfStmt* stmt) = 0;
    virtual void visitForStmt(ForStmt* stmt) = 0;
    virtual void visitWhileStmt(WhileStmt* stmt) = 0;
    virtual void visitDoWhileStmt(DoWhileStmt* stmt) = 0;
    virtual void visitSwitchStmt(SwitchStmt* stmt) = 0;
    virtual void visitBreakStmt(BreakStmt* stmt) = 0;
    virtual void visitContinueStmt(ContinueStmt* stmt) = 0;
    virtual void visitReturnStmt(ReturnStmt* stmt) = 0;
    virtual void visitPrintStmt(PrintStmt* stmt) = 0;
    virtual void visitExprStmt(ExprStmt* stmt) = 0;

    // EXPRESSIONS (Simplified)
    virtual void visitAssignmentExpr(AssignmentExpr* expr) = 0;
    virtual void visitConditionalExpr(ConditionalExpr* expr) = 0;
    virtual void visitLogicalExpr(LogicalExpr* expr) = 0;
    virtual void visitBinaryExpr(BinaryExpr* expr) = 0;
    virtual void visitUnaryExpr(UnaryExpr* expr) = 0;
    virtual void visitPostfixExpr(PostfixExpr* expr) = 0;
    virtual void visitPrimaryExpr(PrimaryExpr* expr) = 0;
    virtual void visitGroupingExpr(GroupingExpr* expr) = 0;
};