#include "ast_print.h"
#include "declaration_nodes.h"
#include "expr_nodes.h"
#include "stmt_nodes.h"
#include <sstream>
#include <stdexcept>

// --- AstPrinter Setup and Helpers ---

AstPrinter::AstPrinter(std::ostream& outputStream)
    : output(outputStream) {
}

std::string AstPrinter::newId() {
    return "N" + std::to_string(nodeIdCounter++);
}

void AstPrinter::emitNode(const std::string& id, const std::string& label) {
    std::string escapedLabel = label;
    // Simple quote escaping
    size_t pos = escapedLabel.find('"');
    while (pos != std::string::npos) {
        escapedLabel.replace(pos, 1, "\\\"");
        pos = escapedLabel.find('"', pos + 2);
    }
    // Minimal node definition
    output << "    " << id << " [label=\"" << escapedLabel << "\"];\n";
}

void AstPrinter::emitEdge(const std::string& parentId, const std::string& childId, const std::string& label) {
    // Clean edge definition: NO label unless explicitly specified.
    if (label.empty()) {
        output << "    " << parentId << " -> " << childId << ";\n";
    }
    else {
        output << "    " << parentId << " -> " << childId << " [label=\"" << label << "\"];\n";
    }
}

void AstPrinter::pushParent(const std::string& id) {
    parentIdStack.push_back(id);
}

void AstPrinter::popParent() {
    if (!parentIdStack.empty()) {
        parentIdStack.pop_back();
    }
}

std::string AstPrinter::getCurrentParentId() {
    if (parentIdStack.empty()) {
        return "";
    }
    return parentIdStack.back();
}

// --- Main Entry Point ---

void AstPrinter::print(const std::vector<Declaration*>& ast) {
    output << "digraph AST {\n";
    output << "    rankdir=TB;\n"; // Keep T/B layout for readability

    std::string programId = newId();
    emitNode(programId, "PROGRAM ROOT");

    pushParent(programId);

    for (Declaration* decl : ast) {
        if (decl) {
            decl->accept(*this);
        }
    }

    popParent();
    output << "}\n";
}

// --- Declaration Visitors ---

void AstPrinter::visitVarDecl(VarDecl* decl) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "VAR: " + decl->name.lexeme);
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    if (decl->initializer) {
        decl->initializer->accept(*this);
    }
    popParent();
}

void AstPrinter::visitFuncDecl(FuncDecl* decl) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();

    std::string label = "FUN: " + decl->name.lexeme;
    emitNode(nodeId, label);
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    decl->body->accept(*this);
    popParent();
}

// --- Statement Visitors (Minimal, cleaner edges) ---

void AstPrinter::visitExprStmt(ExprStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "Expr Stmt");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    stmt->expression->accept(*this);
    popParent();
}

void AstPrinter::visitPrintStmt(PrintStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "PRINT");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    if (stmt->expression) {
        stmt->expression->accept(*this);
    }
    popParent();
}

void AstPrinter::visitReturnStmt(ReturnStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "RETURN");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    if (stmt->value) {
        stmt->value->accept(*this);
    }
    popParent();
}

void AstPrinter::visitBreakStmt(BreakStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "BREAK");
    if (!parentId.empty()) emitEdge(parentId, nodeId);
}

void AstPrinter::visitContinueStmt(ContinueStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "CONTINUE");
    if (!parentId.empty()) emitEdge(parentId, nodeId);
}

void AstPrinter::visitBlockStmt(BlockStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "BLOCK {}");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    for (Declaration* s : stmt->statements) {
        if (s) s->accept(*this);
    }
    popParent();
}

void AstPrinter::visitIfStmt(IfStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "IF");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    stmt->condition->accept(*this);
    stmt->thenBranch->accept(*this);
    if (stmt->elseBranch) {
        stmt->elseBranch->accept(*this);
    }
    popParent();
}

void AstPrinter::visitWhileStmt(WhileStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "WHILE");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    stmt->condition->accept(*this);
    stmt->body->accept(*this);
    popParent();
}

void AstPrinter::visitDoWhileStmt(DoWhileStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "DO-WHILE");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    stmt->body->accept(*this);
    stmt->condition->accept(*this);
    popParent();
}

void AstPrinter::visitForStmt(ForStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "FOR");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    if (stmt->initializer) stmt->initializer->accept(*this);
    if (stmt->condition) stmt->condition->accept(*this);
    if (stmt->increment) stmt->increment->accept(*this);
    stmt->body->accept(*this);
    popParent();
}

void AstPrinter::printCaseStmt(const std::string& switchNodeId, CaseStmt* caseStmt, int index) {
    std::string nodeId = newId();
    std::string label = caseStmt->value ? "CASE" : "DEFAULT";
    emitNode(nodeId, label);
    emitEdge(switchNodeId, nodeId); // Unlabeled edge from SWITCH to CASE

    pushParent(nodeId);
    if (caseStmt->value) {
        caseStmt->value->accept(*this);
    }
    for (Declaration* s : caseStmt->body) {
        if (s) s->accept(*this);
    }
    popParent();
}

void AstPrinter::visitSwitchStmt(SwitchStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "SWITCH");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    stmt->condition->accept(*this);

    for (size_t i = 0; i < stmt->cases.size(); ++i) {
        printCaseStmt(nodeId, stmt->cases[i], i + 1);
    }
    popParent();
}

// --- Expression Visitors (Minimal, clean edges applied to all) ---

void AstPrinter::visitPrimaryExpr(PrimaryExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();

    std::string value = expr->value.lexeme;

    emitNode(nodeId, "LIT: " + value);
    if (!parentId.empty()) emitEdge(parentId, nodeId);
}

void AstPrinter::visitGroupingExpr(GroupingExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "GROUPING ()");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    expr->expression->accept(*this);
    popParent();
}

void AstPrinter::visitUnaryExpr(UnaryExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "Unary: " + expr->op.lexeme);
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    expr->right->accept(*this);
    popParent();
}

void AstPrinter::visitBinaryExpr(BinaryExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "Binary: " + expr->op.lexeme);
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    expr->left->accept(*this);
    expr->right->accept(*this);
    popParent();
}

void AstPrinter::visitLogicalExpr(LogicalExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "Logical: " + expr->op.lexeme);
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    expr->left->accept(*this);
    expr->right->accept(*this);
    popParent();
}

void AstPrinter::visitAssignmentExpr(AssignmentExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "Assign: " + expr->op.lexeme);
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    expr->left->accept(*this);
    expr->right->accept(*this);
    popParent();
}

void AstPrinter::visitConditionalExpr(ConditionalExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "Ternary ?:");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    expr->condition->accept(*this);
    expr->thenExpr->accept(*this);
    expr->elseExpr->accept(*this);
    popParent();
}

void AstPrinter::printPostfixTail(const std::string& parentId, PostfixTail* tail, int index) {
    std::string nodeId = newId();
    std::string label = tail->op.lexeme;
    emitNode(nodeId, "Tail: " + label);
    emitEdge(parentId, nodeId);

    pushParent(nodeId);

    if (tail->op.type == TokenType::LEFT_PAREN) { // Function Call
        for (Expr* arg : tail->arguments) {
            arg->accept(*this);
        }
    }
    else if (tail->indexOrCondition) { // Array or Member Access
        tail->indexOrCondition->accept(*this);
    }

    popParent();
}

void AstPrinter::visitPostfixExpr(PostfixExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "POSTFIX");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    expr->primary->accept(*this);

    for (size_t i = 0; i < expr->tails.size(); ++i) {
        printPostfixTail(nodeId, expr->tails[i], i + 1);
    }
    popParent();
}