#include "expr_nodes.h"
#include "declaration_nodes.h" 

PostfixTail::~PostfixTail() {
    delete indexOrCondition;

    // Delete all expression nodes used as arguments in a function call.
    for (Expr* arg : arguments) {
        delete arg;
    }
}

// --- PostfixExpr Destructor ---
// Definition needed to ensure virtual destructor is implemented.
PostfixExpr::~PostfixExpr() {
    delete primary;
    for (PostfixTail* tail : tails) {
        delete tail;
    }
}
// Note: Other destructors like ~UnaryExpr, ~BinaryExpr, etc., 
// are already defined inline or assumed to be handled.