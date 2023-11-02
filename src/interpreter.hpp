#include <iostream>
#include <cmath>
#include <unordered_map>
#include "parser.hpp"



// 0 - not used
// 1 - true
// -1 - false
int isComparationTrue{};


/** Functie care returneaza string-ul din ghilimele si size-ul string-ului
 ex: ii dai "a" si iti da a, 1
 chestii precum "\\" se transforma in \, 1
 -1 ca size inseamna eroare
*/
std::string getString(const std::string& n) {
	std::string new_str{};
	for(int i{1}; i < (int) n.length() - 1; i++) {
		if(n[i] == '\\') {
			char next = n[++i];
			switch(next) {
				case '\\':
					new_str += '\\';
					break;
				case '\"':
					new_str += '\"';
					break;
				case '\'':
					new_str += '\'';
					break;
				case 't':
					new_str += '\t';
					break;
				case '?':
					new_str += '\?';
					break;
				case 'a':
					new_str += '\a';
					break;
				case 'b':
					new_str += '\b';
					break;
				case 'f':
					new_str += '\f';
					break;
				case 'n':
					new_str += '\n';
					break;
				case 'r':
					new_str += '\r';
					break;
				case 'v':
					new_str += '\v';
					break;
				default:
					break;
			}

		}
		else {
			new_str += n[i];
		}
	}
	return new_str;
}

/** Returneaza sirul de caractere fara delimitator (""), dar nu verifica inainte
 @return Sirul de caractere fara primul si ultimul caractere
*/
std::string getStringRaw(const std::string& n) {
	std::string new_str{};
	for(int i{1}; i < (int) n.length() - 1; i++) {
		new_str += n[i];
	}

	return new_str;
}

/// Functie care verifica diferite flag-uri
namespace interpretExpr {
	void checkFlags(exprAst *Tree, double result) {
		if(Tree->flags[exprAst::INT_FLAG]) {
			if(!Tree->flags[exprAst::INT_BUG_FIX_FLAG]) {
				if(result >= 0 or (result < 0 and result == (int) result)) {result = (int) result;}
				else {result = (int) result; result--;}
			}
			else {
				result *= -1;
				if(result >= 0 or (result < 0 and result == (int) result)) {result = (int) result;}
				else {result = (int) result; result--;}
			}
		}
		if(Tree->flags[exprAst::NEGATIVE_FLAG]) {
			result *= -1;
		}
		if(Tree->flags[exprAst::NOT_GATE_FLAG]) {
			result = !result;
		}
		if(Tree->flags[exprAst::CMP_BUG_FIX_FLAG]) {
			if(isComparationTrue == 1) result = 1;
			else if(isComparationTrue == -1) result = 0;

			isComparationTrue = 0;
		}
		Tree->number = result;
	}



	std::string multiply_str(std::string x, double number) {
		if(number != (int) number) {
			throw syntaxError;
		}

		std::string final_str = "\"";
		std::string pure_str=getStringRaw(x);

		for(int i{}; i < (int) number; i++) {
			final_str += pure_str;
		}
		final_str += "\"";

		return final_str;
	}

	// Ar fi bine sa se poata intelege ce face functia asta de cineva care nu a apucat sa lucreze la proiect foarte mult
	void aux(exprAst *node) {
		if(isComparationTrue == 1) {
			node->op = numberType;
			node->number = 1;
		}
		else if(isComparationTrue == -1) {
			node->op = numberType;
			node->number = 0;
		}
		isComparationTrue = 0;
	}

	void clean(exprAst *Tree){
		delete Tree->left;
		delete Tree->right;
		Tree->left = nullptr;
		Tree->right = nullptr;
	}

