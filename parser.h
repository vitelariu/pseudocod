#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include "lexer.h"
#include "errors.h"

template <class T>
T readFromString(const std::string& str)
{
	std::stringstream input(str);
	T x;
	input >> x;
	return x;
}

/// Astea sunt folosite pentru a determina tipul de node din ast-ul de expresii
enum types {
	numberType,
	stringType,

	orType,

	andType,

	greaterType,
	greaterEqualType,
	smallerType,
	smallerEqualType,
	equalType,
	notEqualType,
	notType,

	plusType,
	minusType,

	multiplyType,
	divisionType,
	moduloType,

	powerType,
};

/** AST = Abstract syntax tree. E o clasa ce reprezinta un arbore binar in care fiecare nod poate
 fi evaluat la o singura valoare, cu proprietatea ca evaluarea radacinii da rezultatul evaluarii
 expresiei originale. Mai jos este definita structura nodurilor unui astfel de arbore (ar trebui
 redenumita clasa la exprAstNode)
*/
class exprAst {
	public:
		const static int INT_FLAG = 0;
		const static int NEGATIVE_FLAG = 1;
		const static int NOT_GATE_FLAG = 2;
		const static int CMP_BUG_FIX_FLAG = 3;

	public:
		// Dupa numarul de la op ne dam seama ce fel de node este
		// ex: 0 -> numar (doar cand op-ul este 0, int-ul number este valid)
		// 1 -> string (doar cand op-ul este 1, string-ul str este valid)
		// 2 -> plus (se va folosi pointerele de mai jos (exprAst *left/right) pentru a reprezenta nod-urile)
		// etc etc
		int op;
		double number;
		std::string str;

		// flags
		// - flags[0]
		// daca este true, calculul de sub acest node va fi transformat in int
		// useful pentru expresii de genul [1 / 2]
		//
		// - flags[1]
		// daca este true, numarul, sau expresia in cazul -(1 + 2) etc, va fi negativa
		//
		// - flags[2]
		// daca este true, numarul sau expr va fi trecuta prin not gate
		//
		// - flags[3]
		// rol in fixarea unui bug legat de comparari
		bool flags[4];

		exprAst *left;
		exprAst *right;

		exprAst() : op(-1), number(0), str(), flags(), left(nullptr), right(nullptr) {}

		exprAst(double x) : op(numberType), number(x), str(), flags(), left(nullptr), right(nullptr) {}

		exprAst(std::string x) : op(stringType), number(0), str(x), flags(), left(nullptr), right(nullptr) {}

		exprAst(int Type, exprAst* left, exprAst* right) : op(Type), number(0), str(), flags(), left(left), right(right) {}

		virtual ~exprAst()
		{
			delete left;
			delete right;
		}

		friend std::ostream& operator<<(std::ostream& out, const exprAst* const ast)
		{
			out << "Op: " << ast->op << '\n';
			out << "Number: " << ast->number << "\n\n";
			if(ast->left != nullptr) {
				out << ast->left;
			}
			if(ast->right != nullptr) {
				out << ast->right;
			}
			return out;
		}
};

/// Clasa pentru transformarea unei liste de tokene intr-un AST. Erorile sunt puse provizoriu si vor fi modificate in viitor
class parse {
	private:
		int index{}; // used for vector

		// must be 0 after execution
		int parenth_cnt{}, square_cnt{};

		std::pair<std::string, int> token;
		std::vector<std::pair<std::string, int>> tokens;


		void match(int type) {
			if(token.second != type) {
				std::cout << "eroare match";
			}
		}

		std::pair<std::string, int> getNextTokenFromVector() {
			if(index >= (int) tokens.size()) {
				std::cout << "eroare de sintaxa\n";
				return token;
			}
			token = tokens[index];
			index++;
			return token;
		}

		parse(std::vector<std::pair<std::string, int>>&& tokens) : tokens(std::move(tokens)) {}

		parse(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}

