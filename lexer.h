#include <string>

std::string sourceCode{};
std::string sourceCode2{}; // sourceCode but without the last '$'
std::string currentToken{};

enum tokens {
	// Keywords [DONE]
	token_INPUT,
	token_OUTPUT,
	token_IF,
	token_THEN,
	token_ELSE_IF,
	token_ELSE,
	token_WHILE,
	token_EXECUTE,
	token_UNTIL,
	token_FOR,

	// Literals [DONE]
	token_FLOAT,
	token_STRING,

	// Data types when reading [DONE]
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
	token_NOT_EQUAL,
	token_AND,
	token_OR,
	token_NOT,

	// Punctuation [DONE]
	token_COMMA,
	token_LEFT_PARENTH,
	token_RIGHT_PARENTH,
	token_LEFT_SQUARE,
	token_RIGHT_SQUARE,

	// Others
	token_IDENTIFIER,
	token_IDENTATION,
	token_UNKNOWN,
	token_FORCE_QUIT
};

int p{};
char character{};

/// Functie auxiliara, verifica daca urmatorul caracter poate fi considerat drept facand parte dintr-un alt token
bool checkNextCharacterDifferentToken(const std::string &code, int i) {
	const char nextCharacter = code[i];
	// cuvantul nu se termina acolo
	if(nextCharacter >= 'a' && nextCharacter <= 'z')
		return false;
	if(nextCharacter >= 'A' && nextCharacter <= 'Z')
		return false;
	if(nextCharacter >= '0' && nextCharacter <= '9')
		return false;

	// cuvantul e urmat de whitespace, poate mai trebuie adaugate alte tipuri de caractere whitespace
	if(nextCharacter == ' ' || nextCharacter == '\n' || nextCharacter == '\t')
		return true;

	// cuvantul e urmat de marcator sfarsit de linie (se poate scoate daca nu mai mergem pe ideea asta)
	if(nextCharacter == '$' and i == (int) code.length() - 1)
		return true;

	// cuvantul e urmat de un caracter de tip operator
	std::string operatori = "+-*/%^<=>!";
	if(operatori.find(nextCharacter) != std::string::npos)
		return true;

	// cuvantul e urmat de un caracter de tip paranteza
	std::string paranteze = "()[]";
	if(paranteze.find(nextCharacter) != std::string::npos)
		return true;

	// cuvantul e urmat de un sir de caractere (caracter ")
	if(nextCharacter == '\"')
		return true;

	// nu am gasit un caracter ce poate marca inceputul unui alt token, presupunem ca este un caracter de
	// genul ~`#:;',.? (nu stiu ce se considera in cazul asta, o sa presupun ca pot face parte din cuvinte)
	return false;
}

/// Functie auxiliara, verifica daca code[start .. start + word.length()] == word si daca tockenul se incheie acolo
bool checkIfToken(const std::string &code, int start, const std::string& word) {
	int i;
	bool poate_fi = true;

	// verifica daca toate caracterele sunt egale
	for(i = 0; (i < (int) word.length()) && poate_fi && (start + i < (int) code.length()); ++i)
		if(word[i] != code[start + i])
			poate_fi=false;

	// daca toate caracterele sunt egale, verificam daca tockenul s-a terminat
	return poate_fi && checkNextCharacterDifferentToken(code, start + i);
}

