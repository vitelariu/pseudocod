#include <iostream>
#include <fstream>
#include <utility>
#include "interpreter.h"

int main(int argc, char **argv) {

	argc--;
	if(argc == 0) {
		/* Interpreteaza direct in terminal */
		
		std::cout << ">> ";
		while(getline(std::cin, sourceCode)) {
			if(sourceCode.length() == 0) {
				std::cout << ">> ";
				continue;
			}

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
			


			exprAst *Tree = parseEntry(tokens);
			interpret(Tree);
			
			if(Tree->op == numberType) std::cout << Tree->number << '\n';
			else std::cout << Tree->str << '\n';

			delete Tree;
			Tree = nullptr;

			std::cout << ">> ";

		}
	}
	else if(argc == 1) {
		/* Citeste din fisier, pune in variabila sourceCode (declarara in lexer.h) si interpreteaza */

		std::ifstream sourceFile(argv[1]);
		if(sourceFile) {


			while(getline(sourceFile, sourceCode)) {
				if(sourceCode.length() == 0) continue;

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
						
					

				parseEntry(tokens);

			}

		}
		else {
			std::cout << "TODO: Eroare\n";
		}

	}
	else {
		/* Eroare, argumente neasteptate */

	}

	return 0;
}
