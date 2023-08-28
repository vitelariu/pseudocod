#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "lexer.h"

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

		double exp() {
			if(token.second == token_FLOAT) {
				return std::stod(token.first);
			}
			else if(token.second == token_LEFT_PARENTH) {

				if(tokens[index].second == token_RIGHT_PARENTH) {
					token = getNextTokenFromVector();
					token.second = token_FORCE_QUIT;
					return 0;
				}

				parenth_cnt++;

				double nested = expr();
				match(token_RIGHT_PARENTH);
				token.second = token_FORCE_QUIT;

				return nested;
			}
			else if(token.second == token_LEFT_SQUARE) {

				if(tokens[index].second == token_RIGHT_SQUARE) {
					token = getNextTokenFromVector();
					token.second = token_FORCE_QUIT;
					return 0;
				}


				square_cnt++;
				
				int nested = (int) expr();
				match(token_RIGHT_SQUARE);
				token.second = token_FORCE_QUIT;
				return (double) nested;
			}
			else {
				std::cout << "eroare";
			}
			return 0;

		}

		double factor() {
			double result = exp();

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
						result = pow(result, exp());
					}
					else {
						std::cout << "error8";
					}
				}
				else {
					std::cout << "error9";
				}
			}

			return result;

		}
		
		double term() {
			double result = factor();


			while(true) {

				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_ASTERISK and token.second != token_DIVISION and token.second != token_MODULO) break;

				if(token.second == token_ASTERISK) {
					token = getNextTokenFromVector();
 
					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result * factor();
					}
					else {
						std::cout << "error1";
					}
				}
				else if(token.second == token_DIVISION) {
					token = getNextTokenFromVector();
 
					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result / factor();
					}
					else {
						std::cout << "error2";
					}
				}
				else if(token.second == token_MODULO) {
					token = getNextTokenFromVector();
 
					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						double f = factor();
						if(result == (int) result and f == (int) f) {
							result = (int) result % (int) f;
						}
						else {
							std::cout << "Modulo pe numere reale interzis\n";
						}
					}
					else {
						std::cout << "error69";
					}
				}
				else {
					std::cout << "error3";
				}

			}

			return result;

		}

		double addend() {


			double result = term();

			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_PLUS and token.second != token_MINUS) break;

				if(token.second == token_PLUS) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result + term();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_MINUS) {
					token = getNextTokenFromVector();
 
					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result - term();
					}
					else {
						std::cout << "error5";
					}
				}
				else {
					std::cout << "error6";
				}
			}
		

			return result;

		}

		double comp() {
			double result = addend();

			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_GREATER and token.second != token_GREATER_EQUAL and token.second != token_SMALLER and token.second != token_SMALLER_EQUAL and token.second != token_EQUAL and token.second != token_NOT_EQUAL) break;

				if(token.second == token_GREATER) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result > addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_GREATER_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result >= addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_SMALLER) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result < addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_SMALLER_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result <= addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result == addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else if(token.second == token_NOT_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result != addend();
					}
					else {
						std::cout << "error4";
					}
				}
				else {
					std::cout << "error6";
				}
			}

			return result;
	

		}

		double logic() {
			double result = comp();

			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}


				if(token.second != token_AND) break;

				if(token.second == token_AND) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result && comp();
					}
					else {
						std::cout << "error4";
					}
				}
				else {
					std::cout << "error6";
				}
			}

			return result;
	
		}

		double expr() {
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


			token = getNextTokenFromVector();
			double result = logic();

			while(true) {
				if(index >= (int) tokens.size()) {
					break;
				}

				if(token.second != token_OR) break;


				if(token.second == token_OR) {
					token = getNextTokenFromVector();
	 
					if(token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE) {
						result = result || logic();
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

			return result;

		}

		double start() {
			double result = expr();

			if(parenth_cnt != 0 or square_cnt != 0) {
				std::cout << "inconsistent brackets\n";
				return square_cnt;
			}

			return result;
		}		
};

int parseEntry(std::vector<std::pair<std::string, int>> tokens) {
	parse Parser(tokens);


	
	// Deocamdata doar printeaza rezultatul
	std::cout << Parser.start() << '\n';

	return 0;
}
