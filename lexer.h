#include <string>

std::string sourceCode{};
std::string currentToken{};

enum Tokens {
	token_citeste,
	token_numere_naturale,
	token_numere_intregi,
	token_numere_reale,
	token_numere_nenule,
	token_scrie,

	token_daca,
	token_altfel_daca,
	token_altfel,

	token_cat_timp,
	token_executa,
	token_pana_cand,

	token_pentru,

	token_identifier,
	token_assign,
	token_constant,
	token_operand,
	token_comma,
	token_open_bracket,
	token_closed_bracket,
}

int p{};
int getNextToken() {
	char character = sourceCode[p];

	/* Get next token and put it in currentToken
	 * variable. Return the token number.
	 * 
	 * /
}
