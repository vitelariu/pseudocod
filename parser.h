#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <type_traits>
#include "lexer.h"

// Astea sunt folosite pentru a determina tipul de node din ast-ul de expresii
enum types {
	numberType,

	orType,

	andType,

	greaterType,
	greaterEqualType,
	smallerType,
	smallerEqualType,
	equalType,
	notEqualType,

	plusType,
	minusType,

	multiplyType,
	divisionType,
	moduloType,

	powerType,
};


class exprAst {
    public:
		// Dupa numarul de la op ne dam seama ce fel de node este
		// ex: 0 -> numar (doar cand op-ul este 0, int-ul number este valid)
		// 1 -> plus (si se va folosi cele 2 pointere de mai jos pentru urmatoarele nodes)
		// etc etc
        int op = -1;
        double number{};
        
        exprAst *left = nullptr;
        exprAst *right = nullptr;
        
		// Functii pentru a construii ast-ul
		// 5 tipuri de functii:
		//
		// 1.
		//
		//    -----
		//    |num|
		exprAst *make(double x) {
			exprAst *newNode = new exprAst;
			newNode->op = numberType;


			newNode->number = x;

			return newNode;
		}
		// 2.
		//     ____
		//     |op|
		//     /  \
		//    /    \
		//  |num| |num| 
		exprAst *make(int Type, int x, int y) {
			exprAst *newNode = new exprAst;
			newNode->op = Type;

			newNode->left = new exprAst;
			newNode->right = new exprAst;

			newNode->left->op = 0;
			newNode->left->number = x;
			newNode->right->op = 0;
			newNode->right->number = y;


			return newNode;

		}
		// 3.
		//	
		//	  ____
		//    |op|
		//    /  \
		//   /    \
		// |op|  |num|
		exprAst *make(int Type, exprAst *left, int y) {

			exprAst *newNode = new exprAst;
			newNode->op = Type;


			newNode->left = left;
			

			newNode->right = new exprAst;

			newNode->right->op = numberType;
			newNode->right->number = y;

			return newNode;
		}
		// 4.
		//
		//    ____
		//    |op|
		//    /  \
		//   /    \
		// |num|  |op|
		exprAst *make(int Type, int x, exprAst* right) {
			exprAst *newNode = new exprAst;

			newNode->op = Type;

			newNode->right = right;
			newNode->left = new exprAst;

			newNode->left->op = numberType;
			newNode->left->number = x;

			return newNode;

		}
		// 5.
		//
		//    ____
		//    |op|
		//    /  \
		//   /    \
		// |op|  |op|
		exprAst *make(int Type, exprAst* left, exprAst* right) {
			exprAst *newNode = new exprAst;

			newNode->op = Type;

			newNode->left = left;
			newNode->right = right;

			return newNode;
		}
};






// erorile sunt puse provizoriu


class parse {
	private:
		int index{}; // used for vector
					 
		// must be 0 after execution
		int parenth_cnt{}, square_cnt{};
												
		std::pair<std::string, int> token;
		std::vector<std::pair<std::string, int>> tokens;

	
		void match(int type) {
			if(token.second != type) {
				std::cout << "eroare";

			} 
		}

		std::pair<std::string, int> getNextTokenFromVector() {
			token = tokens[index];
			index++;
			return token;
		}


	public:

		parse(std::vector<std::pair<std::string, int>> tokens) {
			this->tokens = tokens;
			

		}

		exprAst *exp() {
			if(token.second == token_FLOAT) {
				double number = std::stod(token.first);
				exprAst *nodeNumber = new exprAst;
				nodeNumber = nodeNumber->make(number);

				return nodeNumber;
			}
			else if(token.second == token_LEFT_PARENTH) {

				if(tokens[index].second == token_RIGHT_PARENTH) {
					token = getNextTokenFromVector();
					token.second = token_FORCE_QUIT;

					exprAst *nodeNumber = new exprAst;
					nodeNumber = nodeNumber->make(0);

					return nodeNumber;
				}

				parenth_cnt++;

				exprAst *nested = expr();
				match(token_RIGHT_PARENTH);
				token.second = token_FORCE_QUIT;

				return nested;
			}
			else if(token.second == token_LEFT_SQUARE) {

				if(tokens[index].second == token_RIGHT_SQUARE) {
					token = getNextTokenFromVector();
					token.second = token_FORCE_QUIT;
					
					exprAst *nodeNumber = new exprAst;
					nodeNumber = nodeNumber->make(0);
					return nodeNumber;

				}


				square_cnt++;
				
				exprAst *nested = expr();
				match(token_RIGHT_SQUARE);
				token.second = token_FORCE_QUIT;
				return nested;
			}
			else {
				std::cout << "eroare";
			}
			
			exprAst *nodeNumber = new exprAst;
			nodeNumber = nodeNumber->make(0);
			return nodeNumber;

		}

