#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <variant>
#include <optional>
#include <iostream>

// All token types in your language
enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, SEMICOLON, COLON,
    QUESTION,

    // One or two character tokens
    PLUS, PLUS_EQUAL, PLUS_PLUS,
    MINUS, MINUS_EQUAL, MINUS_MINUS,
    STAR, STAR_EQUAL,
    SLASH, SLASH_EQUAL,
    PERCENT, PERCENT_EQUAL,
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    AMP, AMP_AMP, AMP_EQUAL,
    PIPE, PIPE_PIPE, PIPE_EQUAL,
    CARET, CARET_EQUAL,
    TILDE,
    SHIFT_LEFT, SHIFT_LEFT_EQUAL,
    SHIFT_RIGHT, SHIFT_RIGHT_EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    VAR, FUN, IF, ELSE, FOR, WHILE, DO, SWITCH, CASE, DEFAULT,
    BREAK, CONTINUE, RETURN, PRINT,
    TRUE, FALSE, NIL,

    // End of file
    END_OF_FILE
};

// Represents a single lexeme from the source code
class Token {
public:
    TokenType type;                      // Kind of token
    std::string lexeme;                  // Actual text
    std::optional<std::variant<double, std::string, bool>> literal;  // Literal value (if any)
    int line;                            // Line number in source
    int start;                           // Starting column (0-based)
    int end;                             // Ending column (inclusive or exclusive, your choice)

    Token(TokenType type, std::string lexeme,
        std::optional<std::variant<double, std::string, bool>> literal,
        int line, int start, int end)
        : type(type),
        lexeme(std::move(lexeme)),
        literal(std::move(literal)),
        line(line),
        start(start),
        end(end) {
    }

    // Returns a human-readable representation
    std::string toString() const {
        std::string typeStr = tokenTypeToString(type);
        std::string litStr;
        if (literal.has_value()) {
            if (std::holds_alternative<double>(*literal))
                litStr = std::to_string(std::get<double>(*literal));
            else if (std::holds_alternative<std::string>(*literal))
                litStr = "\"" + std::get<std::string>(*literal) + "\"";
            else if (std::holds_alternative<bool>(*literal))
                litStr = std::get<bool>(*literal) ? "true" : "false";
        }

        return "Token(" + typeStr + ", \"" + lexeme + "\"" +
            (litStr.empty() ? "" : ", " + litStr) +
            ", line=" + std::to_string(line) +
            ", col=" + std::to_string(start) + "-" + std::to_string(end) + ")";
    }

private:
    static std::string tokenTypeToString(TokenType type) {
        switch (type) {
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::PLUS: return "PLUS";
        case TokenType::PLUS_EQUAL: return "PLUS_EQUAL";
        case TokenType::PLUS_PLUS: return "PLUS_PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MINUS_EQUAL: return "MINUS_EQUAL";
        case TokenType::MINUS_MINUS: return "MINUS_MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::STAR_EQUAL: return "STAR_EQUAL";
        case TokenType::SLASH: return "SLASH";
        case TokenType::SLASH_EQUAL: return "SLASH_EQUAL";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::PERCENT_EQUAL: return "PERCENT_EQUAL";
        case TokenType::BANG: return "BANG";
        case TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::GREATER: return "GREATER";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::AMP: return "AMP";
        case TokenType::AMP_AMP: return "AMP_AMP";
        case TokenType::AMP_EQUAL: return "AMP_EQUAL";
        case TokenType::PIPE: return "PIPE";
        case TokenType::PIPE_PIPE: return "PIPE_PIPE";
        case TokenType::PIPE_EQUAL: return "PIPE_EQUAL";
        case TokenType::CARET: return "CARET";
        case TokenType::CARET_EQUAL: return "CARET_EQUAL";
        case TokenType::TILDE: return "TILDE";
        case TokenType::SHIFT_LEFT: return "SHIFT_LEFT";
        case TokenType::SHIFT_LEFT_EQUAL: return "SHIFT_LEFT_EQUAL";
        case TokenType::SHIFT_RIGHT: return "SHIFT_RIGHT";
        case TokenType::SHIFT_RIGHT_EQUAL: return "SHIFT_RIGHT_EQUAL";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::VAR: return "VAR";
        case TokenType::FUN: return "FUN";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::FOR: return "FOR";
        case TokenType::WHILE: return "WHILE";
        case TokenType::DO: return "DO";
        case TokenType::SWITCH: return "SWITCH";
        case TokenType::CASE: return "CASE";
        case TokenType::DEFAULT: return "DEFAULT";
        case TokenType::BREAK: return "BREAK";
        case TokenType::CONTINUE: return "CONTINUE";
        case TokenType::RETURN: return "RETURN";
        case TokenType::PRINT: return "PRINT";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::NIL: return "NIL";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
        }
    }
};

#endif // TOKEN_H
