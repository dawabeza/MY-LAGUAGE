#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <optional>
#include <vector>
// Assuming "scanner.h" is in your include path
#include "scanner.h"

std::optional<std::string> readFile(const std::string & filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filePath << "'" << std::endl;
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

void runFile(const std::string & filePath) {
    std::optional<std::string> sourceCode = readFile(filePath);

    if (!sourceCode.has_value()) {
        return;
    }

    Scanner scanner(sourceCode.value());

    std::vector<Token> tokens = scanner.scanTokens();

    if (scanner.didEncounterError()) {
        std::cerr << "Compilation failed due to lexing errors." << std::endl;
        return;
    }

    std::cout << "Lexing successful! Passing " << tokens.size() << " tokens to the parser." << std::endl;
    for (int i = 0; i < tokens.size(); ++i) {
        std::cout << tokens[i].toString() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path/to/source/file>" << std::endl;
        return 64;
    }

    runFile(argv[1]);

    return 0;
}