		exprAst *factor() {
			exprAst *exprTree = new exprAst;
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

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(powerType, exprTree, exp());

						//result = pow(result, exp());
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
			exprAst *exprTree = new exprAst;
			exprAst *result = factor();
			exprTree = result;


			while(true) {

				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_ASTERISK and token.second != token_DIVISION and token.second != token_MODULO) break;

				if(token.second == token_ASTERISK) {
					token = getNextTokenFromVector();
 
					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(multiplyType, exprTree, factor());
						//result = result * factor();
					}
					else {
						std::cout << "error1";
					}
				}
				else if(token.second == token_DIVISION) {
					token = getNextTokenFromVector();
 
					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(divisionType, exprTree, factor());
						//result = result / factor();
					}
					else {
						std::cout << "error2";
					}
				}
				else if(token.second == token_MODULO) {
					token = getNextTokenFromVector();
 
					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(moduloType, exprTree, factor());
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
			exprAst *exprTree = new exprAst;
			exprAst *result = term();
			exprTree = result;



			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_PLUS and token.second != token_MINUS) break;

				if(token.second == token_PLUS) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(plusType, exprTree, term());

						//result = result + term();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_MINUS) {
					token = getNextTokenFromVector();
 
					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(minusType, exprTree, term());

						//result = result - term();
					}
					else {
						std::cout << "error5";
					}
				}
				else {
					std::cout << "error6";
				}
			}
		

			return exprTree;

		}

		exprAst *comp() {
			exprAst *exprTree = new exprAst;
			exprAst *result = addend();
			exprTree = result;



			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_GREATER and token.second != token_GREATER_EQUAL and token.second != token_SMALLER and token.second != token_SMALLER_EQUAL and token.second != token_EQUAL and token.second != token_NOT_EQUAL) break;

				if(token.second == token_GREATER) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(greaterType, exprTree, addend());
						//result = result > addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_GREATER_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(greaterEqualType, exprTree, addend());
						//result = result >= addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_SMALLER) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(smallerType, exprTree, addend());
					//	result = result < addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_SMALLER_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(smallerEqualType, exprTree, addend());

						//result = result <= addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(equalType, exprTree, addend());

						//result = result == addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_NOT_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(notEqualType, exprTree, addend());

						//result = result != addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else {
					std::cout << "error6";
				}
			}

			return exprTree;
	

		}

		exprAst *logic() {
			exprAst* exprTree = new exprAst;

			exprAst* result = comp();

			exprTree = result;


			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_AND) break;

				if(token.second == token_AND) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(andType, exprTree, comp());
						//result = result && comp();
					}
					else {
						std::cout << "error4";
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
			exprAst* exprTree = new exprAst;

			token = getNextTokenFromVector();
			exprAst *result = logic();

			exprTree = result;

			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}

				if(token.second != token_OR) break;


				if(token.second == token_OR) {
					token = getNextTokenFromVector();
	 
					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						exprTree = exprTree->make(orType, exprTree, logic());
						//result = result || logic();
					}
					else {
						std::cout << "error9";
					}
				}
				else {
					std::cout << "error10";
				}


			}

			if(token.second == token_RIGHT_PARENTH) {
				parenth_cnt--;
			}
			else if(token.second == token_RIGHT_SQUARE) {
				square_cnt--;
			}



			if(token.second != token_FLOAT and token.second != token_RIGHT_PARENTH and token.second != token_RIGHT_SQUARE and token.second != token_FORCE_QUIT) {
				std::cout << "error7";
			}

			return exprTree;

		}

		exprAst *start() {
			exprAst *result = expr();

			if(parenth_cnt != 0 or square_cnt != 0) {
				std::cout << "inconsistent brackets\n";
			}

			return result;
		}		
};


// functie auxiliara printeaza tot ast-ul


void printAst(exprAst *exprTree) {
	std::cout << "Op: " << exprTree->op << '\n';
	std::cout << "Number: " << exprTree->number << "\n\n";
	if(exprTree->left != nullptr) {
		printAst(exprTree->left);
	}
	if(exprTree->right != nullptr) {
		printAst(exprTree->right);
	}
}

int parseEntry(std::vector<std::pair<std::string, int>> tokens) {
	parse Parser(tokens);


	
	exprAst *exprTree = Parser.start();

	// Tree inspection
	printAst(exprTree);

	return 0;
}
