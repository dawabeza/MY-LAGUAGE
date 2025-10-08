#include "ast_printer.h"
#include "declaration_nodes.h"
#include "expr_nodes.h"
#include "stmt_nodes.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>

// --- AstPrinter Implementation ---

AstPrinter::AstPrinter(std::ostream& outputStream)
    : output(outputStream) {
}

// --- Helper Implementation ---

std::string AstPrinter::newId() {
    return "N" + std::to_string(nodeIdCounter++);
}

void AstPrinter::emitNode(const std::string& id, const std::string& label) {
    // Escapes double quotes in the label for DOT compatibility
    std::string escapedLabel = label;
    size_t pos = escapedLabel.find('"');
    while (pos != std::string::npos) {
        escapedLabel.replace(pos, 1, "\\\"");
        pos = escapedLabel.find('"', pos + 2);
    }
    output << "    " << id << " [label=\"" << escapedLabel << "\"];\n";
}

void AstPrinter::emitEdge(const std::string& parentId, const std::string& childId, const std::string& label) {
    output << "    " << parentId << " -> " << childId << " [label=\"" << label << "\"];\n";
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
        // Should only happen for the absolute program root
        return "";
    }
    return parentIdStack.back();
}

// --- Public Entry Point ---

void AstPrinter::print(const std::vector<Declaration*>& ast) {
    output << "digraph AST {\n";
    output << "    rankdir=TB; // Top to Bottom layout\n";

    std::string programId = newId();
    emitNode(programId, "PROGRAM ROOT");

    pushParent(programId);

    for (size_t i = 0; i < ast.size(); ++i) {
        if (ast[i]) {
            std::string edgeLabel = "Decl " + std::to_string(i + 1);
            ast[i]->accept(*this);
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
        emitEdge(nodeId, newId(), "Initializer");
        decl->initializer->accept(*this);
    }
    popParent();
}

void AstPrinter::visitFuncDecl(FuncDecl* decl) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();

    std::string label = "FUN: " + decl->name.lexeme;
    label += " (Params: ";
    for (const auto& p : decl->params) {
        label += p.lexeme + ",";
    }
    if (!decl->params.empty()) label.pop_back(); // Remove trailing comma
    label += ")";

    emitNode(nodeId, label);
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    decl->body->accept(*this);
    popParent();
}

// --- Statement Visitors ---

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
    else {
        emitNode(newId(), "No Expression");
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
    else {
        emitNode(newId(), "No Value");
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
    for (size_t i = 0; i < stmt->statements.size(); ++i) {
        if (stmt->statements[i]) {
            std::string edgeLabel = "Stmt " + std::to_string(i + 1);
            stmt->statements[i]->accept(*this);
        }
    }
    popParent();
}

void AstPrinter::visitIfStmt(IfStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "IF");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);

    // Condition
    emitEdge(nodeId, newId(), "Condition");
    stmt->condition->accept(*this);

    // Then Branch
    emitEdge(nodeId, newId(), "Then");
    stmt->thenBranch->accept(*this);

    // Else Branch (optional)
    if (stmt->elseBranch) {
        emitEdge(nodeId, newId(), "Else");
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
    emitEdge(nodeId, newId(), "Condition");
    stmt->condition->accept(*this);
    emitEdge(nodeId, newId(), "Body");
    stmt->body->accept(*this);
    popParent();
}

void AstPrinter::visitDoWhileStmt(DoWhileStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "DO-WHILE");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    emitEdge(nodeId, newId(), "Body");
    stmt->body->accept(*this);
    emitEdge(nodeId, newId(), "Condition");
    stmt->condition->accept(*this);
    popParent();
}

void AstPrinter::visitForStmt(ForStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "FOR");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);

    if (stmt->initializer) {
        emitEdge(nodeId, newId(), "Init");
        stmt->initializer->accept(*this);
    }
    if (stmt->condition) {
        emitEdge(nodeId, newId(), "Cond");
        stmt->condition->accept(*this);
    }
    if (stmt->increment) {
        emitEdge(nodeId, newId(), "Inc");
        stmt->increment->accept(*this);
    }
    emitEdge(nodeId, newId(), "Body");
    stmt->body->accept(*this);

    popParent();
}

