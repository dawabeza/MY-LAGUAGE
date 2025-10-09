#pragma once
#include "ast_visitor.h"
#include "token.h"
#include <iostream>
#include <vector>
#include <string>

// Forward Declarations
class VarDecl; class FuncDecl; class BlockStmt; class IfStmt; class ForStmt;
class WhileStmt; class DoWhileStmt; class SwitchStmt; class BreakStmt;
class ContinueStmt; class ReturnStmt; class PrintStmt; class ExprStmt;
struct CaseStmt; class LogicalExpr; class BinaryExpr; class AssignmentExpr;
class ConditionalExpr; class UnaryExpr; class PostfixExpr; class PrimaryExpr;
class GroupingExpr; struct PostfixTail;

class AstPrinter : public AstVisitor {
public:
    explicit AstPrinter(std::ostream& outputStream);
    void print(const std::vector<Declaration*>& ast);

    // --- Overridden Visitor Methods ---
    void visitVarDecl(VarDecl* decl) override;
    void visitFuncDecl(FuncDecl* decl) override;
    void visitBlockStmt(BlockStmt* stmt) override;
    void visitIfStmt(IfStmt* stmt) override;
    void visitForStmt(ForStmt* stmt) override;
    void visitWhileStmt(WhileStmt* stmt) override;
    void visitDoWhileStmt(DoWhileStmt* stmt) override;
    void visitSwitchStmt(SwitchStmt* stmt) override;
    void visitBreakStmt(BreakStmt* stmt) override;
    void visitContinueStmt(ContinueStmt* stmt) override;
    void visitReturnStmt(ReturnStmt* stmt) override;
    void visitPrintStmt(PrintStmt* stmt) override;
    void visitExprStmt(ExprStmt* stmt) override;
    void visitAssignmentExpr(AssignmentExpr* expr) override;
    void visitConditionalExpr(ConditionalExpr* expr) override;
    void visitLogicalExpr(LogicalExpr* expr) override;
    void visitBinaryExpr(BinaryExpr* expr) override;
    void visitUnaryExpr(UnaryExpr* expr) override;
    void visitPostfixExpr(PostfixExpr* expr) override;
    void visitPrimaryExpr(PrimaryExpr* expr) override;
    void visitGroupingExpr(GroupingExpr* expr) override; // Added Grouping

private:
    std::ostream& output;
    int nodeIdCounter = 0;
    std::vector<std::string> parentIdStack;

    // --- DOT Generation Helpers (Minimal) ---
    std::string newId();
    void emitNode(const std::string& id, const std::string& label);
    void emitEdge(const std::string& parentId, const std::string& childId, const std::string& label = "");

    // --- Parent Stack Management ---
    void pushParent(const std::string& id);
    void popParent();
    std::string getCurrentParentId();

    // --- Specialized Printing Helpers ---
    void printCaseStmt(const std::string& switchNodeId, CaseStmt* caseStmt, int index);
    void printPostfixTail(const std::string& parentId, PostfixTail* tail, int index);
};