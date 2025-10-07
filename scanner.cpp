#include "scanner.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

std::map<std::string, TokenType> Scanner::initializeKeywords() const {
    std::map<std::string, TokenType> keywordMap;
    keywordMap["var"] = TokenType::VAR;
    keywordMap["fun"] = TokenType::FUN;
    keywordMap["return"] = TokenType::RETURN;
    keywordMap["if"] = TokenType::IF;
    keywordMap["else"] = TokenType::ELSE;
    keywordMap["for"] = TokenType::FOR;
    keywordMap["while"] = TokenType::WHILE;
    keywordMap["do"] = TokenType::DO;
    keywordMap["switch"] = TokenType::SWITCH;
    keywordMap["case"] = TokenType::CASE;
    keywordMap["default"] = TokenType::DEFAULT;
    keywordMap["break"] = TokenType::BREAK;
    keywordMap["continue"] = TokenType::CONTINUE;
    keywordMap["true"] = TokenType::TRUE;
    keywordMap["false"] = TokenType::FALSE;
    keywordMap["nil"] = TokenType::NIL;
    keywordMap["print"] = TokenType::PRINT;
    return keywordMap;
}

Scanner::Scanner(const std::string& source)
    : source(source), keywords(initializeKeywords()) {
}

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", std::nullopt, line, start, current);
    return tokens;
}

void Scanner::scanToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) return;

    char c = advance();

    if (isAlpha(c)) {
        scanIdentifier();
    }
    else if (isDigit(c)) {
        scanNumber();
    }
    else {
        switch (c) {
        case '"':
            scanString();
            break;
        case '(': case ')': case '{': case '}': case '[': case ']':
        case ',': case '.': case ';': case ':': case '?':
        case '+': case '-': case '*': case '/': case '%':
        case '!': case '=': case '>': case '<':
        case '&': case '|': case '^': case '~':
            scanOperatorOrSymbol(c);
            break;
        default:
            reportError("Unexpected character: '" + std::string(1, c) + "'.");
            break;
        }
    }
}

void Scanner::scanIdentifier() {
    while (isAlphaNumeric(peek())) {
        advance();
    }

    std::string text = source.substr(start, current - start);
    TokenType type;

    if (keywords.count(text)) {
        type = keywords.at(text);
    }
    else {
        type = TokenType::IDENTIFIER;
    }

    addToken(type);
}

void Scanner::scanNumber() {
    while (isDigit(peek())) {
        advance();
    }

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) {
            advance();
        }
    }

    if (peek() == 'e' || peek() == 'E') {
        advance();
        if (peek() == '+' || peek() == '-') {
            advance();
        }

        if (!isDigit(peek())) {
            reportError("Expected digit after exponent marker.");
        }

        while (isDigit(peek())) {
            advance();
        }
    }

    std::string text = source.substr(start, current - start);

    try {
        double value = std::stod(text);
        addToken(TokenType::NUMBER, value);
    }
    catch (const std::exception& e) {
        reportError("Invalid numeric literal.");
        addToken(TokenType::NUMBER);
    }
}

void Scanner::scanString() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        reportError("Unterminated string literal.");
        return;
    }

    advance();

    std::string value = source.substr(start + 1, current - start - 2);

    addToken(TokenType::STRING, value);
}

