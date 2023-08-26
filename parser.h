#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "lexer.h"

// erorile sunt puse provizoriu


class parse {
	private:
		int index{}; // used for vector
		std::pair<std::string, int> token;
		std::vector<std::pair<std::string, int>> tokens;

	
		void match(int type) {
			if(token.second != type) {
				std::cout << "eroare";

			} 
		}

		std::pair<std::string, int> getNextTokenFromVector() {
			std::pair<std::string, int> token = tokens[index];
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
				double nested = expr();
				match(token_RIGHT_PARENTH);
				return nested;
			}
			else if(token.second == token_LEFT_SQUARE) {
				int nested = (int) expr();
				match(token_RIGHT_SQUARE);
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

		double expr() {
			/*
			Arithmetic expression parser / interpreter.

			calc> 7 + 3 * (10 / (12 / (3 + 1) - 1))
			22

			expr   : term ((PLUS | MINUS) term)*
			term   : factor ((MUL | DIV | MOD) factor)*
			factor : exp ((POW) exp)*
			exp : INTEGER | LPAREN expr RPAREN | LSQUARE expr SQUARE
			*/



			token = getNextTokenFromVector();
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
		
			if(token.second != token_FLOAT and token.second != token_RIGHT_PARENTH and token.second != token_RIGHT_SQUARE) {
				std::cout << "error7";
				std::cout << token.first << '\n';
			}

			return result;

		}
};

int parseEntry(std::vector<std::pair<std::string, int>> tokens) {
	parse Parser(tokens);

	
	// Deocamdata doar printeaza rezultatul
	std::cout << Parser.expr();

	return 0;
}
