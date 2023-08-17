#include <string>

std::string sourceCode{};
std::string currentToken{};

enum tokens {
	// Keywords
	token_INPUT,
	token_OUTPUT,
	token_IF,
	token_THEN,
	token_ELSE_IF,
	token_ELSE,
	token_WHILE,
	token_EXECUTE,
	token_DO_WHILE,
	token_FOR,

	// Literals [FACUT]
	token_FLOAT,
	token_STRING,
	token_BOOL,

	// Data types when reading
	// spre exemplu citeste x (numar natural, nenul)
	// citeste x,y,z (numere reale)
	token_ASSIGN_NATURAL,
	token_ASSIGN_nonzeroNATURAL,
	token_ASSIGN_INT,
	token_ASSIGN_nonzeroINT, 
	token_ASSIGN_FLOAT,
	token_ASSIGN_nonzeroFLOAT,
	token_ASSIGN_STRING,
	token_ASSIGN_BOOL,

	// Operands
	token_ASSIGN,
	token_PLUS,
	token_MINUS,
	token_ASTERISK,
	token_DIVISION,
	token_MODULO,
	token_POWER,

	// Logic
	token_GREATER,
	token_GREATER_EQUAL,
	token_SMALLER,
	token_SMALLER_EQUAL,
	token_EQUAL,
	token_NOT,
	token_AND,
	token_OR,

	// Punctuation
	token_COMMA,
	token_LEFT_PARENTH,
	token_RIGHT_PARENTH,
	token_LEFT_SQUARE,
	token_RIGHT_SQUARE,

	// Others
	token_IDENTIFIER,
	token_UNKNOWN,

};

int p{};
char character{};



int getNextToken() {
	character = sourceCode[p];
	currentToken = "";

	/*
		Ia urmatorul token si pune-l in currentToken;
		Returneaza tipul de token; Un token este un keyword / cuvant + (un spatiu sau end on line)

		Exemplu:
		10 "FuckYou"Adevarat

		Dupa primul apel getNextToken
		69 "FuckYou"Adevarat
		  ^
		p = 2;
		currentToken = 10;
		returneaza token_FLOAT;

		Dupa al doilea apel
		69 "FuckYou"Adevarat
                    ^
		p = 12;
		currentToken = "FuckYou";
		returneaza token_STRING

		Dupa al treilea apel
		69 "FuckYou" Adevarat
		                    ^
		p = 20 (Nu, nu da segmentation fault, se adauga '$' la sfarsit pentru fiecare linie)
		currentToken = Adevarat
		returneaza token_BOOL
	*/


	// Spatiu
	// Modifica mai tarziu pentru a identifica identarea (pentru tab-uri si spatii)
	if(character == ' ') {
		p++;
		return getNextToken();
	}

	// Literals	
	if(isdigit(character)) {
		do {
			currentToken += character;
			character = sourceCode[++p];
		}
		while(isdigit(character) and p < sourceCode.length() - 1);
		if(character == '.') {
			do {
				currentToken += character;
				character = sourceCode[++p];
			}
			while(isdigit(character) and p < sourceCode.length() - 1);
		} 
		return token_FLOAT;
	}
	if(character == '\"') {
		do {
			currentToken += character;
			character = sourceCode[++p];
		}
		while((character != '\"' or sourceCode[p-1] == '\\') and p < sourceCode.length() - 1);
		currentToken += character;
		if(character == '\"' and sourceCode[p-1] != '\\') {
			p++;
			return token_STRING;
		}
		else if(p != sourceCode.length() - 1) {p++;}
		return token_UNKNOWN;
	}
	if(character == 'A' or character == 'F') {
		do {
			currentToken += character;
			character = sourceCode[++p];
		}
		while(isalpha(character) and p < sourceCode.length() - 1);
		if(currentToken == "Adevarat" or currentToken == "Fals") {
			return token_BOOL;
		}
		else {
			return token_IDENTIFIER;
		}
	}


	


	
	// Daca a ajuns pana aici, inseamna ca nu s-a potrivit cu nimic,
	// deci pune caracterul in currentToken, creste p-ul si returneaza UNKNOWN
	currentToken += character;
	p++;
	return token_UNKNOWN;
}
