#include <iostream>
#include <fstream>
#include <utility>
#include "interpreter.h"


void start(std::vector<std::pair<std::string, int>>& tokens, bool inTerminal) {
	switch(tokens[0].second) {
		case token_OUTPUT:
		{
			outAst *Tree = parseOut::parseEntry(tokens);
			interpretOut::interpretEntry(Tree);
			if(inTerminal) std::cout << '\n';
			break;
		}
		default:
		{
			exprAst *Tree = parseExpr::parseEntry(tokens);
			interpretExpr::interpretEntry(Tree);

			if(inTerminal) {
				if(Tree->op == numberType) std::cout << Tree->number;
				else std::cout << Tree->str;
				std::cout << '\n';
			}

			delete Tree;
			Tree = nullptr;

		}
	}
}


int main(int argc, char **argv) {
	std::string filename{};
	argc--;
	if(argc == 1) {
		filename = argv[1];
	}

	int line_number{1};
	try {

		if(argc == 0) {
			/* Interpreteaza direct in terminal */

			std::cout << ">> ";
			while(getline(std::cin, sourceCode)) {
				if(sourceCode.length() == 0) {
					std::cout << ">> ";
					continue;
				}
				sourceCode2 = sourceCode;

				std::vector<std::pair<std::string, int>> tokens{}; // every token and its type on this line

				sourceCode += "$";
				while(character != '$' and p < (int) sourceCode.length() - 1) {
					int x = getNextToken();


					std::pair<std::string, int> token(currentToken, x);


					// token_FORCE_QUIT este un token care este returnat atunci cand
					//pe linie, ultimul caracter este ' ', '\t'
					// Acesta trebuie ignorat
					if(x != token_FORCE_QUIT) {
						tokens.push_back(token);
					}
				}
				p = 0;
				character = ' '; // asta putea fi setat la orice in afara de $

				start(tokens, true);


				std::cout << ">> ";

			}
		}
		else if(argc == 1) {
			/* Citeste din fisier, pune in variabila sourceCode (declarara in lexer.h) si interpreteaza */

			std::ifstream sourceFile(filename);
			if(sourceFile) {


				while(getline(sourceFile, sourceCode)) {
					if(sourceCode.length() == 0) continue;
					sourceCode2 = sourceCode;

					std::vector<std::pair<std::string, int>> tokens{}; // every token and its type on this line



					sourceCode += "$";
					while(character != '$' and p < (int) sourceCode.length() - 1) {
						int x = getNextToken();


						std::pair<std::string, int> token(currentToken, x);


						// token_FORCE_QUIT este un token care este returnat atunci cand
						// pe linie, ultimul caracter este ' ', '\t'
						// Acesta trebuie ignorat
						if(x != token_FORCE_QUIT) {
							tokens.push_back(token);
						}
					}
					p = 0;
					character = ' '; // asta putea fi setat la orice in afara de $

					start(tokens, false);
					line_number++;

				}		

			}
			else {
				throw invalidFile;
			}

		}
		else {
			throw tooManyArgs;
		}

		return 0;
	}
	catch(errorsTypes n) {
		switch(n) {
			case syntaxError:
				errors::syntax_error(filename, sourceCode2, line_number);
				break;
			case divisionByZero:
				errors::division_by_zero(filename, sourceCode2, line_number);
				break;
			case invalidFile:
				errors::invalid_file(filename);
				break;
			case tooManyArgs:
				errors::too_many_args();
				break;
		}
	}
}

