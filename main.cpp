#include "scanner.h"       // For tokenizing the source code
#include "parser.h"        // For building the AST
#include "ast_printer.h"   // For generating the DOT output
#include "declaration_nodes.h" // Ensures Declaration* type is known

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

/**
 * @brief Reads source code from a file, scans, parses, and generates the Graphviz DOT representation of the AST.
 * * @param inputFilename The name of the file containing the source code (e.g., "program.my_lang").
 * @param outputFilename The name of the file (e.g., "ast.dot") to write the DOT code to.
 */
void visualizeAST(const std::string& inputFilename, const std::string& outputFilename) {
    std::cout << "--- Starting AST Visualization for file: " << inputFilename << " ---\n";

    // 1. READ SOURCE CODE FROM FILE
    std::ifstream inputFile(inputFilename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file: " << inputFilename << "\n";
        return;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string sourceCode = buffer.str();
    inputFile.close();

    // 2. SCANNING: Convert source code into a stream of tokens.
    Scanner scanner(sourceCode);
    std::vector<Token> tokens = scanner.scanTokens();

    if (tokens.empty()) {
        std::cerr << "Error: Scanner returned no tokens or encountered a critical error.\n";
        return;
    }

    // 3. PARSING: Convert the token stream into an Abstract Syntax Tree (AST).
    Parser parser(tokens);
    std::vector<Declaration*> ast = parser.parse();

    if (parser.Error()) {
        std::cerr << "Warning: Parsing encountered errors. AST visualization may be incomplete.\n";
    }

    if (ast.empty()) {
        std::cout << "Info: Parser produced an empty AST. Nothing to visualize.\n";
    }

    // 4. VISUALIZATION: Use the AstPrinter to generate the DOT language code.
    try {
        std::ofstream dotFile(outputFilename);
        if (!dotFile.is_open()) {
            std::cerr << "Error: Could not open output file: " << outputFilename << "\n";
            return;
        }

        AstPrinter printer(dotFile);
        printer.print(ast); // Writes the DOT code to the file

        dotFile.close();

        std::cout << "Success! DOT graph saved to: " << outputFilename << "\n";
        std::cout << "To generate an image (PNG), run this command in your terminal:\n";
        std::cout << "  dot -Tpng " << outputFilename << " -o ast.png\n";

    }
    catch (const std::exception& e) {
        std::cerr << "Visualization Error: " << e.what() << "\n";
    }

    // 5. Cleanup: Crucial C++ memory management for AST nodes.
    for (Declaration* decl : ast) {
        delete decl;
    }
}

int main() {
	std::string inputFilename = "lang.dav"; // Input source code file
	std::string outputFilename = "ast.dot";       // Output DOT file
	visualizeAST(inputFilename, outputFilename);
    system("dot -Tpng ast.dot -o ast.png");
	return 0;
}