/**
Ia urmatorul token si pune-l in currentToken. Modifica variabilele globale 'p', 'character' si 'currentToken'.
Probabil ar fi o idee buna sa nu mai folosim variabile globale pentru ca pot aparea erori si in cazul ala
pierzi starea precedenta a variabilelor.
@return Returneaza tipul de token; Un token este un keyword / cuvant + (un spatiu sau end on line)
*/
int getNextToken() {
	if(p < 0 or p >= (int)sourceCode.length())
	{
		currentToken = "";
		throw "getNextToken called with invalid p = " + std::to_string(p) + " when source code length = " + std::to_string(sourceCode.length());
	}

	character = sourceCode[p];
	currentToken = "";

	// Spatiu
	if(character == ' ' or character == '\t') {
		if(p == 0) {
			// Numara identarea (este la inceputul liniei)
			// 1 / 2 / 3 / 4 spatii inseamna o identare
			// 1 tab inseamna o identare
			// Exemplu: un spatiu + un tab + un spatiu = 2 identarii
			// (in total ai un tab si 2 spatii)
			//
			// Token-ul returnat va fi token_IDENTATION indiferent de numarul de identari
			// Numarul de identari este pus in currentToken

			int spaces_cnt{}, tabs_cnt{}, identation_cnt{};
			do {
				if(character == ' ') spaces_cnt++;
				else if(character == '\t') tabs_cnt++;

				character = sourceCode[++p];
			}
			while((character == ' ' or character == '\t'));

			identation_cnt += tabs_cnt; // tab-uri sunt echivalente cu identarea
			identation_cnt += spaces_cnt / 4 + (spaces_cnt % 4 and 1); // formula pentru a
																	   // transforma spatii in
																	   // identare
			currentToken = std::to_string(identation_cnt);
			return token_IDENTATION;

		}
		else { // Ce a vrut sa spuna autorul aici?
			p++;
			if(p < (int) sourceCode.length() - 1) {return getNextToken();}
			else {
				return token_FORCE_QUIT;
			}
		}
	}

	// Literals (numere, siruri de caractere, Adevarat/Fals)
	if(isdigit(character)) {
		do {
			currentToken += character;
			character = sourceCode[++p];
		}
		while(isdigit(character));
		if(character == '.') {
			do {
				currentToken += character;
				character = sourceCode[++p];
			}
			while(isdigit(character));
		}
		return token_FLOAT;
	}
	else if(character == '\"' and p < (int) sourceCode.length() - 2) { // Aici ar trebui adaugat de fapt un check pentru siruri de
		// caractere care sa returneze eroare daca e inceput dar nu e terminat
		int start = p;
		currentToken += character;
		while(p < (int) sourceCode.length() - 1) { // Aici s-ar putea face mult mai simplu ca sa nu verifici la
			// sfarsit din nou
			character = sourceCode[++p];
			if(character == '\"') {
				// check previous escape chars
				int end = p - 1;
				int escape{};
				for(int i = end; i > start; i--) {
					if(sourceCode[i] == '\\') escape++;
					else break;
				}
				if(escape % 2 == 0) {
					currentToken += character;
					p++;
					return token_STRING;
				}
				else if(escape % 2 == 1) {
					currentToken += character;
				}
			}
			else {
				currentToken += character;
			}
		}
		if(character == '\"') {
			// check previous escape chars
			int end = p - 1;
			int escape{};
			for(int i = end; i > start; i--) {
				if(sourceCode[i] == '\\') escape++;
				else break;
			}
			if(escape % 2 == 0) {
				currentToken += character;
				p++;
				return token_STRING;
			}
			else {
				return token_UNKNOWN;
			}
		}
		else {
			return token_UNKNOWN;
		}
	}
	else if(checkIfToken(sourceCode, p, "Adevarat")) {
		currentToken = "1";
		p += 8;
		return token_FLOAT;
	}
	else if(checkIfToken(sourceCode, p, "Fals")) {
		currentToken = "0";
		p += 4;
		return token_FLOAT;

	}


	// Keywords (Input, Output, Structuri de decizie, Structuri repetitive)
	{
		const unsigned int cnt_keywords = 10;
		unsigned int i;

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

	// Operands
	if(character == '<') { // Asta pare foarte gresit;
		// Codul "daca x<-y atunci" pare ca produce un token de atribuire dar dupa cum am discutat nu asa va functiona
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
		if(sourceCode[p+1] == '=') {
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
		if(sourceCode[p+1] == '=') {
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
	if(checkIfToken(sourceCode, p, "!=")) {
		currentToken = "!=";
		p += currentToken.length();
		return token_NOT_EQUAL;
	}
	if(character == '!') {
		currentToken = character;
		p++;
		return token_NOT;
	}

	if(checkIfToken(sourceCode, p, "si")) {
		currentToken = "si";
		p += currentToken.length();
		return token_AND;
	}
	if(checkIfToken(sourceCode, p, "sau")) {
		currentToken = "sau";
		p += currentToken.length();
		return token_OR;
	}

	// Data types
	{
		const unsigned int cnt_keywords = 16;
		unsigned int i;

		std::string keywords[cnt_keywords] = {"(numar natural)", "(numere naturale)", "(numar natural, nenul)", "(numere naturale, nenule)", "(numar intreg)", "(numere intregi)", "(numar intreg, nenul)", "(numere intregi, nenule)", "(numar real)", "(numere reale)", "(numar real, nenul)", "(numere reale, nenule)", "(string)", "(text)", "(bool)", "(logica)"};
		tokens keyword_tok[cnt_keywords / 2] = {token_ASSIGN_NATURAL, token_ASSIGN_nonzeroNATURAL, token_ASSIGN_INT, token_ASSIGN_nonzeroINT, token_ASSIGN_FLOAT, token_ASSIGN_nonzeroFLOAT, token_ASSIGN_STRING, token_ASSIGN_BOOL};

		for(i = 0; i < cnt_keywords; i++)
		{
			if(checkIfToken(sourceCode, p, keywords[i]))
			{
				// daca asta e token-ul, returneaza-l
				currentToken = keywords[i];
				p += keywords[i].length();
				return keyword_tok[i / 2];
			}
			i++;
			if(checkIfToken(sourceCode, p, keywords[i]))
			{
				// daca asta e token-ul, returneaza-l
				currentToken = keywords[i];
				p += keywords[i].length();
				return keyword_tok[i / 2];
			}

		}

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

	// Identifier

	if(isalpha(character)) {
		do {
			currentToken += character;
			character = sourceCode[++p];
		}
		while(isalpha(character) or isdigit(character) or character == '_');
		return token_IDENTIFIER;
	}

	// Daca a ajuns pana aici, inseamna ca nu s-a potrivit cu nimic,
	// deci pune caracterul in currentToken, creste p-ul si returneaza UNKNOWN
	currentToken += character;
	p++;
	return token_UNKNOWN;
}
