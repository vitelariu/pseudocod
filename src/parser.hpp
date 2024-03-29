#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <type_traits>
#include "lexer.hpp"
#include "errors.hpp"


// Un struct pentru vari si un ubnordered map unde se vor
// tine variabilele
class var {
	public:
		int type{-1};
		int dimensions{};
		// 0 dimensiuni = variabila normala
		// spre ex: a <- 1
		// n dimensiuni = array
		// spre ex: a[100][100][100] <- 0
		// (array cu 3 dimensiuni)
		std::vector<int> dimensions_size{};

		double numberValue{};
		std::string stringValue{};	

		// pentru arrays
		double *arrayD;
		char *arrayS;
	
};
// std::string nume, var variabila
std::unordered_map<std::string, var> variables{};




/// Astea sunt folosite pentru a determina tipul de node din ast-ul de expresii
/// De asemenea se mai folosesc si pentru class varNode
enum types {
	numberType,
	stringType,
	unknownType, // Type that has a special use (it means that can be either numberType or stringType)
	varType,

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


class variable {
	public:
		std::string name{};
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
		variable var;

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

        exprAst *left{};
        exprAst *right{};

        exprAst() : op(-1), number(0), str(), var(), flags(), left(nullptr), right(nullptr) {}

        exprAst(double x) : op(numberType), number(x), str(), var(), flags(),  left(nullptr), right(nullptr) {}

        exprAst(std::string x) : op(stringType), number(0), str(x), var(), flags(), left(nullptr), right(nullptr) {}

        exprAst(int Type, exprAst* left, exprAst* right) : op(Type), number(0), str(), var(), flags(), left(left), right(right) {}
		
		exprAst(variable x) : op(varType), number(), str(), var(x), flags(), left(nullptr), right(nullptr) {} 

        ~exprAst()
        {
        	delete left;
        	delete right;
        }

