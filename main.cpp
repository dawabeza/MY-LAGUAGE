#include "scanner.h"       // For tokenizing the source code
#include "parser.h"        // For building the AST
#include "ast_print.h"   // For generating the DOT output
#include "declaration_nodes.h" // Ensures Declaration* type is known

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>


int main() {
	std::ifstream inputFile("lang.dav");
	if (!inputFile.is_open()) {
		std::cerr << "Error: Could not open input file: lang.dav\n";
		return 1;
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
		return 1;
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
		std::ofstream dotFile("ast.dot");
		if (!dotFile.is_open()) {
			std::cerr << "Error: Could not open output file: ast.dot\n";
			return 1;
		}
		AstPrinter printer(dotFile);
		printer.print(ast); // Writes the DOT code to the file
		dotFile.close();
		std::cout << "Success! DOT graph saved to: ast.dot\n";
		std::cout << "To generate an image (PNG), run this command in your terminal:\n";
		std::cout << "  dot -Tpng ast.dot -o ast.png\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Visualization Error: " << e.what() << "\n";
	}

	// Cleanup: Delete the AST to free memory.
	for (Declaration* decl : ast) {
		delete decl;
	}
	return 0;
}