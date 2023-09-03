#include <iostream>
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

double interpret(exprAst *Tree) {
	if(Tree->op == numberType) {
		checkFlags(Tree, Tree->number);
		return Tree->number;
	}

	double result{};

	switch(Tree->op) {
		case orType:
			result = interpret(Tree->left) || interpret(Tree->right);
			clean(Tree, result);
			break;
		case andType:
			result = interpret(Tree->left) && interpret(Tree->right);
			clean(Tree, result);
			break;
		case greaterType:
			result = interpret(Tree->left) > interpret(Tree->right);
			clean(Tree, result);
			break;
		case greaterEqualType:
			result = interpret(Tree->left) >= interpret(Tree->right);
			clean(Tree, result);
			break;
		case smallerType:
			result = interpret(Tree->left) < interpret(Tree->right);
			clean(Tree, result);
			break;
		case smallerEqualType:
			result = interpret(Tree->left) <= interpret(Tree->right);
			clean(Tree, result);
			break;
		case equalType:
			result = interpret(Tree->left) == interpret(Tree->right);
			clean(Tree, result);
			break;
		case notEqualType:
			result = interpret(Tree->left) != interpret(Tree->right);
			clean(Tree, result);
			break;
		case plusType:
			result = interpret(Tree->left) + interpret(Tree->right);
			clean(Tree, result);
			break;
		case minusType:
			result = interpret(Tree->left) - interpret(Tree->right);
			clean(Tree, result);
			break;
		case multiplyType:
			result = interpret(Tree->left) * interpret(Tree->right);
			clean(Tree, result);
			break;
		case divisionType:
			result = interpret(Tree->left) / interpret(Tree->right);
			clean(Tree, result);
			break;
		case powerType:
			result = pow(interpret(Tree->left),interpret(Tree->right));
			clean(Tree, result);
			break;


	}


	return Tree->number;
}
