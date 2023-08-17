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

	// Literals [DONE]
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

	// Operands [DONE]
	token_ASSIGN,
	token_PLUS,
	token_MINUS,
	token_ASTERISK,
	token_DIVISION,
	token_MODULO,
	token_POWER,

	// Logic [DONE]
	token_GREATER,
	token_GREATER_EQUAL,
	token_SMALLER,
	token_SMALLER_EQUAL,
	token_EQUAL,
	token_NOT,
	token_AND,
	token_OR,

	// Punctuation [DONE]
	token_COMMA,
	token_LEFT_PARENTH,
	token_RIGHT_PARENTH,
	token_LEFT_SQUARE,
	token_RIGHT_SQUARE,

	// Others
	token_IDENTIFIER,
	token_UNKNOWN,
	token_FORCE_QUIT
};

int p{};
char character{};

bool checkIfPropperWord(const std::string& code, int start, const std::string& word) {
	int i;
	bool poate_fi = true;

	for(i = 0; (i < (int) word.length()) && poate_fi && (start + i < (int) code.length());++i)
		if(word[i] != code[start + i]) {
			poate_fi=false;
		}

	return poate_fi && (code[start + i] == ' ' or code[start + i] == '\t' or(code[start + i] == '$' and start + i == code.length() - 1));
}

int getNextToken() {
	character = sourceCode[p];
	currentToken = "";


	/*
		Ia urmatorul token si pune-l in currentToken;
		Returneaza tipul de token; Un token este un keyword / cuvant + (un spatiu sau end on line)

		Exemplu:
		10 "Hello_World!"Adevarat

		Dupa primul apel getNextToken
		10 "Hello_World!"Adevarat
		  ^
		p = 2;
		currentToken = "10";
		returneaza token_FLOAT;

		Dupa al doilea apel
		10 "Hello_World!"Adevarat
						 ^
		p = 17;
		currentToken = "\"Hello_World!"\"
		returneaza token_STRING

		Dupa al treilea apel
		10 "Hello_World!"Adevarat
								 ^
		p = 25 (Nu, nu da segmentation fault, se adauga '$' la sfarsit pentru fiecare linie)
		currentToken = "Adevarat"
		returneaza token_BOOL
	*/


	// Spatiu
	// Modifica mai tarziu pentru a identifica identarea (pentru tab-uri si spatii)
	if(character == ' ') {
		p++;
		if(p < sourceCode.length() - 1) {return getNextToken();}
		else {
			return token_FORCE_QUIT;
		}
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

	// Input, Output
	if(checkIfPropperWord(sourceCode, p, "citeste")) {
		currentToken = "citeste";
		p += currentToken.length();
		return token_INPUT;
	}
	if(checkIfPropperWord(sourceCode, p, "scrie")) {
		currentToken = "scrie";
		p += currentToken.length();
		return token_OUTPUT;
	}

	// Operands
	if(character == '<' and p < sourceCode.length() - 1) {
		if(sourceCode[p+1] == '-') {
			currentToken = "<-";
			p += 2;
			return token_ASSIGN;
		} 
	} 
	switch(character) {
		case '+':
			currentToken = character;
			p++;
			return token_PLUS;
		case '-':
			currentToken = character;
			p++;
			return token_MINUS;
		case '*':
			currentToken = character;
			p++;
			return token_ASTERISK;
		case '/':
			currentToken = character;
			p++;
			return token_DIVISION;
		case '%':
			currentToken = character;
			p++;
			return token_MODULO;
		case '^':
			currentToken = character;
			p++;
			return token_POWER;
	}

	// Logic
	if(character == '>') {
		if(sourceCode[p+1] == '=' and p < sourceCode.length() - 1) {
			currentToken = ">=";
			p += 2;
			return token_GREATER_EQUAL;
		}
		else {
			currentToken = character;
			p++;
			return token_GREATER;
		}
		
	}
	if(character == '<') {
		if(sourceCode[p+1] == '=' and p < sourceCode.length() - 1) {
			currentToken = "<=";
			p += 2;
			return token_SMALLER_EQUAL;
		}
		else {
			currentToken = character;
			p++;
			return token_SMALLER;
		}
	}
	if(character == '=') {
		currentToken = character;
		p++;
		return token_EQUAL;
	}
	if(character == '!') {
		currentToken = character;
		p++;
		return token_NOT;
	}
	
	if(checkIfPropperWord(sourceCode, p, "si")) {
		currentToken = "si";
		p += currentToken.length();
		return token_AND;
	}
	if(checkIfPropperWord(sourceCode, p, "sau")) {
		currentToken = "sau";
		p += currentToken.length();
		return token_OR;
	}

	// Punctuation
	switch(character) {
		case ',':
			currentToken = character;
			p++;
			return token_COMMA;
		case '(':
			currentToken = character;
			p++;
			return token_LEFT_PARENTH;
		case ')':
			currentToken = character;
			p++;
			return token_RIGHT_PARENTH;
		case '[':
			currentToken = character;
			p++;
			return token_LEFT_SQUARE;
		case ']':
			currentToken = character;
			p++;
			return token_RIGHT_SQUARE;
	}



	// Daca a ajuns pana aici, inseamna ca nu s-a potrivit cu nimic,
	// deci pune caracterul in currentToken, creste p-ul si returneaza UNKNOWN
	currentToken += character;
	p++;
	return token_UNKNOWN;
}
