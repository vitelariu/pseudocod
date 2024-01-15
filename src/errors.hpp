#include <iostream>


enum errorsTypes {
	syntaxError,
	divisionByZero,
	invalidFile,
	tooManyArgs,
	uninitialisedVar,
	badInput,
	forgotThen,
	forgotExecute,
	onlyOneElseError,
	elseWithoutIf,
};


namespace errors {
	void medium(std::string filename) {
		std::cout << '\n';
		if(filename == "") {
			std::cout << "Terminal: eroare -> ";
		}
		else {
			std::cout << "In fisierul: " << filename << ": eroare -> ";
		}
	}
	void printLine(std::string line, int line_number) {
		std::cout << "   " << line_number << " | " << line << '\n';
	}
	void syntax_error(std::string filename, std::string line, int line_number) {
		medium(filename);
		std::cout << "Sintaxa necunoscuta; Executare nereusita!\n";
		printLine(line, line_number);
	}
	void division_by_zero(std::string filename, std::string line, int line_number) {
		medium(filename);
		std::cout << "Impartire la 0 interzisa!\n";
		printLine(line, line_number);
	}
	void invalid_file(std::string filename) {
		std::cout << "Eroare: fisier invalid -> " << filename << '\n';
	}
	void too_many_args() {
		std::cout << "Eroare: se asteapta zero sau un singur argument!\n";
	}
	void uninitialised_var(std::string filename, std::string line, int line_number) {
		medium(filename);
		std::cout << "Variabila neinitializata!\n";
		printLine(line, line_number);
	}
	void bad_input(std::string filename, std::string line, int line_number) {
		medium(filename);
		std::cout << "S-a citit gresit valoarea!\n";
		printLine(line, line_number);
	}
	void forgot_then(std::string filename, std::string line, int line_number) {
		medium(filename);
		std::cout << "Token-ul \"atunci\" lipseste!\n";
		printLine(line, line_number);
	}
	void forgot_execute(std::string filename, std::string line, int line_number) {
		medium(filename);
		std::cout << "Token-ul \"executa\" lipseste!\n";
		printLine(line, line_number);
	}
	void only_one_else_error(std::string filename, std::string line, int line_number) {
		medium(filename);
		std::cout << "Eroare: nu poti avea decat un singur \"altfel\" per \"daca\"\n";
		printLine(line, line_number);
	}
	void else_without_if(std::string filename, std::string line, int line_number) {
		medium(filename);
		std::cout << "Eroare: niciun \"daca\" detectat inainte de \"altfel\"\n";
		printLine(line, line_number);

	}

}
