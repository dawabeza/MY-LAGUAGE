#include "stmt_nodes.h"
#include "declaration_nodes.h"

// --- BlockStmt Destructor ---
BlockStmt::~BlockStmt() {
    // Must delete all statements/declarations contained within the block.
    for (Declaration* stmt : statements) {
        delete stmt;
    }
}

// --- CaseStmt Destructor ---
CaseStmt::~CaseStmt() {
    delete value; // The expression for the case value
    // Must delete all statements contained within the case body.
    for (Declaration* stmt : body) {
        delete stmt;
    }
}

// --- SwitchStmt Destructor ---
SwitchStmt::~SwitchStmt() {
    delete condition;
    // Must delete all case statements.
    for (CaseStmt* c : cases) {
        delete c;
    }
}