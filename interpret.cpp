#include <iostream>
#include <fstream>
#include "lexer.h"


int main(int argc, char **argv) {
	argc--;

	if(argc == 0) {
		/* Interpreteaza direct in terminal */
	}
	else if(argc == 1) {
		/* Citeste din fisier, pune in variabila sourceCode (declarara in lexer.h) si interpreteaza */

		std::ifstream sourceFile(argv[1]);
		if(sourceFile) {

			while(getline(sourceFile, sourceCode)) {
				// TODO: SKIP la liniile complet goale


				sourceCode += "$";

				while(character != '$' and p < (int) sourceCode.length() - 1) {
					// So far, doar da print la acel tipul de token;
					std::cout << getNextToken() << '\n';
				}
				p = 0;
			}
		}
		else {
			std::cout << "TODO: Eroare plm\n";
		}

	}
	else {
		/* Eroare, argumente neasteptate */

	}

	return 0;
}
