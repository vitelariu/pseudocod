#include <iostream>
#include <cmath>
#include <unordered_map>
#include <limits>
#include <cstring>
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

	// verifica daca toate flag-urile sunt false, pentru ca daca este vreo unu adevarat
	// iar el e aplicat pe un string, atunci ar trebui sa dea eroare
	void checkIfTrueFlagsString(exprAst *Tree) {
		if(Tree->flags[exprAst::INT_FLAG]) {
			throw syntaxError;
		}
		if(Tree->flags[exprAst::NEGATIVE_FLAG]) {
			throw syntaxError;
		}
		if(Tree->flags[exprAst::NOT_GATE_FLAG]) {
			throw syntaxError;
		}
		if(Tree->flags[exprAst::CMP_BUG_FIX_FLAG]) {
			throw syntaxError;
		}

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
			checkIfTrueFlagsString(Tree);
			return;
		}
		else if(Tree->op == varType) {
			var Var = variables[(Tree->var).name];

			if(Var.type == numberType) {
				double numFromVar = (variables[(Tree->var).name]).numberValue;
				checkFlags(Tree, numFromVar);
				Tree->op = numberType;
			}
			else {
				checkIfTrueFlagsString(Tree);
				std::string strFromVar = (variables[(Tree->var).name]).stringValue;
				Tree->str = strFromVar;
				Tree->op = stringType;

			}

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
					if(Tree->left->op == stringType or Tree->right->op == stringType) throw syntaxError; 

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

					if(Tree->left->op == stringType or Tree->right->op == stringType) throw syntaxError; 

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
					if(getString(Tree->left->str).compare(getString(Tree->right->str)) > 0) {
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
					if(getString(Tree->left->str).compare(getString(Tree->right->str)) >= 0) {
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
					if(getString(Tree->left->str).compare(getString(Tree->right->str)) < 0) {
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
					if(getString(Tree->left->str).compare(getString(Tree->right->str)) <= 0) {
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
					if(getString(Tree->left->str).compare(getString(Tree->right->str)) == 0) {
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
					if(getString(Tree->left->str).compare(getString(Tree->right->str)) != 0) {
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


				if(Tree->left->op == stringType or Tree->right->op == stringType) throw syntaxError; 

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


				if(Tree->left->op == stringType or Tree->right->op == stringType) throw syntaxError; 

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

				if(Tree->left->op == stringType or Tree->right->op == stringType) throw syntaxError; 
				if(Tree->left->number != (int) Tree->left->number or Tree->right->number != (int) Tree->right->number) {
					throw syntaxError;
				}
				else if(Tree->right->number == 0) {
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

				if(Tree->left->op == stringType or Tree->right->op == stringType) throw syntaxError; 

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
	
	// god please forgive me for this atrocity
	void interpretEntry(exprAst *Tree) {
		interpret(Tree);
		if(isComparationTrue) {
			aux(Tree);
			checkFlags(Tree, Tree->number);
		}
	}
	int interpretEntryReturnInt(exprAst *Tree) {
		interpret(Tree);
		if(isComparationTrue) {
			aux(Tree);
			checkFlags(Tree, Tree->number);
		}
		if(Tree->op == numberType) {
			return Tree->number;
		}
		else {
			throw syntaxError;
		}
	}
	double interpretEntryReturnDouble(exprAst *Tree) {
		interpret(Tree);
		if(isComparationTrue) {
			aux(Tree);
			checkFlags(Tree, Tree->number);
		}
		if(Tree->op == numberType) {
			return Tree->number;
		}
		else {
			throw syntaxError;
		}
	}
}

namespace interpretOut {
	

	void interpretEntry(outAst *Tree) {

		for(int i{}; i < (int) Tree->list_of_exprTree.size(); i++) {
			interpretExpr::interpretEntry(Tree->list_of_exprTree[i]);

			
			if(Tree->list_of_exprTree[i]->op == numberType) {std::cout << Tree->list_of_exprTree[i]->number; }
			else {
				std::string rez = Tree->list_of_exprTree[i]->str;
				rez = getString(rez); 
				std::cout << rez;
			}


			//delete Tree->list_of_exprTree[i];
			//Tree->list_of_exprTree[i] = nullptr;
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

	void interpretEntryArray(varNode *node) {
		unsigned long long sizeArray{1};
		
		for(int i{}; i < (int) node->coordinates.size(); i++) {
			interpretExpr::interpretEntry(node->coordinates[i]);

			if(node->coordinates[i]->op == numberType) {
				variables[node->varName].dimensions_size.push_back(node->coordinates[i]->number);
				sizeArray *= node->coordinates[i]->number;
			}
			else {
				throw syntaxError;
			}
		}


		if(variables[node->varName].type == numberType) {	
			if(node->expr != nullptr) {
				interpretExpr::interpretEntry(node->expr);
				if(variables[node->varName].type != node->expr->op) throw syntaxError;
				if((int) node->coordinates.size() != variables[node->varName].dimensions) throw syntaxError;
				
				unsigned long long product = 1;
				unsigned long long rez{};
				for(int i{}; i < (int) node->coordinates.size(); i++) {
					product *= (node->coordinates[i]->number + 1);
				}

				for(int i{}; i < (int) node->coordinates.size(); i++) {
					product /= (node->coordinates[i]->number + 1);
					rez += (node->coordinates[i]->number + 1) * product; 
				}
				*(variables[node->varName].arrayD+rez-1) = node->expr->number;


			}
			else {
				for(unsigned long long i{}; i < node->values.size(); i++) {
					interpretExpr::interpretEntry(node->values[i]);
					if(variables[node->varName].type != node->values[i]->op) throw syntaxError;
				}


				if(node->values[0]->op == numberType) {
					if((int) node->coordinates.size() != variables[node->varName].dimensions) throw syntaxError;
				
					unsigned int product = 1;
					int rez{};
					for(int i{}; i < (int) node->coordinates.size(); i++) {
						product *= (node->coordinates[i]->number + 1);
					}

					for(int i{}; i < (int) node->coordinates.size(); i++) {
						product /= (node->coordinates[i]->number + 1);
						rez += (node->coordinates[i]->number + 1) * product; 
					}
					for(unsigned long long i{}; i < node->values.size(); i++) {
						*(variables[node->varName].arrayD+rez-1+i) = node->values[i]->number;
					}
				}
			}
		}
		else if(variables[node->varName].type == stringType) {
			if(node->expr != nullptr) {
				interpretExpr::interpretEntry(node->expr);
				if(variables[node->varName].type != node->expr->op) throw syntaxError;
				if((int) node->coordinates.size() != variables[node->varName].dimensions + 1 and (int) node->coordinates.size() != variables[node->varName].dimensions) throw syntaxError;
				
				unsigned long long product = 1;
				unsigned long long rez{};
				for(int i{}; i < (int) node->coordinates.size(); i++) {
					product *= (node->coordinates[i]->number + 1);
				}

				for(int i{}; i < (int) node->coordinates.size(); i++) {
					product /= (node->coordinates[i]->number + 1);
					rez += (node->coordinates[i]->number + 1) * product; 
				}
				*(variables[node->varName].arrayD+rez-1) = node->expr->number;


			}

		}
		else {
			if(node->expr != nullptr) {
				interpretExpr::interpretEntry(node->expr);
				variables[node->varName].type = node->expr->op;
				if(node->expr->op == numberType) {
					variables[node->varName].arrayD = (double*) malloc(sizeArray * sizeof(double));	
					if(variables[node->varName].arrayD) {
						for(unsigned long long j{}; j < sizeArray; j++) {
							*(variables[node->varName].arrayD+j) = node->expr->number;
						}
					}
					else {
						throw syntaxError;
					}
				}
				else {
					variables[node->varName].arrayS = (char*) malloc(sizeArray * sizeof(char) * 2048);
					if(variables[node->varName].arrayS) {
						for(unsigned long long j{}; j < sizeArray; j++) {
							strcpy(variables[node->varName].arrayS+(j*2048), node->expr->str.c_str());		
						}
					}
					else {
						throw syntaxError;
					}				
				}
			}
			else {
				interpretExpr::interpretEntry(node->values[0]);
				variables[node->varName].type = node->values[0]->op;

				for(int j{1}; j < (int) node->values.size(); j++) {
					interpretExpr::interpretEntry(node->values[j]);
					if(node->values[j]->op != variables[node->varName].type) throw syntaxError;
				}

				if(node->values[0]->op == numberType) {
					variables[node->varName].arrayD = (double*) malloc(sizeArray * sizeof(double));	
					if(variables[node->varName].arrayD) {
						for(unsigned long long j{}; j < node->values.size(); j++) {
							*(variables[node->varName].arrayD+j) = node->values[j]->number;
						}
					}
					else {
						throw syntaxError;
					}
				}
				else {
					variables[node->varName].arrayS = (char*) malloc(sizeArray * sizeof(char) * 2048);
					if(variables[node->varName].arrayS) {
						for(unsigned long long j{}; j < node->values.size(); j++) {
							strcpy(variables[node->varName].arrayS+(j*2048), node->values[j]->str.c_str());		
						}
					}
					else {
						throw syntaxError;
					}
				}
			}
		}
	}
}



std::string buffer{};
namespace interpretIn {

	void cutbuffer(int i) {
		std::string newbuffer{};
		while(i < (int) buffer.size()) {
			newbuffer += buffer[i];
			i++;
		}
		buffer = newbuffer;
	}
	
	// returneaza urmatorul numar din buffer
	double getNextNumberFromBuffer() {
		int i{};
		double number{};

		// skip spaces;
		while(buffer[i] == ' ' or buffer[i] == '\t') {
			i++;
		}

		// Vericam daca primul caracter nu e + sau -
		// daca e, incepem de la urmatorul iar la sfarsit inmultim numarul cu -1 daca e nevoie
		bool is_negative = false;
		if(buffer[i] == '+' or buffer[i] == '-') {
			if(buffer[i] == '-') is_negative = true;
			i++;
		}



		// verifica daca primul caracter nu e de fapt un chiar un char
		// in loc de cifra
		// daca e returneaza numarul corespunzator
		char first = buffer[i];
		if((int) first < 48 or (int) first > 57) {
			cutbuffer(i + 1);
			number = (double) first;
			if(is_negative) number *= -1;
			return number;
		}

		bool real = false;
		double p{1};

		while(i < (int) buffer.size()) {
			char x = buffer[i];

			if(48 <= (int) x and (int) x <= 57) {
				number = number * 10 + (x - '0');

				if(real) {
					p *= 10;
				}
			}
			else if(x == '.' and !real) {
				// doar unul singur pe numar
				real = true;
			}
			else {
				break;
			}

			i++;
		}
		cutbuffer(i);
		number /= p;

		if(is_negative) number *= -1;
		return number;
	}


	std::string getNextBoolFromBuffer() {
		int i{};
		std::string Bool{};

		while(buffer[i] == ' ' or buffer[i] == '\t') {
			i++;
		}


		if(buffer[i] == '0' or buffer[i] == '1') {
			Bool += buffer[i];
			cutbuffer(1);
			return Bool;
		}

		while(i < (int) buffer.size()) {
			char x = buffer[i];


			if((65 <= (int) x and (int) x <= 90) or (97 <= (int) x and (int) x <= 122)) {
				Bool += x;
			}
			else if((int) x == ' ') {
				break;
			}

			i++;
		}
		cutbuffer(i);
		return Bool;
	}

	bool allSpaces(std::string n) {
		bool isAll = true;
		for(char x : n) {
			if(x != ' ' and x != '\t') {
				isAll = false;
				break;
			}
		}
		return isAll;
	}

	void doubleAllBackSlashes(std::string &n) {
		std::string new_n{};

		for(char x : n) {
			new_n += x;
			if(x == '\\') new_n += '\\';
		}

		n = new_n;
	}

	void interpretEntry(inAst *Tree) {
		for(int i{}; i < (int) Tree->list_of_identifiers.size(); i++) {
			std::string name = Tree->list_of_identifiers[i];
			var variable;
			
			std::string xstr{};
			double xdou{};

			if(Tree->type == token_ASSIGN_FLOAT) {
				// verifica buffer-ul daca contine ceva
				// daca contine, citeste din buffer
				if((int) buffer.size() == 0) {
					while(true) {
						std::getline(std::cin, buffer);
						if(!allSpaces(buffer)) break;
					}
				}
				xdou = getNextNumberFromBuffer();
				variable.type = numberType;
				variable.numberValue = xdou;
			}
			else if(Tree->type == token_ASSIGN_STRING) {
				if((int) buffer.size() != 0) {
					xstr = buffer;
					cutbuffer((int) buffer.size());
				}
				else {
					std::getline(std::cin, xstr);
				}
				variable.type = stringType;
				doubleAllBackSlashes(xstr);
				variable.stringValue = '"' + xstr + '"';
			}
			else if(Tree->type == token_ASSIGN_NATURAL) {
				if((int) buffer.size() == 0) {
					while(true) {
						std::getline(std::cin, buffer);
						if(!allSpaces(buffer)) break;
					}
				}

				xdou = getNextNumberFromBuffer();

				if(xdou < 0 or xdou != (int) xdou) throw badInput; 

				variable.type = numberType;
				variable.numberValue = xdou;
			}
			else if(Tree->type == token_ASSIGN_INT) {
				if((int) buffer.size() == 0) {
					while(true) {
						std::getline(std::cin, buffer);
						if(!allSpaces(buffer)) break;
					}
				}

				xdou = getNextNumberFromBuffer();

	
				if(xdou != (int) xdou) throw badInput;

				variable.type = numberType;
				variable.numberValue = xdou;

			}
			else if(Tree->type == token_ASSIGN_BOOL) {
				if((int) buffer.size() == 0) {
					while(true) {
						std::getline(std::cin, buffer);
						if(!allSpaces(buffer)) break;
					}
				}
				xstr = getNextBoolFromBuffer();


				if(xstr == "Adevarat" or xstr == "1") xdou = 1;
				else if(xstr == "Fals" or xstr == "0") xdou = 0;
				else throw badInput;

				variable.type = numberType;
				variable.numberValue = xdou;

			}

			variables[name] = variable;
		}


	}
}
