#include "parser.h"
#include "expr_nodes.h"
#include "stmt_nodes.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens)
{
    // The parser is initialized with the token stream received from the scanner.
    // 'current' is automatically 0, pointing to the first token.
    // 'hadError' is automatically false.
}

std::vector<Declaration*> Parser::parse() { // <<< Definition for unresolved external symbol 1
    std::vector<Declaration*> declarations;

    // The program is a list of declarations until the EOF is hit.
    while (!isAtEnd()) {
        Declaration* decl = declaration();
        if (decl != nullptr) {
            declarations.push_back(decl);
        }
    }

    return declarations;
}

Declaration* Parser::varDeclaration() { /* ... implementation ... */ return nullptr; }
Declaration* Parser::funDeclaration(const std::string& kind) { /* ... implementation ... */ return nullptr; }

Stmt* Parser::statement() {
    // TEMPORARY: Instead of statement logic, we force it to parse 
    // the highest expression rule we want to test (unary) and treat 
    // it as an expression statement.
    Expr* expr = primary();
    consume(TokenType::SEMICOLON, "Expect ';' after expression for testing.");
    return new ExprStmt(expr);
}

// --- 2. Revised declaration() (Bypassing VAR/FUN for testing) ---

Declaration* Parser::declaration() {
    try {
        if (check(TokenType::VAR) || check(TokenType::FUN)) {
            throw error(peek(), "Declaration keywords ('var', 'fun') temporarily disabled for expression testing.");
        }
        return statement();
    }
    catch (ParseError& error) {
        synchronize();
        return nullptr;
    }
}

Stmt* Parser::blockStatement() {
    // Minimal definition
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return new BlockStmt({});
}

Stmt* Parser::ifStatement() { throw error(peek(), "Placeholder: ifStatement not implemented."); }
Stmt* Parser::forStatement() { throw error(peek(), "Placeholder: forStatement not implemented."); }
Stmt* Parser::whileStatement() { throw error(peek(), "Placeholder: whileStatement not implemented."); }
Stmt* Parser::doWhileStatement() { throw error(peek(), "Placeholder: doWhileStatement not implemented."); }
Stmt* Parser::switchStatement() { throw error(peek(), "Placeholder: switchStatement not implemented."); }
Stmt* Parser::breakStatement() { throw error(peek(), "Placeholder: breakStatement not implemented."); }
Stmt* Parser::continueStatement() { throw error(peek(), "Placeholder: continueStatement not implemented."); }
Stmt* Parser::returnStatement() { throw error(peek(), "Placeholder: returnStatement not implemented."); }
Stmt* Parser::printStatement() { throw error(peek(), "Placeholder: printStatement not implemented."); }
Stmt* Parser::exprStatement() { throw error(peek(), "Placeholder: exprStatement not implemented."); }


// --- Expression Rules (The Complete Chain - all must be defined) ---

// Note: parse, expression, assignment, conditional, logicalOr, etc. 
// must be fully defined, even as simple placeholders that call the next rule.

Expr* Parser::expression() { return assignment(); }
Expr* Parser::assignment() { return conditional(); }
Expr* Parser::conditional() { return logicalOr(); } // Ternary operator
Expr* Parser::logicalOr() { return logicalAnd(); }
Expr* Parser::logicalAnd() { return bitwiseOr(); }
Expr* Parser::bitwiseOr() { return bitwiseXor(); }
Expr* Parser::bitwiseXor() { return bitwiseAnd(); }
Expr* Parser::bitwiseAnd() { return equality(); }
Expr* Parser::equality() { return comparison(); }
Expr* Parser::comparison() { return shift(); }
Expr* Parser::shift() { return term(); }
Expr* Parser::term() { return factor(); }
Expr* Parser::factor() { return unary(); }

Expr* Parser::unary() { // <<< Definition for unresolved external symbol 2
    // UNARY -> ( "!" | "~" | "++" | "--" | "+" | "-" ) UNARY | POSTFIX ;

    if (match(TokenType::BANG, TokenType::TILDE, TokenType::PLUS_PLUS,
        TokenType::MINUS_MINUS, TokenType::PLUS, TokenType::MINUS)) {

        Token op = previous();
        Expr* right = unary(); // Recursive call for right-associativity
        return new UnaryExpr(op, right);
    }

    return postfix();
}

// unary, postfix, primary must have their full implementation as provided earlier.

