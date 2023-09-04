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

void clean(exprAst *Tree, double result) {
	Tree->op = numberType;
	checkFlags(Tree, result);
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

std::string interpret(exprAst *Tree) {
	if(Tree->op == numberType) {
		checkFlags(Tree, Tree->number);
		if(Tree->number == (int) Tree->number) return std::to_string((int) Tree->number);
		return std::to_string(Tree->number);
	}
	else if(Tree->op == stringType) {
		return Tree->str;
	} 

	double result{};
	bool string = false;

	switch(Tree->op) {
		case orType:

			result = std::stoi(interpret(Tree->left)) || std::stoi(interpret(Tree->right));
			clean(Tree, result);
			break;
		case andType:

			result = std::stoi(interpret(Tree->left)) && std::stoi(interpret(Tree->right));
			clean(Tree, result);
			break;
		case greaterType:

			result = std::stod(interpret(Tree->left)) > std::stod(interpret(Tree->right));
			clean(Tree, result);
			break;
		case greaterEqualType:
			result = std::stod(interpret(Tree->left)) >= std::stod(interpret(Tree->right));
			clean(Tree, result);
			break;
		case smallerType:


			result = std::stod(interpret(Tree->left)) < std::stod(interpret(Tree->right));
			clean(Tree, result);
			break;
		case smallerEqualType:


			result = std::stod(interpret(Tree->left)) <= std::stod(interpret(Tree->right));
			clean(Tree, result);
			break;
		case equalType:


			result = std::stod(interpret(Tree->left)) == std::stod(interpret(Tree->right));
			clean(Tree, result);
			break;
		case notEqualType:


			result = std::stod(interpret(Tree->left)) != std::stod(interpret(Tree->right));
			clean(Tree, result);
			break;
		case plusType:


			result = std::stod(interpret(Tree->left)) + std::stod(interpret(Tree->right));
			clean(Tree, result);
			break;
		case minusType:


			result = std::stod(interpret(Tree->left)) - std::stod(interpret(Tree->right));
			clean(Tree, result);
			break;
		case multiplyType:
			if(checkIfChildIsString(Tree->left) and !checkIfChildIsString(Tree->right)) {
				string = true;
				Tree->op =	stringType;
				Tree->str = multiply_str(Tree->left->str, std::stod(interpret(Tree->right)));
				delete Tree->left;
				delete Tree->right;
				Tree->left = nullptr;
				Tree->right = nullptr;

				break;
			}
			else if(checkIfChildIsString(Tree->right) and !checkIfChildIsString(Tree->left)) {
				string = true;
				Tree->op =	stringType;
				Tree->str = multiply_str(Tree->right->str, std::stod(interpret(Tree->left)));
				delete Tree->left;
				delete Tree->right;
				Tree->left = nullptr;
				Tree->right = nullptr;
				break;
			}
			result = std::stod(interpret(Tree->left)) * std::stod(interpret(Tree->right));
			clean(Tree, result);
			break;
		case divisionType:


			result = std::stod(interpret(Tree->left)) / std::stod(interpret(Tree->right));
			clean(Tree, result);
			break;
		case powerType:
			result = pow(std::stod(interpret(Tree->left)),std::stod(interpret(Tree->right)));
			clean(Tree, result);
			break;


	}

	if(string) return Tree->str;
	if(Tree->number == (int) Tree->number) return std::to_string((int) Tree->number);
	return std::to_string(Tree->number);
}