	/// Interpreteaza AST-ul primit.
	void interpret(exprAst *Tree) {
		if(Tree->op == numberType) {
			checkFlags(Tree, Tree->number);
			return;
		}
		else if(Tree->op == stringType) {
			return;
		}

		double result{};


		switch(Tree->op) {
			case orType:

				interpret(Tree->left);
				aux(Tree->left);

				// Chestia asta poate scapa de sub control, asa ca o sa facem o mica optimizare
				// interpret(Tree->right);
				// aux(Tree->right);

				if(Tree->left->number)
					result = true;
				else
				{
					interpret(Tree->right);
					aux(Tree->right);

					result = (bool) Tree->right->number;
				}

				Tree->op = numberType;
				checkFlags(Tree, result);
				clean(Tree);
				break;

			case andType:
				interpret(Tree->left);
				aux(Tree->left);

				// Chestia asta poate scapa de sub control, asa ca o sa facem o mica optimizare
				// interpret(Tree->right);
				// aux(Tree->right);

				if(Tree->left->number)
				{
					interpret(Tree->right);
					aux(Tree->right);

					result = (bool) Tree->right->number;
				}
				else
					result = false;

				Tree->op = numberType;
				checkFlags(Tree, result);
				clean(Tree);
				break;

			case greaterType:
				interpret(Tree->left);
				interpret(Tree->right);

				Tree->op = numberType;

				if((Tree->left->op == stringType) ^ (Tree->right->op == stringType)) {
					throw syntaxError;
				}
				else if(Tree->left->op == stringType and Tree->right->op == stringType) {
					if(getString(Tree->left->str)[0] > getString(Tree->right->str)[0]) {
						checkFlags(Tree, 1);
					}
					else {
						checkFlags(Tree, 0);
					}
				}
				else {
					if(isComparationTrue == 0 or isComparationTrue == 1) {
						if(Tree->left->number > Tree->right->number) {
							isComparationTrue = 1;
						}
						else {
							isComparationTrue = -1;
						}
					}
					checkFlags(Tree, Tree->right->number);
				}


				clean(Tree);
				break;

			case greaterEqualType:
				interpret(Tree->left);
				interpret(Tree->right);

				Tree->op = numberType;

				Tree->op = numberType;
				if((Tree->left->op == stringType) ^ (Tree->right->op == stringType)) {
					throw syntaxError;
				}
				else if(Tree->left->op == stringType and Tree->right->op == stringType) {
					if(getString(Tree->left->str)[0] >= getString(Tree->right->str)[0]) {
						checkFlags(Tree, 1);
					}
					else {
						checkFlags(Tree, 0);
					}
				}
				else {
					if(isComparationTrue == 0 or isComparationTrue == 1) {
						if(Tree->left->number >= Tree->right->number) {
							isComparationTrue = 1;
						}
						else {
							isComparationTrue = -1;
						}
					}
					checkFlags(Tree, Tree->right->number);
				}



				clean(Tree);

				break;

			case smallerType:
				interpret(Tree->left);
				interpret(Tree->right);

				Tree->op = numberType;
				if((Tree->left->op == stringType) ^ (Tree->right->op == stringType)) {
					throw syntaxError;
				}
				else if(Tree->left->op == stringType and Tree->right->op == stringType) {
					if(getString(Tree->left->str)[0] < getString(Tree->right->str)[0]) {
						checkFlags(Tree, 1);
					}
					else {
						checkFlags(Tree, 0);
					}
				}
				else {
					if(isComparationTrue == 0 or isComparationTrue == 1) {
						if(Tree->left->number < Tree->right->number) {
							isComparationTrue = 1;
						}
						else {
							isComparationTrue = -1;
						}
					}
					checkFlags(Tree, Tree->right->number);
				}




				clean(Tree);

				break;

			case smallerEqualType:
				interpret(Tree->left);
				interpret(Tree->right);

				Tree->op = numberType;
				if((Tree->left->op == stringType) ^ (Tree->right->op == stringType)) {
					throw syntaxError;
				}
				else if(Tree->left->op == stringType and Tree->right->op == stringType) {
					if(getString(Tree->left->str)[0] <= getString(Tree->right->str)[0]) {
						checkFlags(Tree, 1);
					}
					else {
						checkFlags(Tree, 0);
					}
				}
				else {
					if(isComparationTrue == 0 or isComparationTrue == 1) {
						if(Tree->left->number <= Tree->right->number) {
							isComparationTrue = 1;
						}
						else {
							isComparationTrue = -1;
						}
					}
					checkFlags(Tree, Tree->right->number);
				}

				clean(Tree);

				break;

			case equalType:
				interpret(Tree->left);
				interpret(Tree->right);

				Tree->op = numberType;

				if((Tree->left->op == stringType) ^ (Tree->right->op == stringType)) {
					throw syntaxError;
				}
				else if(Tree->left->op == stringType and Tree->right->op == stringType) {
					if(getString(Tree->left->str) == getString(Tree->right->str)) {
						checkFlags(Tree, 1);
					}
					else {
						checkFlags(Tree, 0);
					}
				}
				else {
					if(isComparationTrue == 0 or isComparationTrue == 1) {
						if(Tree->left->number == Tree->right->number) {
							isComparationTrue = 1;
						}
						else {
							isComparationTrue = -1;
						}
					}


					checkFlags(Tree, Tree->right->number);
				}

				clean(Tree);
				break;

			case notEqualType:
				interpret(Tree->left);
				interpret(Tree->right);

				Tree->op = numberType;
				if((Tree->left->op == stringType) ^ (Tree->right->op == stringType)) {
					throw syntaxError;
				}
				else if(Tree->left->op == stringType and Tree->right->op == stringType) {
					if(getString(Tree->left->str) != getString(Tree->right->str)) {
						checkFlags(Tree, 1);
					}
					else {
						checkFlags(Tree, 0);
					}
				}
				else {
					if(isComparationTrue == 0 or isComparationTrue == 1) {
						if(Tree->left->number != Tree->right->number) {
							isComparationTrue = 1;
						}
						else {
							isComparationTrue = -1;
						}
					}
					checkFlags(Tree, Tree->right->number);
				}

				clean(Tree);


				break;

			case plusType:
				interpret(Tree->left);
				interpret(Tree->right);

				if((Tree->left->op == stringType) ^ (Tree->right->op == stringType)) {
					throw syntaxError;
				}

				if(Tree->left->op == stringType and Tree->left->op == stringType) {
					Tree->op = stringType;
					Tree->str = "\"" + getStringRaw(Tree->left->str) + getStringRaw(Tree->right->str) + "\"";

					clean(Tree);

					break;
				}

				result = Tree->left->number + Tree->right->number;

				Tree->op = numberType;

				checkFlags(Tree, result);
				clean(Tree);
				break;

			case minusType:
				interpret(Tree->left);
				interpret(Tree->right);
				result = Tree->left->number - Tree->right->number;

				Tree->op = numberType;
				

				checkFlags(Tree, result);

				clean(Tree);
				break;

			case multiplyType:
				interpret(Tree->left);
				interpret(Tree->right);
				if(Tree->left->op == stringType and Tree->right->op == numberType) {
					Tree->op = stringType;
					Tree->str = multiply_str(Tree->left->str, Tree->right->number);
					clean(Tree);
					break;
				}
				else if(Tree->right->op == stringType and Tree->left->op == numberType) {
					Tree->op = stringType;
					Tree->str = multiply_str(Tree->right->str, Tree->left->number);
					clean(Tree);
					break;

				}
				else if(Tree->left->op == stringType or Tree->right->op == stringType) {
					throw syntaxError;
				}

				result = Tree->left->number * Tree->right->number;


				Tree->op = numberType;
				checkFlags(Tree, result);
				clean(Tree);
				break;
			case divisionType:
				interpret(Tree->left);
				interpret(Tree->right);
				if(Tree->right->number == 0) {
					throw divisionByZero;

					return;
				}
				result = Tree->left->number / Tree->right->number;

				Tree->op = numberType;
				checkFlags(Tree, result);
				clean(Tree);
				break;
			case moduloType:
				interpret(Tree->left);
				interpret(Tree->right);
				if(Tree->left->number != (int) Tree->left->number or Tree->right->number != (int) Tree->right->number or Tree->right->number == 0) {
					throw divisionByZero;
				}
				result = (int) Tree->left->number % (int) Tree->right->number;

				Tree->op = numberType;
				checkFlags(Tree, result);
				clean(Tree);
				break;

			case powerType:
				bool negative = false;

				interpret(Tree->left);
				interpret(Tree->right);
				result = pow(Tree->left->number, Tree->right->number);
				if(Tree->left->flags[exprAst::POW_BUG_FIX_FLAG]) {
					negative = true;
				}

				
				Tree->op = numberType;
				if(negative) result *= -1;  
				checkFlags(Tree, result);
				clean(Tree);
				break;

		}


		return;
	}