Expr* Parser::postfix() {
    // POSTFIX -> PRIMARY POSTFIX_TAIL* ;
    // POSTFIX_TAIL -> "(" ARG_LIST? ")" | "[" EXPR "]" | "." IDENTIFIER | "++" | "--" ;

    Expr* expr = primary();
    // Loop to handle zero or more POSTFIX_TAIL operations
    while (check(TokenType::LEFT_PAREN) || check(TokenType::LEFT_BRACKET) ||
        check(TokenType::DOT) || check(TokenType::PLUS_PLUS) ||
        check(TokenType::MINUS_MINUS)) {

        // Ensure the base expression is a PostfixExpr if we're adding tails
        PostfixExpr* postfix = dynamic_cast<PostfixExpr*>(expr);
        if (!postfix) {
            // First postfix operation, wrap the base expression
            postfix = new PostfixExpr(expr);
            expr = postfix;
        }

        advance(); // Consume the operator/delimiter token
        Token op = previous();
        PostfixTail* tail = new PostfixTail(op);
        switch (op.type) {
        case TokenType::LEFT_PAREN: { // Function call: ( ARG_LIST? )
            // ARG_LIST -> ASSIGNMENT ( "," ASSIGNMENT )*
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    tail->arguments.push_back(assignment());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RIGHT_PAREN, "Expect ')' after function arguments.");
            break;
        }
        case TokenType::LEFT_BRACKET: { // Array access: [ EXPR ]
            tail->indexOrCondition = expression();
            consume(TokenType::RIGHT_BRACKET, "Expect ']' after array index.");
            break;
        }
        case TokenType::DOT: { // Member access: . IDENTIFIER
            tail->indexOrCondition = new PrimaryExpr(
                consume(TokenType::IDENTIFIER, "Expect property name after '.'.")
            );
            break;
        }
        case TokenType::PLUS_PLUS:
        case TokenType::MINUS_MINUS: { // Postfix Increment/Decrement
            // No additional parsing needed, just the operator token
            break;
        }
        default:
            // Should be unreachable due to the initial check/advance block, but included for safety.
            delete tail;
            break;
        }

        postfix->tails.push_back(tail);
    }

    return expr;
}

Expr* Parser::primary() {
    // PRIMARY -> "(" EXPR ")" | IDENTIFIER | NUMBER | STRING
    //           | "true" | "false" | "nil" ;

    if (match(TokenType::FALSE)) return new PrimaryExpr(previous());
    if (match(TokenType::TRUE)) return new PrimaryExpr(previous());
    if (match(TokenType::NIL)) return new PrimaryExpr(previous());

    if (match(TokenType::NUMBER, TokenType::STRING)) {
        return new PrimaryExpr(previous());
    }

    if (match(TokenType::IDENTIFIER)) {
        return new PrimaryExpr(previous());
    }

    if (match(TokenType::LEFT_PAREN)) {
        Expr* expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
		// here the precedence is enforced via the grammar structure
        return expr;
    }

    // If none of the above match, it's a syntax error.
    throw error(peek(), "Expect expression.");
}


bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    if (current >= tokens.size()) {
        // Safe check: If we're past the last token, return the actual EOF token 
        // which should be the last element in the vector.
        return tokens.back();
    }
    return tokens[current];
}

Token Parser::previous() const {
    // Returns the token that was just consumed.
    if (current > 0) return tokens[current - 1];
    // Should ideally not be called when current is 0, but return first token as a fallback.
    return tokens[0];
}

Token Parser::advance() {
    // Consumes the current token and moves 'current' pointer forward.
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    // Checks if the current token is of the given type, without consuming it.
    if (isAtEnd()) return false;
    return peek().type == type;
}

template <typename... Args>
bool Parser::match(Args... types) {
    // Checks if the current token matches any of the given types.
    // If it matches, the token is consumed (advance is called), and returns true.
    bool found = false;
    // C++17 fold expression to check all types
    if ((check(types) || ...)) {
        found = true;
        advance();
    }
    return found;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();

    // If we reach here, we found an error.
    throw error(peek(), message);
}

// --- ERROR REPORTING AND SYNCHRONIZATION HELPERS ---

void Parser::reportError(const Token& token, const std::string& message) {
    // Prints a detailed error message to the console.
    std::cerr << "[Line " << token.line << "] Error";
    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << " at end";
    }
    else if (token.type != TokenType::IDENTIFIER) {
        std::cerr << " at '" << token.lexeme << "'";
    }
    std::cerr << ": " << message << std::endl;
    hadError = true;
}

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    // Helper function to call reportError and return the ParseError exception.
    reportError(token, message);
    return ParseError();
}

void Parser::synchronize() {
    // Attempts to skip tokens until it finds a point where the parser can safely restart,
    // usually after a semicolon or before a clear declaration keyword.
    advance();

    while (!isAtEnd()) {
        // If we see a semicolon, the statement is over, so we can stop.
        if (previous().type == TokenType::SEMICOLON) return;

        // Look ahead for keywords that start a new top-level construct.
        switch (peek().type) {
        case TokenType::VAR:
        case TokenType::FUN:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::SWITCH: // Added SWITCH based on your grammar
        case TokenType::RETURN:
            return;
        default:
            break;
        }

        advance();
    }
}