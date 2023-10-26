#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <type_traits>
#include "lexer.h"
#include "errors.h"


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
		const static int INT_BUG_FIX_FLAG = 4; 
		const static int POW_BUG_FIX_FLAG = 5;

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
		//
		// - flags[4]
		// rol in fixarea unui bug legat de convertirea unui nr real negativ in intreg.
		//
		// - flags[5]
		// rol in fixarea unui bug legat de puteri: -x^y=z in loc de -x^y=-z
		bool flags[6];

        exprAst *left;
        exprAst *right;

        exprAst() : op(-1), number(0), str(), flags(), left(nullptr), right(nullptr) {}

        exprAst(double x) : op(numberType), number(x), str(), flags(), left(nullptr), right(nullptr) {}

        exprAst(std::string x) : op(stringType), number(0), str(x), flags(), left(nullptr), right(nullptr) {}

        exprAst(int Type, exprAst* left, exprAst* right) : op(Type), number(0), str(), flags(), left(left), right(right) {}

        ~exprAst()
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

/// Clasa pentru transformarea unei liste de tokene intr-un AST expr. 
class parseExpr {
	private:
		int index{}; // used for vector

		// must be 0 after execution
		int parenth_cnt{}, square_cnt{};

		std::pair<std::string, int> token;
		std::vector<std::pair<std::string, int>> tokens;

		void match(int type) {
			if(token.second != type) {
				throw syntaxError;
			}
		}

		std::pair<std::string, int> getNextTokenFromVector() {
			if(index >= (int) tokens.size()) {
				throw syntaxError;
			}
			token = tokens[index];
			index++;
			return token;
		}

		parseExpr(std::vector<std::pair<std::string, int>>&& tokens) : tokens(std::move(tokens)) {}
		parseExpr(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}


		exprAst *exp() {
			if(token.second == token_FLOAT) {
				double number = std::stod(token.first);
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
				nested->flags[exprAst::CMP_BUG_FIX_FLAG] = true;

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
				if(nested->flags[exprAst::NEGATIVE_FLAG]) {
					nested->flags[exprAst::INT_BUG_FIX_FLAG] = true;
					nested->flags[exprAst::NEGATIVE_FLAG] = false;
				}

				match(token_RIGHT_SQUARE);
				token.second = token_FORCE_QUIT;
				return nested;
			}
			else if(token.second == token_MINUS) {
				if(index < (int) tokens.size()) {token = getNextTokenFromVector();}
				else {
					throw syntaxError;
				}

				// while-ul de mai jos e folsit pentru a fixa un bug cand un numar negativ
				// e inmultit cu putere: -(expr)^y = -1 * (expr)^y
				int index_copy = index - 1, np_cnt{};
				bool nextpow{};
				while(index_copy < (int) tokens.size()) {
					int searchtk = tokens[index_copy].second;
					if(searchtk == token_LEFT_PARENTH or searchtk == token_LEFT_SQUARE) {
						np_cnt++;
					}
					else if (searchtk == token_RIGHT_PARENTH or searchtk == token_RIGHT_SQUARE) {
						np_cnt--;
						index_copy++;
						continue;

					}

					if(np_cnt == 0) {
						if(searchtk != token_FLOAT and searchtk != token_IDENTIFIER) {
							if(searchtk == token_POWER) {
								nextpow = true;
								
							}
							break;
						}
					}


					index_copy++;
				}



				exprAst *nested = exp();
				if(nextpow) {
					nested->flags[exprAst::POW_BUG_FIX_FLAG] = true;
				}
				else {

					nested->flags[1] = !(nested->flags[1]);
				}
				token.second = token_FORCE_QUIT;




				return nested;

			}
			else if(token.second == token_PLUS) {
				if(index < (int) tokens.size()) {token = getNextTokenFromVector();}
				else {
					throw syntaxError;
				}
				exprAst *nested = exp();
				token.second = token_FORCE_QUIT;
				return nested;

			}
			else if(token.second == token_NOT) {
				if(index < (int) tokens.size()) {token = getNextTokenFromVector();}
				else {
					throw syntaxError;
				}
				if(token.second != token_LEFT_PARENTH and token.second != token_FLOAT) {
					throw syntaxError;
				}
				else {
					exprAst *nested = exp();
					nested->flags[2] = !(nested->flags[2]);
					token.second = token_FORCE_QUIT;
					return nested;
				}

			}

			throw syntaxError;

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
					throw syntaxError;
				}
				if(token.second != token_POWER) break;