	void interpretEntry(exprAst *Tree) {
		interpret(Tree);
		if(isComparationTrue) {
			aux(Tree);
			checkFlags(Tree, Tree->number);
		}



	}
}

namespace interpretOut {
	

	void interpretEntry(outAst *Tree) {

		for(int i{}; i < (int) Tree->list_of_exprTree.size(); i++) {
			interpretExpr::interpretEntry(Tree->list_of_exprTree[i]);
			
			if(Tree->list_of_exprTree[i]->op == numberType) {std::cout << Tree->list_of_exprTree[i]->number;}
			else {
				std::string rez = Tree->list_of_exprTree[i]->str;
				rez = getString(rez); 
				std::cout << rez;
			}


			delete Tree->list_of_exprTree[i];
			Tree->list_of_exprTree[i] = nullptr;
		}




	}
}

namespace interpretVar {


	void interpretEntry(varNode *node) {
		// Prima data interpreteaza expresia care va fi atribuita variabilei
		interpretExpr::interpretEntry(node->expr);
		var variabile{};

		if(node->expr->op == numberType) {
			variabile.type = numberType;
			variabile.numberValue = node->expr->number;
		}
		else {
			variabile.type = stringType;
			variabile.stringValue = node->expr->str;
		}

		variables[node->varName] = variabile;
	
	}
}

namespace interpretIn {
	void interpretEntry(inAst *Tree) {
		for(int i{}; i < (int) Tree->list_of_identifiers.size(); i++) {
			std::string name = Tree->list_of_identifiers[i];
			var variable;
			
			std::string xstr{};
			double xdou{};

			if(Tree->type == token_ASSIGN_FLOAT) {
				std::cin >> xdou;
				variable.type = numberType;
				variable.numberValue = xdou;
			}
			else {
				std::getline(std::cin, xstr);
				variable.type = stringType;
				variable.stringValue = '"' + xstr + '"';

			}

			variables[name] = variable;
		}
	}
}