        friend std::ostream& operator<<(std::ostream& out, const exprAst* const ast)
        {
			if(!ast) out << "TREE-UL DAT NU E VALID\n";
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
		int parenth_cnt{}, square_cnt{}, vertical_bar_cnt{};

		std::pair<std::string, int> token;
		std::vector<std::pair<std::string, int>> tokens;

		void match(int type) {
			if(token.second != type) {
				throw syntaxError;
			}
		}

		void getNextTokenFromVector() {
			if(index >= (int) tokens.size()) {
				throw syntaxError;
			}
			token = tokens[index];
			index++;
		}

		parseExpr(std::vector<std::pair<std::string, int>>&& tokens) : tokens(std::move(tokens)) {}
		parseExpr(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}



		exprAst *exp() {
			if(token.second == token_FLOAT) {
				double number = std::stod(token.first);
				exprAst *nodeNumber = new exprAst(number);

				return nodeNumber;
			}
			else if(token.second == token_IDENTIFIER) {
				var Var = variables[token.first];
				if(Var.type == -1) {
					// Asta inseamna ca nici macar nu exista aceasta variabila si este folosita intr-o expr;
					throw uninitialisedVar;
				}

				variable name;
				name.name = token.first;
				exprAst *nodeVar = new exprAst(name); 
				return nodeVar;
			}
			else if(token.second == token_STRING) {
				std::string str = token.first;
				exprAst *nodeString = new exprAst(str);

				return nodeString;
			}
			else if(token.second == token_LEFT_PARENTH) {

				// daca urmatorul token din lista este )
				if(tokens[index].second == token_RIGHT_PARENTH) {
					getNextTokenFromVector();
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
				
				// daca urmatorul token din lista este ]
				if(tokens[index].second == token_RIGHT_SQUARE) {
					getNextTokenFromVector();
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
				if(index < (int) tokens.size()) {getNextTokenFromVector();}
				else {
					throw syntaxError;
				}

				// while-ul de mai jos e folosit pentru a fixa un bug cand un numar negativ
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
				if(index < (int) tokens.size()) {getNextTokenFromVector();}
				else {
					throw syntaxError;
				}
				exprAst *nested = exp();
				token.second = token_FORCE_QUIT;
				return nested;

			}
			else if(token.second == token_NOT) {
				if(index < (int) tokens.size()) {getNextTokenFromVector();}
				else {
					throw syntaxError;
				}
				if(token.second != token_LEFT_PARENTH and token.second != token_FLOAT and token.second != token_IDENTIFIER) {
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

				getNextTokenFromVector();

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
					getNextTokenFromVector();

					if((token.second == token_IDENTIFIER or token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						if(exprTree->op == numberType or exprTree->op == varType) {exprTree = new exprAst(powerType, exprTree, exp());}
						else {
							exprAst *exprTree_copy = exprTree;
							while(true) {
								if(exprTree_copy->right->right != nullptr) {
									exprTree_copy = exprTree_copy->right;
								}
								else {
									break;
								}
							} 
							exprTree_copy->right = new exprAst(powerType, exprTree_copy->right, exp());
						}

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
					getNextTokenFromVector();

					if(token.second == token_IDENTIFIER or token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT or token.second == token_STRING) {
						exprTree = new exprAst(multiplyType, exprTree, factor());
					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_DIVISION) {
					getNextTokenFromVector();

					if((token.second == token_IDENTIFIER or token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						exprTree = new exprAst(divisionType, exprTree, factor());
					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_MODULO) {
					getNextTokenFromVector();

					if((token.second == token_IDENTIFIER or token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
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
					getNextTokenFromVector();



					if((token.second == token_IDENTIFIER or token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
						exprTree = new exprAst(plusType, exprTree, term());

					}
					else if((token.second == token_IDENTIFIER  or token.second == token_STRING or token.second == token_LEFT_PARENTH) or result->op == stringType) {
						exprTree = new exprAst(plusType, exprTree, term());
					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_MINUS) {
					getNextTokenFromVector();



					if((token.second == token_IDENTIFIER or token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
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

				if((tokens[index - 2].second == token_STRING) ^ (tokens[index].second == token_STRING) and tokens[index - 2].second != token_IDENTIFIER and tokens[index].second != token_IDENTIFIER) {
					throw syntaxError;
				}


				if(token.second == token_GREATER) {
					getNextTokenFromVector();

					if(token.second == token_IDENTIFIER  or token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(greaterType, exprTree, addend());
					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_GREATER_EQUAL) {
					getNextTokenFromVector();

					if(token.second == token_IDENTIFIER  or token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(greaterEqualType, exprTree, addend());
					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_SMALLER) {
					getNextTokenFromVector();

					if(token.second == token_IDENTIFIER  or token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(smallerType, exprTree, addend());
					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_SMALLER_EQUAL) {
					getNextTokenFromVector();

					if(token.second == token_IDENTIFIER  or token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(smallerEqualType, exprTree, addend());

					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_EQUAL) {
					getNextTokenFromVector();

					if(token.second == token_IDENTIFIER  or token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
						exprTree = new exprAst(equalType, exprTree, addend());

					}
					else {
						throw syntaxError;
					}
				}
				else if(token.second == token_NOT_EQUAL) {
					getNextTokenFromVector();

					if(token.second == token_IDENTIFIER or token.second == token_FLOAT or token.second == token_STRING or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_MINUS or token.second == token_PLUS or token.second == token_NOT) {
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
					getNextTokenFromVector();

					if((token.second == token_IDENTIFIER  or token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
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

				if((token.second == token_IDENTIFIER  or token.second == token_FLOAT or token.second == token_LEFT_PARENTH or token.second == token_LEFT_SQUARE or token.second == token_PLUS or token.second == token_NOT) and result->op != stringType) {
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


			if(token.second != token_IDENTIFIER and token.second != token_FLOAT and token.second != token_RIGHT_PARENTH and token.second != token_STRING and token.second != token_RIGHT_SQUARE and token.second != token_FORCE_QUIT) {
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
		static exprAst *parseEntry(std::vector<std::pair<std::string, int>>& tokens) {
			// Prima si prima data trebuie sa verificam daca tokens are "<-"
			// pentru ca suntem in exprAst, vom trata asta ca pe "mai mic negativ" in loc
			// de asignare.
			// Practic cand dam de "<-" (token_ASSIGN) il vom imparti in 2: "<" (token_SMALLER)
			// si "-" (token_MINUS)
			
			for(int i{}; i < (int) tokens.size(); i++) {
				std::pair<std::string, int> x = tokens[i];
				if(x.second == token_ASSIGN) {
					std::pair<std::string, int> last = tokens.back();
					tokens.push_back(last);
					

					tokens[i] = std::make_pair("<", token_SMALLER);
					std::pair<std::string, int> current = tokens[i+1];
					tokens[i+1] = std::make_pair("-", token_MINUS);
					
					for(int j = i + 2; j < (int) tokens.size(); j++) {
						std::pair<std::string, int> copy = tokens[j];
						tokens[j] = current;
						current = copy;
					}
				}
			}

			parseExpr Parser(tokens);


			return Parser.convert();
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

		void getNextTokenFromVector() {
			if(index >= (int) tokens.size()) {
				throw syntaxError;
			}
			token = tokens[index];
			index++;
		}

		parseOut(std::vector<std::pair<std::string, int>>&& tokens) : tokens(std::move(tokens)) {}
		parseOut(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}

		outAst* convert() {
			outAst *outputExprs = new outAst; 
			index++;

			while(index < (int) tokens.size()) {
				getNextTokenFromVector();

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
			return Parser.convert();
		}

};


// Urmatoarea clasa reprezinta o variabila
class varNode {
	public:
		std::string varName{};

		std::vector<std::pair<std::string, int>> expr_for_var;
		// Pointer to exprAst for evaluating the value
		exprAst *expr{}; // folosit si pentru arrays

		// de aici in jos sunt variabile folosite in caz ca acest varNode
		// stocheaza un array, si nu o variabila normala
		
		std::vector<exprAst*> coordinates{}; // size = dimensiuni
		std::vector<exprAst*> values{};


		~varNode() {
			if(expr) delete expr;
		}
};


class parseVar {
	int index{};
	std::pair<std::string, int> token;
	std::vector<std::pair<std::string, int>> tokens;

	void match(int type) {
		if(token.second != type) {
			throw syntaxError;
		}
	}

	void getNextTokenFromVector() {
		if(index >= (int) tokens.size()) {
			throw syntaxError;
		}
		token = tokens[index];
		index++;
	}

	parseVar(std::vector<std::pair<std::string, int>>&& tokens) : tokens(std::move(tokens)) {}
	parseVar(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}

	varNode *convert() {
		varNode *node = new varNode;
		getNextTokenFromVector();
		
		node->varName = token.first;
		var variable;

		// Nu am cum sa stiu daca expr va fi stringType sau numberType pana nu o execut asa ca folosesc unknwonType
		if(variables[node->varName].type == -1) {
			variable.type = unknownType;
			variables[node->varName] = variable;

			getNextTokenFromVector();
			match(token_ASSIGN);


			

			for(int i{2}; i < (int) tokens.size(); i++) {
				(node->expr_for_var).push_back(tokens[i]);
			}

			// if ul asta nu lasa sa se initializeze o variabila cu ea insasi 
			if(node->expr_for_var.size() > 0) {
				for(std::pair<std::string, int> x : node->expr_for_var) {
					if(x.first == node->varName) {
						variable.type = -1;
						variables[node->varName] = variable;
						throw syntaxError;
					}
				}
			}	
			node->expr = parseExpr::parseEntry(node->expr_for_var);

		}
		else {
			getNextTokenFromVector();
			match(token_ASSIGN);


			

			for(int i{2}; i < (int) tokens.size(); i++) {
				(node->expr_for_var).push_back(tokens[i]);
			}
			
			node->expr = parseExpr::parseEntry(node->expr_for_var);
		}


		return node;
	}


	public:
		static varNode* parseEntry(const std::vector<std::pair<std::string, int>>& tokens) {
			parseVar Parser(tokens);
			return Parser.convert();
		}
};


class parseArray {
	int index{};
	std::pair<std::string, int> token;
	std::vector<std::pair<std::string, int>> tokens;

	void match(int type) {
		if(token.second != type) {
			throw syntaxError;
		}
	}

	void getNextTokenFromVector() {
		if(index >= (int) tokens.size()) {
			throw syntaxError;
		}
		token = tokens[index];
		index++;
	}

	parseArray(std::vector<std::pair<std::string, int>>&& tokens) : tokens(std::move(tokens)) {}
	parseArray(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}

	varNode *convert() {
		varNode *node = new varNode;
		getNextTokenFromVector();
		node->varName = token.first;

		getNextTokenFromVector();
		

		// aici ar trebui sa urmeze dimensiunea
		match(token_LEFT_SQUARE);
		while(token.second == token_LEFT_SQUARE) {
			std::vector<std::pair<std::string, int>> dimension{};
			getNextTokenFromVector();
			while(token.second != token_RIGHT_SQUARE) {
				dimension.push_back(token);
				getNextTokenFromVector();
			}
			node->coordinates.push_back(parseExpr::parseEntry(dimension));
			getNextTokenFromVector();
		}
		
		// urmatorul trebuie sa fie <-
		match(token_ASSIGN);
		getNextTokenFromVector();

		
		// Cele 3 tipuri de initializare
		// ... <- 0
		// ... <- {0}
		// ... <- {1, 2, 3}
		
		if(token.second == token_LEFT_CURLY) {
			// iar fiecare expr in parte
			getNextTokenFromVector();
			if(token.second == token_RIGHT_CURLY) {
				if(index != (int) tokens.size()) throw syntaxError;
			}
			while(token.second != token_RIGHT_CURLY) {
				std::vector<std::pair<std::string, int>> expression{};
				while(token.second != token_COMMA and token.second != token_RIGHT_CURLY) {
					expression.push_back(token);
					getNextTokenFromVector();
				}
				node->values.push_back(parseExpr::parseEntry(expression));
			
				if(token.second == token_COMMA) getNextTokenFromVector();
			}

			if((int) node->values.size() == 0) throw syntaxError;

		}
		else {
			// ia totul pana la sfarsit
			std::vector<std::pair<std::string, int>> expression{};
			for(int i = index - 1; i < (int) tokens.size(); i++) {
				expression.push_back(tokens[i]);

			}


			node->expr = parseExpr::parseEntry(expression);
		}

		var variableArr;
		if(variables[node->varName].type == -1) {
			variableArr.type = unknownType;
			variableArr.dimensions = (int) node->coordinates.size();
			variables[node->varName] = variableArr;
		}

		return node;
	}


	public:
		static varNode* parseEntry(const std::vector<std::pair<std::string, int>>& tokens) {
			parseArray Parser(tokens);
			return Parser.convert();
		}

};


class inAst {
	public:
		int type{token_ASSIGN_FLOAT};
		std::vector<std::string> list_of_identifiers{};

		void add(std::string name) {
			list_of_identifiers.push_back(name);
		}
};

class parseIn {
	int index{};
	std::pair<std::string, int> token;
	std::vector<std::pair<std::string, int>> tokens;

	void match(int type) {
		if(token.second != type) {
			throw syntaxError;
		}
	}

	void getNextTokenFromVector() {
		if(index >= (int) tokens.size()) {
			throw syntaxError;
		}
		token = tokens[index];
		index++;
	}


	parseIn(std::vector<std::pair<std::string, int>>&& tokens) : tokens(std::move(tokens)) {}
	parseIn(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}


	inAst *convert() {
		inAst *Tree = new inAst;

		getNextTokenFromVector();
		match(token_INPUT);

		getNextTokenFromVector();
		if(token.second == token_IDENTIFIER) Tree->add(token.first);
		else throw syntaxError;
		if(index < (int) tokens.size()) {
			getNextTokenFromVector();
			while(true) {
				if(index == (int) tokens.size()) {
					if(token.second == token_ASSIGN_FLOAT) {
						Tree->type = token_ASSIGN_FLOAT;
						break;
					}
					else if(token.second == token_ASSIGN_STRING) {
						Tree->type = token_ASSIGN_STRING;
						break;
					}
					else if(token.second == token_ASSIGN_NATURAL) {
						Tree->type = token_ASSIGN_NATURAL;
						break;
					}
					else if(token.second == token_ASSIGN_INT) {
						Tree->type = token_ASSIGN_INT;
						break;
					}
					else if(token.second == token_ASSIGN_BOOL) {
						Tree->type = token_ASSIGN_BOOL;
						break;
					}
					else {throw syntaxError;}
				}
				
				if(token.second != token_COMMA) throw syntaxError;
				getNextTokenFromVector();
				if(token.second == token_IDENTIFIER) Tree->add(token.first);
				else throw syntaxError;

				if(index < (int) tokens.size()) {
					getNextTokenFromVector();
				}
				else {
					break;
				}
			}
		}


		for(std::string id : Tree->list_of_identifiers) {
			var variable;

			// pot de fapt sa determin daca e stringType sau numberType dar nu am nevoie oricum asa ca folosesc unkownType
			variable.type = unknownType;

			variables[id] = variable;
		}


		return Tree;
	}

	public:
		static inAst* parseEntry(const std::vector<std::pair<std::string, int>>& tokens) {
			parseIn Parser(tokens);
			return Parser.convert();
		}
};




// forward declaration
// asta e folosit de statement dar in acelasi timp
// clasa asta poate sa foloseasca indirect clasa statement
// (prin clasa statements)
//
// ex: statements: statement -> whileAst -> statements

class whileAst; 
class forAst;
class doAst;
class ifelseAst;

class statement {
	public:	
		// practic astea is toate pointerele de care ai nevoie pentru a construi
		// tree-ul


		exprAst *exprAst_p{};
		outAst *outAst_p{};
		varNode *varNode_p{};
		inAst *inAst_p{};
		whileAst *whileAst_p{};
		forAst *forAst_p{};
		doAst *doAst_p{};
		ifelseAst *ifelseAst_p{};

		bool isNone = false; // adica e gol
							 // asta va fi true daca pe linie nu exista nimic

		~statement() {
			if(exprAst_p) delete exprAst_p;
			if(outAst_p) delete outAst_p;
			if(varNode_p) delete varNode_p;
			if(inAst_p) delete inAst_p;
		}
};



class statements {
	public:
		std::vector<statement*> s{};
		void add(statement* st) {
			s.push_back(st);
		}
};

class whileAst {
	public:
		exprAst *condition = new exprAst;
		statements *block = new statements;

		~whileAst() {
			delete condition;
			delete block;
		}
};

class parseWhile {
	int index{};
	std::pair<std::string, int> token;
	std::vector<std::pair<std::string, int>> tokens;
	std::vector<std::pair<std::string, int>> tokensCondition;

	void match(int type) {
		if(token.second != type) {
			throw syntaxError;
		}

	}

	void getNextTokenFromVector() {
		if(index >= (int) tokens.size()) {
			throw syntaxError;
		}
		token = tokens[index];
		index++;
	}


	parseWhile(std::vector<std::pair<std::string, int>>&& tokens) : tokens(std::move(tokens)) {}
	parseWhile(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}


	whileAst *convert() {
		whileAst *Tree = new whileAst;
		
		getNextTokenFromVector();
		match(token_WHILE);
		
		
		// de aici ar trebui sa fie o expr pentru conditie
		// si la sfarsit cuvantul "executa"
		while(index < (int) tokens.size() - 1) {
			getNextTokenFromVector();
			tokensCondition.push_back(token);
		}
		getNextTokenFromVector();
		if(token.second != token_EXECUTE) throw forgotExecute;
		
		Tree->condition = parseExpr::parseEntry(tokensCondition);
		
		return Tree;
	}

	public:
		static whileAst *parseEntry(const std::vector<std::pair<std::string, int>>& tokens) {
			parseWhile Parser(tokens);
			return Parser.convert();
		}
};


class forAst {
	public:
		std::string iteratorName{};
		varNode *assign = new varNode;
		exprAst *UpperBoundary = new exprAst;
		varNode *step = new varNode;
		bool default_step = true;

		statements *block = new statements;

		~forAst() {
			delete assign;
			delete UpperBoundary;
			delete step;
			delete block;
		}
};

class parseFor {

	int index{};
	std::pair<std::string, int> token;
	std::vector<std::pair<std::string, int>> tokens{};
	std::vector<std::pair<std::string, int>> tokensAssign{};
	std::vector<std::pair<std::string, int>> tokensUpperBoundary{};
	std::vector<std::pair<std::string, int>> step{};


	void match(int type) {
		if(token.second != type) {
			throw syntaxError;
		}

	}

	void getNextTokenFromVector() {
		if(index >= (int) tokens.size()) {
			throw syntaxError;
		}
		token = tokens[index];
		index++;
	}


	parseFor(std::vector<std::pair<std::string, int>>&& tokens) : tokens(std::move(tokens)) {}
	parseFor(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}

	forAst *convert() {
		forAst *Tree = new forAst;

		getNextTokenFromVector();
		match(token_FOR);
	
		getNextTokenFromVector();


		std::pair<std::string, int> iterator = token;
		std::pair<std::string, int> assignSign = std::make_pair("<-", token_ASSIGN);
		step.push_back(iterator);
		step.push_back(assignSign);
		Tree->iteratorName = iterator.first;

		// asignarea
		while(token.second != token_COMMA) {
			tokensAssign.push_back(token);

			getNextTokenFromVector();
		}

		if((int) tokensAssign.size() == 1 and (variables[tokensAssign[0].first].type == unknownType or variables[tokensAssign[0].first].type == numberType)) {
			tokensAssign.push_back(assignSign);
			tokensAssign.push_back(iterator);
		}

		Tree->assign = parseVar::parseEntry(tokensAssign);


		// limita sus
		getNextTokenFromVector();
		while(token.second != token_COMMA and index < (int) tokens.size()) {
			tokensUpperBoundary.push_back(token);
			getNextTokenFromVector();


		}

		Tree->UpperBoundary = parseExpr::parseEntry(tokensUpperBoundary);





		if(token.second == token_EXECUTE) {
			step.push_back(iterator);
			step.push_back(std::make_pair("+", token_PLUS));
			step.push_back(std::make_pair("1", token_FLOAT));
			Tree->step = parseVar::parseEntry(step);
		}
		else {
			Tree->default_step = false;
			getNextTokenFromVector();	
			while(index < (int) tokens.size()) {
				step.push_back(token);
				getNextTokenFromVector();
			}

			Tree->step = parseVar::parseEntry(step);
			
			if(token.second != token_EXECUTE) {
				throw forgotExecute;
			}


		}
		return Tree;
	}

	public:
		static forAst *parseEntry(const std::vector<std::pair<std::string, int>>& tokens) {
			parseFor Parser(tokens);
			return Parser.convert();
		}
};


class doAst {
	public:
		exprAst *condition = new exprAst;
		statements *block = new statements;

		bool isCondition = false;

		~doAst() {
			delete condition;
			delete block;
		}
};

class parseDo {
	int index{};
	std::pair<std::string, int> token;
	std::vector<std::pair<std::string, int>> tokens;
	std::vector<std::pair<std::string, int>> tokensCondition;


	void match(int type) {
		if(token.second != type) {
			throw syntaxError;
		}

	}

	void getNextTokenFromVector() {
		if(index >= (int) tokens.size()) {
			throw syntaxError;
		}
		token = tokens[index];
		index++;
	}


	parseDo(std::vector<std::pair<std::string, int>>&& tokens) : tokens(std::move(tokens)) {}
	parseDo(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}
	

	doAst *convert(doAst *Tree) {
		getNextTokenFromVector();
		match(token_WHILE);

		while(index < (int) tokens.size()) {
			getNextTokenFromVector();
			tokensCondition.push_back(token);
		}

		Tree->condition = parseExpr::parseEntry(tokensCondition);
		Tree->isCondition = true;
		return Tree;
	}

	public:
		static doAst *parseEntry(const std::vector<std::pair<std::string, int>>& tokens, doAst *Tree) {
			parseDo Parser(tokens);
			return Parser.convert(Tree);
		}

};


class ifelseAst {
	public:
		std::vector<exprAst*> blocksExpr{};
		std::vector<statements*> blocks{};

		statements *blockElse{};
		
		~ifelseAst() {
			for(exprAst *x : blocksExpr) {
				delete x;
			}
			for(statements *x : blocks) {
				delete x;
			}
			if(blockElse) delete blockElse;
		}

};


class parseIfelse {
	int index{};
	std::pair<std::string, int> token;
	std::vector<std::pair<std::string, int>> tokens;
	std::vector<std::pair<std::string, int>> tokensCondition;



	void match(int type) {
		if(token.second != type) {
			throw syntaxError;
		}

	}

	void getNextTokenFromVector() {
		if(index >= (int) tokens.size()) {
			throw syntaxError;
		}
		token = tokens[index];
		index++;
	}


	parseIfelse(std::vector<std::pair<std::string, int>>&& tokens) : tokens(std::move(tokens)) {}
	parseIfelse(const std::vector<std::pair<std::string, int>>& tokens) : tokens(tokens) {}
	

	ifelseAst *convert(ifelseAst *Tree) {
		getNextTokenFromVector();
		getNextTokenFromVector();
		
		while(index < (int) tokens.size()) {
			tokensCondition.push_back(token);
			getNextTokenFromVector();
		}
		match(token_THEN);

		exprAst *condition = parseExpr::parseEntry(tokensCondition);
		Tree->blocksExpr.push_back(condition);

		statements *newblock = new statements;
		Tree->blocks.push_back(newblock);
		
		return Tree;
	}



	public:
		static ifelseAst *parseEntryIf(const std::vector<std::pair<std::string, int>>& tokens) {
			parseIfelse Parser(tokens);
			ifelseAst *Tree = new ifelseAst;
			return Parser.convert(Tree);
		}
		static ifelseAst *parseEntryElseIf(const std::vector<std::pair<std::string, int>>& tokens, ifelseAst *Tree) {
			parseIfelse Parser(tokens);
			return Parser.convert(Tree);
		}
};











// Functii auxiliare de care am nevoie pentru arbori


// nu se copie intreg tree-ul, ci doar exprAst din el
// (doar alea se modifica in execute)
namespace CopyTree {
	
	void copy_exprAst(exprAst *OriginalTree, exprAst *NewTree) {
		if(OriginalTree->left) {
			NewTree->left = new exprAst(*OriginalTree->left);
			copy_exprAst(OriginalTree->left, NewTree->left);
		}	
		if(OriginalTree->right) {
			NewTree->right = new exprAst(*OriginalTree->right);
			copy_exprAst(OriginalTree->right, NewTree->right);
		}
	}

		
	// [arborele original din care se copie] [noul arbore]
	void Entry(statements *OriginalTree, statements *NewTree) {


		for(int i{}; i < (int) OriginalTree->s.size(); i++) {
			// Luam fiecare tip in parte si apelam la copyExprAst
			

			if(OriginalTree->s[i]->exprAst_p) {

				exprAst *expr = new exprAst(*OriginalTree->s[i]->exprAst_p);
				CopyTree::copy_exprAst(OriginalTree->s[i]->exprAst_p, expr);
				statement *line = new statement(*OriginalTree->s[i]);
				line->exprAst_p = expr;
				NewTree->s[i] = line;
			}
			else if(OriginalTree->s[i]->varNode_p) {

				exprAst *expr = new exprAst(*OriginalTree->s[i]->varNode_p->expr);
				CopyTree::copy_exprAst(OriginalTree->s[i]->varNode_p->expr, expr);
				varNode *asignare = new varNode(*OriginalTree->s[i]->varNode_p);
				asignare->expr = expr;
				statement *line = new statement(*OriginalTree->s[i]);
				line->varNode_p = asignare;
				NewTree->s[i] = line;
			}
			else if(OriginalTree->s[i]->outAst_p) {
				outAst *out = new outAst();
				for(exprAst *OriginalExpr : OriginalTree->s[i]->outAst_p->list_of_exprTree) {
					exprAst *expr = new exprAst(*OriginalExpr);
					CopyTree::copy_exprAst(OriginalExpr, expr);
					out->add(expr);
				}
				statement *line = new statement(*OriginalTree->s[i]);
				line->outAst_p = out;
				NewTree->s[i] = line;
			}
		}
	}
}

bool isNone(std::vector<statement*> s) {
	for(statement *x : s) {
		if(x->isNone == false) return false;	
	}
	return true;
}

// returneaza numarul de linii pentru un vector cu statement
// (nu, nu va fi doar size-ul vectorului, pentru ca un statement
// poate contine "statements" care l-a randul lui sa contine o serie
// de statement)
int getAllLinesNumber(std::vector<statement*> v) {
	int rez{};
	for(statement *x : v) {
		if(x->whileAst_p) {
			rez += getAllLinesNumber(x->whileAst_p->block->s) + 1;
		}
		else if(x->doAst_p) {
			rez += getAllLinesNumber(x->doAst_p->block->s) + 2;
		}
		else if(x->forAst_p) {
			rez += getAllLinesNumber(x->forAst_p->block->s) + 1;
		}
		else if(x->ifelseAst_p) {
			for(statements *ifelseS : x->ifelseAst_p->blocks) {
				rez += getAllLinesNumber(ifelseS->s) + 1;
			}
			if(x->ifelseAst_p->blockElse) {
				rez += getAllLinesNumber(x->ifelseAst_p->blockElse->s) + 1;
			}
		}
		else {
			rez++;
		}
	}

	return rez;
}

bool isArrayAssign(std::vector<std::pair<std::string, int>>& tokens) {
	int start = 1;
	while(start < (int) tokens.size() and tokens[start].second != token_ASSIGN) {
		if(tokens[start].second != token_LEFT_SQUARE) {
			return false;
		}
		else {
			start++;
			while(tokens[start].second != token_RIGHT_SQUARE and start < (int) tokens.size()) {
				start++;
			}
			if(tokens[start].second != token_RIGHT_SQUARE) return false;
		}
		
		start++;
	}
	return true;
}