void AstPrinter::printCaseStmt(const std::string& switchNodeId, CaseStmt* caseStmt, int index) {
    std::string nodeId = newId();
    std::string label = caseStmt->value ? "CASE" : "DEFAULT";
    emitNode(nodeId, label);
    emitEdge(switchNodeId, nodeId, "Case " + std::to_string(index));

    pushParent(nodeId);
    if (caseStmt->value) {
        caseStmt->value->accept(*this);
    }

    std::string bodyId = newId();
    emitNode(bodyId, "Case Body");
    emitEdge(nodeId, bodyId, "Body");

    pushParent(bodyId);
    for (Declaration* s : caseStmt->body) {
        if (s) s->accept(*this);
    }
    popParent();
    popParent();
}

void AstPrinter::visitSwitchStmt(SwitchStmt* stmt) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "SWITCH");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    emitEdge(nodeId, newId(), "Condition");
    stmt->condition->accept(*this);

    for (size_t i = 0; i < stmt->cases.size(); ++i) {
        printCaseStmt(nodeId, stmt->cases[i], i + 1);
    }
    popParent();
}

// --- Expression Visitors ---

void AstPrinter::visitPrimaryExpr(PrimaryExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();

    // Attempt to retrieve literal/identifier value for label
    std::string value;
    if (expr->value.type == TokenType::IDENTIFIER) {
        value = expr->value.lexeme;
    }
    else if (expr->value.type == TokenType::STRING) {
        value = "\"" + expr->value.lexeme + "\"";
    }
    else {
        // Assume number or keyword literal
        value = expr->value.lexeme;
    }

    emitNode(nodeId, "LIT: " + value);
    if (!parentId.empty()) emitEdge(parentId, nodeId);
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
    emitEdge(nodeId, newId(), "Left");
    expr->left->accept(*this);

    emitEdge(nodeId, newId(), "Right");
    expr->right->accept(*this);
    popParent();
}

void AstPrinter::visitLogicalExpr(LogicalExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "Logical: " + expr->op.lexeme);
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    emitEdge(nodeId, newId(), "Left");
    expr->left->accept(*this);

    emitEdge(nodeId, newId(), "Right");
    expr->right->accept(*this);
    popParent();
}

void AstPrinter::visitAssignmentExpr(AssignmentExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "Assign: " + expr->op.lexeme);
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    emitEdge(nodeId, newId(), "Target");
    expr->left->accept(*this);

    emitEdge(nodeId, newId(), "Value");
    expr->right->accept(*this);
    popParent();
}

void AstPrinter::visitConditionalExpr(ConditionalExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "Ternary ?:");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    emitEdge(nodeId, newId(), "Condition");
    expr->condition->accept(*this);

    emitEdge(nodeId, newId(), "Then");
    expr->thenExpr->accept(*this);

    emitEdge(nodeId, newId(), "Else");
    expr->elseExpr->accept(*this);
    popParent();
}

void AstPrinter::printPostfixTail(const std::string& parentId, PostfixTail* tail, int index) {
    std::string nodeId = newId();
    std::string label = "Tail " + std::to_string(index) + ": " + tail->op.lexeme;
    emitNode(nodeId, label);
    emitEdge(parentId, nodeId);

    pushParent(nodeId);

    if (tail->op.type == TokenType::LEFT_PAREN) { // Function Call
        for (size_t i = 0; i < tail->arguments.size(); ++i) {
            emitEdge(nodeId, newId(), "Arg " + std::to_string(i + 1));
            tail->arguments[i]->accept(*this);
        }
    }
    else if (tail->indexOrCondition) { // Array or Member Access
        emitEdge(nodeId, newId(), tail->op.type == TokenType::DOT ? "Member" : "Index");
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
    emitEdge(nodeId, newId(), "Base");
    expr->primary->accept(*this);

    for (size_t i = 0; i < expr->tails.size(); ++i) {
        printPostfixTail(nodeId, expr->tails[i], i + 1);
    }
    popParent();
}

void AstPrinter::visitGroupingExpr(GroupingExpr* expr) {
    std::string nodeId = newId();
    std::string parentId = getCurrentParentId();
    emitNode(nodeId, "GROUPING ()");
    if (!parentId.empty()) emitEdge(parentId, nodeId);

    pushParent(nodeId);
    // Recursively visit the expression inside the parentheses
    emitEdge(nodeId, newId(), "Expression");
    expr->expression->accept(*this);
    popParent();
}