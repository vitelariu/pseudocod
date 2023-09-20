#include <iostream>
#include <cmath>
#include "parser.h"


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
void checkFlags(exprAst *Tree, double result) {
	if(Tree->flags[exprAst::INT_FLAG]) {
		result = (int) result;
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

bool checkIfChildIsString(exprAst *node) {
	return node->op == stringType;
}

int getLength(std::string x) { // Pare ca se sterg prea multe caractere, pentru "\\" da 0 dar ar trebui sa dea 1
	int raw_l = x.length() - 2;
	int escape_cnt{};
	for(int i{}; i < (int) x.length(); i++) {
		if(x[i] == '\\') escape_cnt++;
	}
	return raw_l - escape_cnt;
}

std::string multiply_str(std::string x, double number) {
	if(number != (int) number) {
		throw "eroare inmultire sir caractere cu numar (neintreg): "+std::to_string(number);
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
				throw "Eroare comparare a unui sir de caractere cu un numar";
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
				std::cout << "eroare123\n";
				break;
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
				std::cout << "eroare123\n";
				break;
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
				std::cout << "eroare123\n";
				break;
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
				std::cout << "eroare123\n";
				break;
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
				std::cout << "eroare123\n";
				break;
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
				// eroare, avem string + numar
				std::cout << "eroare string + numar\n";
				break;
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
				std::cout << "eroare de sintaxa\n";
				break;
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
				std::cout << "eroare impartire la 0";

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
				std::cout << "Operatie interzisa\n";
				break;
			}
			result = (int) Tree->left->number % (int) Tree->right->number;

			Tree->op = numberType;
			checkFlags(Tree, result);
			clean(Tree);
			break;

		case powerType:
			interpret(Tree->left);
			interpret(Tree->right);
			result = pow(Tree->left->number, Tree->right->number);

			Tree->op = numberType;
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