	private:
		exprAst *exp() {
			if(token.second == token_FLOAT) {
				double number = readFromString<double>(token.first);
				exprAst *nodeNumber = new exprAst(number);

				return nodeNumber;
			}
			else if(token.second == token_STRING) {
				std::string str = token.first;
				exprAst *nodeString = new exprAst(str);

				return nodeString;
			}
			else if(token.second == token_LEFT_PARENTH) {

				if(tokens[index].second == token_RIGHT_PARENTH) {
					token = getNextTokenFromVector();
					token.second = token_FORCE_QUIT;

					exprAst *nodeNumber = new exprAst(0);

					return nodeNumber;
				}

				parenth_cnt++;
				if(index == (int) tokens.size()) {
					exprAst *nodeNumber = new exprAst(0);

					return nodeNumber;

				}
				exprAst *nested = expr();
				match(token_RIGHT_PARENTH);
				token.second = token_FORCE_QUIT;
				nested->flags[3] = true;

				return nested;
			}
			else if(token.second == token_LEFT_SQUARE) {

				if(tokens[index].second == token_RIGHT_SQUARE) {
					token = getNextTokenFromVector();
					token.second = token_FORCE_QUIT;

					exprAst *nodeNumber = new exprAst(0);
					return nodeNumber;
				}


				square_cnt++;

				if(index == (int) tokens.size()) {
					exprAst *nodeNumber = new exprAst(0);

					return nodeNumber;
				}

				exprAst *nested = expr();
				nested->flags[0] = true;
				nested->flags[3] = true;

				match(token_RIGHT_SQUARE);
				token.second = token_FORCE_QUIT;
				return nested;
			}
			else if(token.second == token_MINUS) {
				if(index < (int) tokens.size()) {token = getNextTokenFromVector();}
				else {
					std::cout << "eroare bitch\n";
					exprAst *nodeNumber = new exprAst(0);
					return nodeNumber;

				}
				exprAst *nested = exp();
				nested->flags[1] = !(nested->flags[1]);
				token.second = token_FORCE_QUIT;
				return nested;

			}
			else if(token.second == token_PLUS) {
				if(index < (int) tokens.size()) {token = getNextTokenFromVector();}
				else {
					std::cout << "eroare bitch2 de data asta\n";
					exprAst *nodeNumber = new exprAst(0);
					return nodeNumber;

				}
				exprAst *nested = exp();
				token.second = token_FORCE_QUIT;
				return nested;

			}
			else if(token.second == token_NOT) {
				if(index < (int) tokens.size()) {token = getNextTokenFromVector();}
				else {
					std::cout << "eroare aksdhGF\n";
					exprAst *nodeNumber = new exprAst(0);
					return nodeNumber;

				}
				if(token.second != token_LEFT_PARENTH and token.second != token_FLOAT) {
					std::cout << "eroare frate\n";
				}
				else {
					exprAst *nested = exp();
					nested->flags[2] = !(nested->flags[2]);
					token.second = token_FORCE_QUIT;
					return nested;
				}

			}

			std::cout << "eroare 99";


			exprAst *nodeNumber = new exprAst(0);
			return nodeNumber;

		}

		exprAst *factor() {
			exprAst *exprTree = nullptr;
			exprAst *result = exp();
			exprTree = result;


			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}

				token = getNextTokenFromVector();

				bool op = false;
				for(int i = token_PLUS; i <= token_OR; i++) {
					if(token.second == i) {
						op = true;
						break;
					}
				}
				if(token.second == token_RIGHT_PARENTH or token.second == token_RIGHT_SQUARE) op = true;
				if(!op) {
					std::cout << "eroare de sintaxa\n";
					break;
				}
				if(token.second != token_POWER) break;

				if(token.second == token_POWER) {
					token = getNextTokenFromVector();

					if((token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						exprTree = new exprAst(powerType, exprTree, exp());
					}
					else {
						std::cout << "error8";
					}
				}
				else {
					std::cout << "error9";
				}
			}

