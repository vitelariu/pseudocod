#include <iostream>




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
		std::cout << '\t' << line_number << " | " << line << '\n';
	}
	void syntax_error(std::string filename, std::string line, int line_number) {
		medium(filename);
		std::cout << "Sintaxa necunoscuta; Executare nereusita\n";
		printLine(line, line_number);
	}
	
}