				if(token.second == token_POWER) {
					token = getNextTokenFromVector();

					if((token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						if(exprTree->op == numberType) exprTree = new exprAst(powerType, exprTree, exp());
						else exprTree->right = new exprAst(powerType, exprTree->right ,exp());

					}
					else {
						throw syntaxError;
					}
				}
				else {
					throw syntaxError;
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
						throw syntaxError;
					}
				}
				else if(token.second == token_DIVISION) {
					token = getNextTokenFromVector();

					if((token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						exprTree = new exprAst(divisionType, exprTree, factor());
					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_MODULO) {
					token = getNextTokenFromVector();

					if((token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						exprTree = new exprAst(moduloType, exprTree, factor());
					}
					else {
						throw syntaxError;
					}
				}
				else {
					throw syntaxError;
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
						throw syntaxError;
					}
				}
				else if(token.second == token_MINUS) {
					token = getNextTokenFromVector();



					if((token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						exprTree = new exprAst(minusType, exprTree, term());

					}
					else {
						throw syntaxError;
					}
				}
				else {
					throw syntaxError;
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
						throw syntaxError;
					}
				}
				else if(token.second == token_GREATER_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(greaterEqualType, exprTree, addend());
					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_SMALLER) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(smallerType, exprTree, addend());
					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_SMALLER_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(smallerEqualType, exprTree, addend());

					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(equalType, exprTree, addend());

					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_NOT_EQUAL) {
					token = getNextTokenFromVector();

					if(token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(notEqualType, exprTree, addend());

					}
					else {
						throw syntaxError;
					}
				}
				else {
					throw syntaxError;
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
						throw syntaxError;
					}
				}
				else {
					throw syntaxError;
				}
			}

			return exprTree;

		}

		exprAst *expr() {
			/*
			Arithmetic expression parser.


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
					throw syntaxError;
				}
			}

			if(token.second == token_RIGHT_PARENTH) {
				parenth_cnt--;
			}
			else if(token.second == token_RIGHT_SQUARE) {
				square_cnt--;
			}


			if(token.second != token_FLOAT and token.second != token_RIGHT_PARENTH and token.second != token_STRING and token.second != token_RIGHT_SQUARE and token.second != token_FORCE_QUIT) {
				throw syntaxError;
			}

			return exprTree;
		}

		/// Punct de inceput conversie tokene in expr AST.
		exprAst *convert() {
			exprAst *result = expr();

			if(parenth_cnt != 0 or square_cnt != 0) {
				delete result;
				throw syntaxError;
			}

			return result;
		}

	public:

		/// Functia principala, converteste expresia la un AST
		static exprAst *parseEntry(const std::vector<std::pair<std::string, int>>& tokens) {
			parseExpr Parser(tokens);
			return Parser.convert(); // return type *exprAst
		}
};


// Urmatoarea clasa reprezinta un Ast de tip scrie
class outAst {
	public:
		// Scrie poate avea o insiruire de expresii legate prin ','
		
		std::vector<exprAst*> list_of_exprTree{};
		
		void add(exprAst* exprTree) {
			list_of_exprTree.push_back(exprTree);
		} 
		~outAst() {
			for(int i{}; i < (int) list_of_exprTree.size(); i++) {
				delete list_of_exprTree[i];
			}
		 }
};

class parseOut {
	private:
		int index{};
		std::pair<std::string, int> token;
		std::vector<std::pair<std::string, int>> tokens;
		std::vector<std::pair<std::string, int>> tokensExpr;

		void match(int type) {
			if(token.second != type) {
				throw syntaxError;
			}
		}

		std::pair<std::string, int> getNextTokenFromVector() {
			if(index >= (int) tokens.size()) {
				throw syntaxError;
			}
			token = tokens[index];
			index++;
			return token;
		}


		parseOut(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}

		
		outAst* convert() {
			outAst *outputExprs = new outAst;
			index++;

			while(index < (int) tokens.size()) {
				token = getNextTokenFromVector();

				if(token.second == token_COMMA) {
					outputExprs->add(parseExpr::parseEntry(tokensExpr));

					tokensExpr.clear();
					continue;
				}

				tokensExpr.push_back(token);
			}
			outputExprs->add(parseExpr::parseEntry(tokensExpr));
			tokensExpr.clear();

			return outputExprs;

		}
	
	public:
		static outAst* parseEntry(const std::vector<std::pair<std::string, int>>& tokens) {
			parseOut Parser(tokens);
			return Parser.convert(); // return type outAst
		}

};

