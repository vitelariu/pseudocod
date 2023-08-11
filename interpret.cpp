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
			std::ostringstream ss;
			ss << sourceFile.rdbuf();
			sourceCode = ss.str();
		}
	}
	else {
		/* Eroare, argumente neasteptate */

	}

	return 0;
}