void Scanner::scanOperatorOrSymbol(char firstChar) {
    TokenType type;

    switch (firstChar) {
    case '(': type = TokenType::LEFT_PAREN; break;
    case ')': type = TokenType::RIGHT_PAREN; break;
    case '{': type = TokenType::LEFT_BRACE; break;
    case '}': type = TokenType::RIGHT_BRACE; break;
    case '[': type = TokenType::LEFT_BRACKET; break;
    case ']': type = TokenType::RIGHT_BRACKET; break;
    case ',': type = TokenType::COMMA; break;
    case '.': type = TokenType::DOT; break;
    case ';': type = TokenType::SEMICOLON; break;
    case ':': type = TokenType::COLON; break;
    case '?': type = TokenType::QUESTION; break;
    case '~': type = TokenType::TILDE; break;

    case '+':
        if (match('=')) { type = TokenType::PLUS_EQUAL; }
        else if (match('+')) { type = TokenType::PLUS_PLUS; }
        else { type = TokenType::PLUS; }
        break;
    case '-':
        if (match('=')) { type = TokenType::MINUS_EQUAL; }
        else if (match('-')) { type = TokenType::MINUS_MINUS; }
        else { type = TokenType::MINUS; }
        break;
    case '*':
        type = match('=') ? TokenType::STAR_EQUAL : TokenType::STAR;
        break;
    case '/':
        type = match('=') ? TokenType::SLASH_EQUAL : TokenType::SLASH;
        break;
    case '%':
        type = match('=') ? TokenType::PERCENT_EQUAL : TokenType::PERCENT;
        break;
    case '!':
        type = match('=') ? TokenType::BANG_EQUAL : TokenType::BANG;
        break;
    case '=':
        type = match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL;
        break;
    case '<':
        if (match('=')) { type = TokenType::LESS_EQUAL; }
        else if (match('<')) { type = match('=') ? TokenType::SHIFT_LEFT_EQUAL : TokenType::SHIFT_LEFT; }
        else { type = TokenType::LESS; }
        break;
    case '>':
        if (match('=')) { type = TokenType::GREATER_EQUAL; }
        else if (match('>')) {
            type = TokenType::SHIFT_RIGHT;
            if (match('=')) { type = TokenType::SHIFT_RIGHT_EQUAL; }
        }
        else { type = TokenType::GREATER; }
        break;
    case '&':
        if (match('=')) { type = TokenType::AMP_EQUAL; }
        else if (match('&')) { type = TokenType::AMP_AMP; }
        else { type = TokenType::AMP; }
        break;
    case '|':
        if (match('=')) { type = TokenType::PIPE_EQUAL; }
        else if (match('|')) { type = TokenType::PIPE_PIPE; }
        else { type = TokenType::PIPE; }
        break;
    case '^':
        type = match('=') ? TokenType::CARET_EQUAL : TokenType::CARET;
        break;
    default:
        reportError("Unreachable state in scanOperatorOrSymbol for character: " + std::string(1, firstChar));
        return;
    }

    addToken(type);
}

void Scanner::addToken(TokenType type) {
    std::string lexeme = source.substr(start, current - start);
    tokens.emplace_back(type, std::move(lexeme), std::nullopt, line, start - lineStart, current - lineStart);
}


void Scanner::addToken(TokenType type, std::variant<double, std::string, bool> literal) {
    std::string lexeme = source.substr(start, current - start);
    tokens.emplace_back(type, std::move(lexeme), std::move(literal), line, start - lineStart, current - lineStart);
}

bool Scanner::isAtEnd() const {
    return current >= source.length();
}

char Scanner::advance() {
    return source[current++];
}

char Scanner::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Scanner::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Scanner::match(char expected) {
    if (isAtEnd() || source[current] != expected) {
        return false;
    }
    current++;
    return true;
}

void Scanner::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            line++;
			lineStart = current;
            advance();
            break;
        case '/':
            if (peekNext() == '/') {
                while (peek() != '\n' && !isAtEnd()) advance();
            }
            else if (peekNext() == '*') {
                advance();
                advance();
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') {
                        line++;
						lineStart = current;
                    }
                    advance();
                }
                if (isAtEnd()) {
                    reportError("Unterminated block comment.");
                }
                else {
                    advance();
                    advance();
                }
            }
            else {
                return;
            }
            break;
        default:
            return;
        }
    }
}

bool Scanner::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

bool Scanner::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

void Scanner::reportError(const std::string& message)  {
    std::cerr << "[Line " << line << ", Col " << start - lineStart << "] Error: " << message << std::endl;
	hadError = true;
}

bool Scanner::didEncounterError() const 
{
    return hadError;
}
