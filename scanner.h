#pragma once
#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include <map> // Added for keyword map
#include "token.h"


class Scanner {
public:
    explicit Scanner(const std::string& source);

    std::vector<Token> scanTokens();
    void reportError(const std::string& message);
    bool didEncounterError() const;

private:
    // --- Data Members ---
    const std::string source;
    std::vector<Token> tokens;

    // This is crucial for distinguishing keywords from general identifiers
    const std::map<std::string, TokenType> keywords;

    int start = 0;   // Start of the current lexeme
    int current = 0; // Current position in the source
    int line = 1;    // Current line number
	int lineStart = 0; // Start index of the current line
    bool hadError = false;

    // --- Core Scanning Logic ---
    void scanToken();

    // --- Specific Scanning Routines ---
    void scanIdentifier();
    void scanNumber();     
    void scanString();     
    void scanOperatorOrSymbol(char firstChar); 

    // --- Helper Methods ---
    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    void skipWhitespace();

    // --- Utility Methods ---
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;

    // --- Token Creation ---
    // Changed to void, as they should add the token directly to the 'tokens' vector
    void addToken(TokenType type);
    void addToken(TokenType type, std::variant<double, std::string, bool> literal);

    // Helper to initialize the keyword map.
    std::map<std::string, TokenType> initializeKeywords() const;
};

#endif // SCANNER_H