			return exprTree;

		}

		exprAst *term() {
			exprAst *exprTree = nullptr;
			exprAst *result = factor();
			exprTree = result;


			while(true) {

				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_ASTERISK and token.second != token_DIVISION and token.second != token_MODULO) break;

				if(token.second == token_ASTERISK) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT or token.second == token_STRING) {
						exprTree = new exprAst(multiplyType, exprTree, factor());
					}
					else {
						std::cout << "error1";
					}
				}
				else if(token.second == token_DIVISION) {
					token = getNextTokenFromVector();

					if((token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						exprTree = new exprAst(divisionType, exprTree, factor());
						//result = result / factor();
					}
					else {
						std::cout << "error2";
					}
				}
				else if(token.second == token_MODULO) {
					token = getNextTokenFromVector();

					if((token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						exprTree = new exprAst(moduloType, exprTree, factor());
					}
					else {
						std::cout << "error69";
					}
				}
				else {
					std::cout << "error3";
				}

			}

			return exprTree;

		}

		exprAst *addend() {
			exprAst *exprTree = nullptr;
			exprAst *result = term();
			exprTree = result;



			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_PLUS and token.second != token_MINUS) break;

				if(token.second == token_PLUS) {
					token = getNextTokenFromVector();



					if((token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						exprTree = new exprAst(plusType, exprTree, term());

					}
					else if((token.second == token_STRING or token.second == token_LEFT_PARENTH) or result->op == stringType) {
						exprTree = new exprAst(plusType, exprTree, term());
					}
					else {
						std::cout << "error4.1";
					}
				}
				else if(token.second == token_MINUS) {
					token = getNextTokenFromVector();



					if((token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						exprTree = new exprAst(minusType, exprTree, term());

						//result = result - term();
					}
					else {
						std::cout << "error aici";
					}
				}
				else {
					std::cout << "error6";
				}
			}


			return exprTree;

		}

		exprAst *comp() {
			exprAst *exprTree = nullptr;
			exprAst *result = addend();
			exprTree = result;



			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_GREATER and token.second != token_GREATER_EQUAL and token.second != token_SMALLER and token.second != token_SMALLER_EQUAL and token.second != token_EQUAL and token.second != token_NOT_EQUAL) break;

				if(token.second == token_GREATER) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(greaterType, exprTree, addend());
					}
					else {
						std::cout << "error4.2";
					}
				}
				else if(token.second == token_GREATER_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(greaterEqualType, exprTree, addend());
					}
					else {
						std::cout << "error4.3";
					}
				}
				else if(token.second == token_SMALLER) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(smallerType, exprTree, addend());
					}
					else {
						std::cout << "error4.4";
					}
				}
				else if(token.second == token_SMALLER_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(smallerEqualType, exprTree, addend());

					}
					else {
						std::cout << "error4.5";
					}
				}
				else if(token.second == token_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(equalType, exprTree, addend());

					}
					else {
						std::cout << "error4.6";
					}
				}
				else if(token.second == token_NOT_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(notEqualType, exprTree, addend());

					}
					else {
						std::cout << "error4.7";
					}
				}
				else {
					std::cout << "error6";
				}
			}

			return exprTree;


		}

		exprAst *logic() {
			exprAst* exprTree = nullptr;
			exprAst* result = comp();

			exprTree = result;


			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_AND) break;

				if(token.second == token_AND) {
					token = getNextTokenFromVector();

					if((token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						exprTree = new exprAst(andType, exprTree, comp());
					}
					else {
						std::cout << "error4.0";
					}
				}
				else {
					std::cout << "error6";
				}
			}

			return exprTree;

		}

		exprAst *expr() {
			/*
			Arithmetic expression parser / interpreter.


			expr : logic ((OR) logic)*
			logic : comp ((AND) comp)*
			comp :  addend ((GREATER | GREATER_EQUAL | SMALLER | SMALLER_EQUAL | EQUAL | NOT_EQUAL) addend)*
			addend : term ((PLUS | MINUS) term)*
			term   : factor ((MUL | DIV | MOD) factor)*
			factor : exp ((POW) exp)*
			exp : INTEGER | LPAREN expr RPAREN | LSQUARE expr SQUARE | NOT_EQUAL exp
			*/
			exprAst* exprTree = nullptr;

			getNextTokenFromVector();
			exprAst *result = logic();

			exprTree = result;

			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}

				if(token.second != token_OR) break;

				getNextTokenFromVector();

				if((token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
					exprTree = new exprAst(orType, exprTree, logic());
				}
				else {
					std::cout << "error9";
				}
			}

			if(token.second == token_RIGHT_PARENTH) {
				parenth_cnt--;
			}
			else if(token.second == token_RIGHT_SQUARE) {
				square_cnt--;
			}


			if(token.second != token_FLOAT and token.second != token_RIGHT_PARENTH and token.second != token_STRING and token.second != token_RIGHT_SQUARE and token.second != token_FORCE_QUIT) {
				std::cout << "error7";
			}

			return exprTree;
		}

		/// Punct de inceput conversie tokene in AST. Poate un nume mai bun este convert.
		exprAst *start() {
			exprAst *result = expr();

			if(parenth_cnt != 0 or square_cnt != 0) {
				delete result;
				throw "inconsistent brackets\n";
			}

			return result;
		}

	public:

		/// Functia principala, converteste expresia la un AST
		static exprAst *parseEntry(const std::vector<std::pair<std::string, int>>& tokens) {
			parse Parser(tokens);
			return Parser.start();
		}
};
