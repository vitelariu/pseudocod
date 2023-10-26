#include <iostream>


enum errorsTypes {
	syntaxError,
	divisionByZero,
	invalidFile,
	tooManyArgs,
};


namespace errors {
	void medium(std::string filename) {
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
		std::cout << "Sintaxa necunoscuta; Executare nereusita\n";
		printLine(line, line_number);
	}
	void division_by_zero(std::string filename, std::string line, int line_number) {
		medium(filename);
		std::cout << "Impartire la 0 interzisa\n";
		printLine(line, line_number);
	}
	void invalid_file(std::string filename) {
		std::cout << "Eroare: fisier invalid -> " << filename << '\n';
	}
	void too_many_args() {
		std::cout << "Eroare: se asteapta zero sau un singur argument\n";
	}
	
}
