#include <iostream>
#include <fstream>
#include <utility>
#include "interpreter.hpp"


#if 0
void start(std::vector<std::pair<std::string, int>>& tokens, bool inTerminal) {
	switch(tokens[0].second) {
		case token_OUTPUT:
		{
			outAst *Tree = parseOut::parseEntry(tokens);
			interpretOut::interpretEntry(Tree);
			if(inTerminal) std::cout << '\n';
			break;
		}
		case token_IDENTIFIER:
		{
			bool assign = false;
			if(tokens.size() > 1 and tokens[1].second == token_ASSIGN) assign = true;

			if(assign) {
				varNode *node = parseVar::parseEntry(tokens);
				interpretVar::interpretEntry(node);
			}
			else {
				std::cou
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

			break;
		}
		case token_INPUT:
		{
			inAst *Tree = parseIn::parseEntry(tokens);
			interpretIn::interpretEntry(Tree);

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
#endif

void parse(std::vector<std::pair<std::string, int>>& tokens, statements *Statements) {
	statement *Statement = new statement;

	switch(tokens[0].second) {
		case token_OUTPUT:
		{
			Statement->outAst_p = parseOut::parseEntry(tokens);
			Statements->add(Statement);
			break;
		}
		case token_INPUT:
		{
			Statement->inAst_p = parseIn::parseEntry(tokens);
			Statements->add(Statement);

			break;
		}
		case token_IDENTIFIER:
		{
			// nu e facut tot aici
			// mai e cazul cand nu e asignare, ci expr care incepe cu o var
			Statement->varNode_p = parseVar::parseEntry(tokens);
			Statements->add(Statement);

			break;
		}

		default:
		{
			Statement->exprAst_p = parseExpr::parseEntry(tokens);
			Statements->add(Statement);

			break;
		}

	}

}


void execute(statements *Statements, bool inTerminal) {
	for(statement* Statement : Statements->s) {

		if(Statement->outAst_p) {
			interpretOut::interpretEntry(Statement->outAst_p);

			if(inTerminal) std::cout << '\n';
		}
		else if(Statement->inAst_p) {
			interpretIn::interpretEntry(Statement->inAst_p);
		}
		else if(Statement->varNode_p) {
			interpretVar::interpretEntry(Statement->varNode_p);
		}
		else if(Statement->exprAst_p) {
			interpretExpr::interpretEntry(Statement->exprAst_p);

			if(inTerminal) {
				if(Statement->exprAst_p->op == numberType) std::cout << Statement->exprAst_p->number;
				else std::cout << Statement->exprAst_p->str;
				std::cout << '\n';
			}
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


	if(argc == 0) {
		std::cout << "PUS PE PAUZA!\n";
		std::cout << "INTERPRETEAZA DOAR DINTR-O FILA DEOCAMDATA!\n";
		std::cout << "VEI PUTEA INTERPRETA SI DE AICI IN URMATORUL UPDATE\n";
		throw 69;



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
			
			try {
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
					case uninitialisedVar:
						errors::uninitialised_var(filename, sourceCode2, line_number);
						break;
				}
			}



			std::cout << ">> ";

		}
	}
	else if(argc == 1) {
		try {
			/* Citeste din fisier, pune in variabila sourceCode (declarara in lexer.h) si interpreteaza */

			statements* Statements = new statements;

			std::ifstream sourceFile(filename);
			if(sourceFile) {


				while(getline(sourceFile, sourceCode)) {
					if(sourceCode.length() == 0) {
						line_number++;
						continue;
					}
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


					parse(tokens, Statements);	


					line_number++;

				}

				// Sfarsitul parsuirii filei
				// Timpul sa executam tree-ul pe care l-am creat (variabila Statements este tot tree-ul)

				execute(Statements, false);

			}
			else {
				throw invalidFile;
			}
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
				case uninitialisedVar:
					errors::uninitialised_var(filename, sourceCode2, line_number);
					break;
			}
		}
	}


	

	return 0;
}

