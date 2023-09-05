#include <iostream>
#include <cmath>
#include "parser.h"


void checkFlags(exprAst *Tree, double result) {
	if(Tree->flags[0]) {
		result = (int) result;
	}
	if(Tree->flags[1]) {
		result *= -1;
	}
	if(Tree->flags[2]) {
		result = !result;
	}
	Tree->number = result;
}

void clean(exprAst *Tree){
	delete Tree->left;
	delete Tree->right;
	Tree->left = nullptr;
	Tree->right = nullptr;
}

bool checkIfChildIsString(exprAst *node) {
	return node->op == stringType;
}

std::string multiply_str(std::string x, double number) {
	if(number != (int) number) {
		std::cout << "eroare cum sa inmultesti cu double";
		return "";
	}
	
	std::string final_str = "\"";
	std::string pure_str{};

	for(int i{1}; i < (int) x.length() - 1; i++) {
		pure_str += x[i];
	}

	for(int i{}; i < (int) number; i++) {
		final_str += pure_str;
	}
	final_str += "\"";

	return final_str;
}

std::string getRaw(std::string x) {
	std::string raw{};
	for(int i{1}; i < (int) x.length() - 1; i++) {
		raw += x[i];
	}
	return raw;
}

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
			interpret(Tree->right);
			result = Tree->left->number || Tree->right->number;

			Tree->op = numberType;
			checkFlags(Tree, result);
			clean(Tree);
			break;
		case andType:
			interpret(Tree->left);
			interpret(Tree->right);
			result = Tree->left->number && Tree->right->number;

			Tree->op = numberType;
			checkFlags(Tree, result);
			clean(Tree);
			break;

		case greaterType:
			interpret(Tree->left);
			interpret(Tree->right);
			result = Tree->left->number > Tree->right->number;

			Tree->op = numberType;
			checkFlags(Tree, result);
			clean(Tree);
			break;

		case greaterEqualType:
			interpret(Tree->left);
			interpret(Tree->right);
			result = Tree->left->number >= Tree->right->number;

			Tree->op = numberType;
			checkFlags(Tree, result);
			clean(Tree);
			break;

		case smallerType:
			interpret(Tree->left);
			interpret(Tree->right);
			result = Tree->left->number < Tree->right->number;

			Tree->op = numberType;
			checkFlags(Tree, result);
			clean(Tree);
			break;

		case smallerEqualType:
			interpret(Tree->left);
			interpret(Tree->right);
			result = Tree->left->number <= Tree->right->number;

			Tree->op = numberType;
			checkFlags(Tree, result);
			clean(Tree);
			break;

		case equalType:
			interpret(Tree->left);
			interpret(Tree->right);
			result = Tree->left->number == Tree->right->number;

			Tree->op = numberType;
			checkFlags(Tree, result);
			clean(Tree);
			break;

		case notEqualType:
			interpret(Tree->left);
			interpret(Tree->right);
			result = Tree->left->number != Tree->right->number;

			Tree->op = numberType;
			checkFlags(Tree, result);
			clean(Tree);
			break;

		case plusType:
			interpret(Tree->left);
			interpret(Tree->right);
			
			if(Tree->left->op == stringType and Tree->left->op == stringType) {
				Tree->op = stringType;
				Tree->str = "\"" + getRaw(Tree->left->str) + getRaw(Tree->right->str) + "\"";

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
			if(Tree->left->number != (int) Tree->left->number or Tree->right->number != (int) Tree->right->number) {
				std::cout << "eroare nu poti face modulo pe numere reale\n";
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
