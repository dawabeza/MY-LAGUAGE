#include "parser.h"
#include "expr_nodes.h"
#include "stmt_nodes.h"
#include "declaration_nodes.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens)
{
    // The parser is initialized with the token stream received from the scanner.
    // 'current' is automatically 0, pointing to the first token.
    // 'hadError' is automatically false.
}

std::vector<Declaration*> Parser::parse() { 
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

Declaration* Parser::varDeclaration() { 
	Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
	Expr* initializer = nullptr;
	if (match(TokenType::EQUAL)) {
		initializer = expression();
	}
	consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
	return new VarDecl(name, initializer);
}

Declaration* Parser::funDeclaration() { 
	Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
	consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");
	std::vector<Token> parameters;
	if (!check(TokenType::RIGHT_PAREN)) {
		do {
			if (parameters.size() >= 255) {
				throw error(peek(), "Cannot have more than 255 parameters.");
			}
			parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
		} while (match(TokenType::COMMA));
	}
	consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
	consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
	BlockStmt* body = dynamic_cast<BlockStmt*>(blockStatement());
	return new FuncDecl(name, parameters, body);
}

Stmt* Parser::statement() {
    switch (peek().type) {
	case TokenType::LEFT_BRACE:
		advance(); // Consume '{'
		return blockStatement();
	case TokenType::IF:
		return ifStatement();
    case TokenType::FOR:
		return forStatement();
	case TokenType::WHILE:
		return whileStatement();
	case TokenType::DO:
		return doWhileStatement();
	case TokenType::SWITCH:
		return switchStatement();
	case TokenType::BREAK:
		return breakStatement();
	case TokenType::CONTINUE:
		return continueStatement();
	case TokenType::RETURN:
		return returnStatement();
	case TokenType::PRINT:
		return printStatement();
	case TokenType::SEMICOLON:
		advance(); // Consume the semicolon
		return new ExprStmt(nullptr); // Empty statement
    default:
		return exprStatement();
    }
}


// --- 2. Revised declaration() (Bypassing VAR/FUN for testing) ---

Declaration* Parser::declaration() {

    try {
        if (match(TokenType::VAR)) {
            return varDeclaration();
        }
        if (match(TokenType::FUN)) {
            return funDeclaration();
        }
        // If it's not a declaration, assume it's a statement.
        return statement();
    }
    catch (ParseError& error) {
        // If an error occurs, enter panic mode to discard tokens until the next safe point.
        synchronize();
        return nullptr;
    }
}

Stmt* Parser::blockStatement() {
	// BLOCK_STATEMENT -> "{" DECLARATION* "}" ;
	std::vector<Declaration*> statements;
	while (!isAtEnd() && !check(TokenType::RIGHT_BRACE)) {
		statements.push_back(declaration());
	}
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
	return new BlockStmt(statements);
}

Stmt* Parser::ifStatement() {
	consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
	Expr* condition = expression();
	consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
	Stmt* thenBranch = statement();
	Stmt* elseBranch = nullptr;
	if (match(TokenType::ELSE)) {
		elseBranch = statement();
	}
	return new IfStmt(condition, thenBranch, elseBranch);
}
Stmt* Parser::forStatement() { throw error(peek(), "Placeholder: forStatement not implemented."); }
Stmt* Parser::whileStatement() { throw error(peek(), "Placeholder: whileStatement not implemented."); }
Stmt* Parser::doWhileStatement() { throw error(peek(), "Placeholder: doWhileStatement not implemented."); }
Stmt* Parser::switchStatement() { throw error(peek(), "Placeholder: switchStatement not implemented."); }
Stmt* Parser::breakStatement() { throw error(peek(), "Placeholder: breakStatement not implemented."); }
Stmt* Parser::continueStatement() { throw error(peek(), "Placeholder: continueStatement not implemented."); }
Stmt* Parser::returnStatement() { throw error(peek(), "Placeholder: returnStatement not implemented."); }
Stmt* Parser::printStatement() { throw error(peek(), "Placeholder: printStatement not implemented."); }
Stmt* Parser::exprStatement() {  throw error(peek(), "Placeholder: Expr stmt not implemented.");
}


// --- Expression Rules (The Complete Chain - all must be defined) ---

// Note: parse, expression, assignment, conditional, logicalOr, etc. 
// must be fully defined, even as simple placeholders that call the next rule.

Expr* Parser::expression() { 
    return assignment(); 
}

Expr* Parser::assignment() {
	// ASSIGNMENT -> CONDITIONAL ( ("=" | "+=" | "-=" | "*=" | "/=" | "%=" | "<<=" | ">>=" | "&=" | "^=" | "|=") ASSIGNMENT )? ;
    Expr* expr = conditional();

    if (match(TokenType::EQUAL, TokenType::PLUS_EQUAL, TokenType::MINUS_EQUAL,
        TokenType::STAR_EQUAL, TokenType::SLASH_EQUAL, TokenType::PERCENT_EQUAL,
        TokenType::SHIFT_LEFT_EQUAL, TokenType::SHIFT_RIGHT_EQUAL,
        TokenType::AMP_EQUAL, TokenType::CARET_EQUAL, TokenType::PIPE_EQUAL)) {

        Token op = previous();
        Expr* value = assignment();
        if (dynamic_cast<PrimaryExpr*>(expr) && dynamic_cast<PrimaryExpr*>(expr)->value.type == TokenType::IDENTIFIER) {
            return new AssignmentExpr(expr, op, value);
        }

        if (dynamic_cast<PostfixExpr*>(expr)) {
            return new AssignmentExpr(expr, op, value);
        }
        error(op, "Invalid assignment target.");
        return new AssignmentExpr(expr, op, value);
    }

    return expr;
}
Expr* Parser::conditional() {
	// CONDITIONAL -> LOGICAL_OR ( "?" EXPR ":" CONDITIONAL )? ;
    // Start with the expression of higher precedence (the condition)
    Expr* expr = logicalOr();

    // Check for the ternary operator start '?'
    if (match(TokenType::QUESTION)) {
        Expr* thenExpr = expression();
        consume(TokenType::COLON, "Expect ':' after true expression in conditional operator.");
        //this is the secret to enforce right_associativity here
        Expr* elseExpr = conditional();

        // Return the ConditionalExpr node
        return new ConditionalExpr(expr, thenExpr, elseExpr);
    }

    // If no '?', return the logicalOr expression as is
    return expr;
}
Expr* Parser::logicalOr() {
	// LOGICAL_OR -> LOGICAL_AND ( "||" LOGICAL_AND )* ;
    Expr* expr = logicalAnd();
    while (match(TokenType::PIPE_PIPE)) {
        Token op = previous();
        Expr* right = logicalAnd();
        expr = new LogicalExpr(expr, op, right);
    }

    return expr;
}

Expr* Parser::logicalAnd() {
	// LOGICAL_AND -> BITWISE_OR ( "&&" BITWISE_OR )* ;
    Expr* expr = bitwiseOr();
    while (match(TokenType::AMP_AMP)) {
        Token op = previous();
        Expr* right = bitwiseOr();
        expr = new LogicalExpr(expr, op, right);
    }

    return expr;
}
Expr* Parser::bitwiseOr() {
	// BITWISE_OR -> BITWISE_XOR ( "|" BITWISE_XOR )* ;
    Expr* expr = bitwiseXor();
    while (match(TokenType::PIPE)) {
        Token op = previous();
        Expr* right = bitwiseXor();
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}
Expr* Parser::bitwiseXor() {
	// BITWISE_XOR -> BITWISE_AND ( "^" BITWISE_AND )* ;
    Expr* expr = bitwiseAnd();
    while (match(TokenType::CARET)) {
        Token op = previous();
        Expr* right = bitwiseAnd();
        expr = new BinaryExpr(expr, op, right);
    }

    return expr;
}
Expr* Parser::bitwiseAnd() {
	// BITWISE_AND -> EQUALITY ( "&" EQUALITY )* ;
    Expr* expr = equality();

    while (match(TokenType::AMP)) {

        Token op = previous();

        Expr* right = equality();

        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}

Expr* Parser::equality() {
	// EQUALITY -> COMPARISON ( ("==" | "!=") COMPARISON )* ;
    Expr* expr = comparison();

    while (match(TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL)) {
        Token op = previous();
        Expr* right = comparison();
        expr = new BinaryExpr(expr, op, right);
    }

    return expr;
}
Expr* Parser::comparison() {
	// COMPARISON -> SHIFT ( ("<" | "<=" | ">" | ">=") SHIFT )* ;
    Expr* expr = shift();
    while (match(TokenType::LESS, TokenType::LESS_EQUAL,
        TokenType::GREATER, TokenType::GREATER_EQUAL)) {
        Token op = previous();
        Expr* right = shift();
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}

Expr* Parser::shift() {
	// SHIFT -> TERM ( ("<<" | ">>") TERM )* ;
    Expr* expr = term();

    while (match(TokenType::SHIFT_LEFT, TokenType::SHIFT_RIGHT)) {
        Token op = previous();
        Expr* right = term();
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}


Expr* Parser::term() {
    // TERM -> FACTOR ( ("+" | "-") FACTOR )* ;
    Expr* expr = factor();
    while (match(TokenType::PLUS, TokenType::MINUS)) {
        Token op = previous();
        Expr* right = factor();
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}
Expr* Parser::factor() {
    // FACTOR -> UNARY ( ("*" | "/" | "%") UNARY )* ;
    Expr* expr = unary();
    // Loop to handle left-associative operators: *, /, %
    while (match(TokenType::STAR, TokenType::SLASH, TokenType::PERCENT)) {
        Token op = previous();
        Expr* right = unary();
		//this enforced left associativity
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}

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