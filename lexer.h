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
	token_UNTIL,
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

/// Functie auxiliara, verifica daca urmatorul caracter poate fi considerat drept facand parte dintr-un alt cuvant
bool checkNextCharacterDifferentToken(const char nextCharacter) {
	// cuvantul nu se termina acolo
	if(nextCharacter >= 'a' && nextCharacter <= 'z')
		return false;
	if(nextCharacter >= 'A' && nextCharacter <= 'Z')
		return false;
	if(nextCharacter >= '0' && nextCharacter <= '9')
		return false;

	// cuvantul e urmat de whitespace, poate mai trebuie adaugate
	if(nextCharacter == ' ' || nextCharacter == '\n' || nextCharacter == '\t')
		return true;

	// cuvantul e urmat de marcator sfarsit de linie (se poate scoate daca nu mai mergem pe ideea asta)
	if(nextCharacter == '$')
		return true;

	// cuvantul e urmat de un caracter de tip operator
	std::string operatori = "+-*/%^<=>!";
	if(operatori.find(nextCharacter) != std::string::npos)
		return true;

	// cuvantul e urmat de un caracter de tip paranteza
	std::string paranteze = "()[]"; //probabil ca {} trebuie adaugate dar nu sunt sigur
	if(paranteze.find(nextCharacter) != std::string::npos)
		return true;

	// cuvantul e urmat de un sir de caractere (caracter ")
	if(nextCharacter == '\"')
		return true;

	// nu am gasit un caracter ce poate marca inceputul unui alt token, presupunem ca este un caracter de
	// genul ~`#:;',.? (nu stiu ce se considera in cazul asta, o sa presupun ca pot face parte din cuvinte)
	return false;
}

/// Functie auxiliara, verifica daca code[start .. start + word.length()] == word si daca cuvantul se incheie acolo
bool checkIfToken(const std::string& code, int start, const std::string& word) {
	int i;
	bool poate_fi = true;

	// verifica daca toate caracterele sunt egale
	for(i = 0; (i < (int) word.length()) && poate_fi && (start + i < (int) code.length()); ++i)
		if(word[i] != code[start + i])
			poate_fi=false;

	// daca toate caracterele sunt egale, verificam daca cuvantul s-a terminat
	return poate_fi && checkNextCharacterDifferentToken(code[start + i]);
}

int getNextToken() {
	character = sourceCode[p];
	currentToken = "";

	/*
		Ia urmatorul token si pune-l in currentToken;
		Returneaza tipul de token;

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
		return getNextToken();
	}

	// Literals
	if(isdigit(character)) {
		do {
			currentToken += character;
			character = sourceCode[++p];
		}
		while(isdigit(character) and p < (int) sourceCode.length() - 1);
		if(character == '.') {
			do {
				currentToken += character;
				character = sourceCode[++p];
			}
			while(isdigit(character) and p < (int) sourceCode.length() - 1);
		}
		return token_FLOAT;
	}
	if(character == '\"') {
		do {
			currentToken += character;
			character = sourceCode[++p];
		}
		while((character != '\"' or sourceCode[p-1] == '\\') and p < (int) sourceCode.length() - 1);
		currentToken += character;
		if(character == '\"' and sourceCode[p-1] != '\\') {
			p++;
			return token_STRING;
		}
		else if(p != (int) sourceCode.length() - 1) {p++;}
		return token_UNKNOWN;
	}
	if(character == 'A' or character == 'F') {
		do {
			currentToken += character;
			character = sourceCode[++p];
		}
		while(isalpha(character) and p < (int) sourceCode.length() - 1);
		if(currentToken == "Adevarat" or currentToken == "Fals") {
			return token_BOOL;
		}
		else {
			return token_IDENTIFIER;
		}
	}

	// Keywords (Input, Output, If/Else, Structuri repetitive)
	{
		const unsigned int cnt_keywords = 10;
		unsigned int i;
		// Probabil va trebui sa modificam check-ul pentru "altfel daca", "cat timp" si "pana cand"
		// pentru a permite scrierea mai multor caractere cum ar fi: "cat     timp     Adevarat"
		// sau inlocuirea cu un caracter '\t' (dar nu sunt sigur daca asta ar trebui sa fie cod valid)
		std::string keywords[cnt_keywords] = {"citeste", "scrie", "daca", "atunci", "altfel", "altfel daca", "cat timp", "executa", "pentru", "pana cand"};
		tokens keyword_tok[cnt_keywords] = {token_INPUT, token_OUTPUT, token_IF, token_THEN, token_ELSE, token_ELSE_IF, token_WHILE, token_EXECUTE, token_FOR, token_UNTIL};

		for(i = 0; i < cnt_keywords; ++i)
		{
			if(checkIfToken(sourceCode, p, keywords[i]))
			{
				// daca asta e token-ul, returneaza-l
				currentToken = keywords[i];
				p += keywords[i].length();
				return keyword_tok[i];
			}
		}
	}


	// Daca a ajuns pana aici, inseamna ca nu s-a potrivit cu nimic,
	// deci pune caracterul in currentToken, creste p-ul si returneaza UNKNOWN
	currentToken += character;
	p++;
	return token_UNKNOWN;
}
