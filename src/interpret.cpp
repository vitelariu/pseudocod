#include <iostream>
#include <fstream>
#include <utility>
#include "interpreter.hpp"


#if 0
void start(std::vector<std::pair<std::string, int>>& tokens, bool inTerminal) {
	switch(tokens[0].second) {
		case token_OUTPUT:
		{
			outAst *Tree = parseOut::parseEntry(tokens);
			interpretOut::interpretEntry(Tree);
			if(inTerminal) std::cout << '\n';
			break;
		}
		case token_IDENTIFIER:
		{
			bool assign = false;
			if(tokens.size() > 1 and tokens[1].second == token_ASSIGN) assign = true;

			if(assign) {
				varNode *node = parseVar::parseEntry(tokens);
				interpretVar::interpretEntry(node);
			}
			else {
				std::cou
				exprAst *Tree = parseExpr::parseEntry(tokens);
				interpretExpr::interpretEntry(Tree);
	
				if(inTerminal) {
					if(Tree->op == numberType) std::cout << Tree->number;
					else std::cout << Tree->str;
					std::cout << '\n';
				}

				delete Tree;
				Tree = nullptr;
			}

			break;
		}
		case token_INPUT:
		{
			inAst *Tree = parseIn::parseEntry(tokens);
			interpretIn::interpretEntry(Tree);

			break;
		}
		default:
		{
			exprAst *Tree = parseExpr::parseEntry(tokens);
			interpretExpr::interpretEntry(Tree);

			if(inTerminal) {
				if(Tree->op == numberType) std::cout << Tree->number;
				else std::cout << Tree->str;
				std::cout << '\n';
			}

			delete Tree;
			Tree = nullptr;

		}
	}
}
#endif


// Aceasta functie este responsabila de a asambla arborele principal
//
// chestia asta se ocupa de nesting in tree
// practic se tine minte fiecare statement in care poti avea statements
// spre ex un while
// cand scrii un while, tot ce e identat mai departe trebuie scris ceva de genul intr-un tree:
// statements -> statement
//            -> statement
//            -> statement -> statements -> statement
//                                       -> statement
//
// Tree-ul de mai sus e pentru programul asta:
//
// citeste n
// i <- 0
// cat timp i < a executa
//     citeste x
//     scrie x, " "
class ProgramBlocks {
	public:
		std::vector<statements*> Blocks{};


		// spre exemplu daca avem un while
		// daca te uiti in parse la clasa whileAst vei vedea un pointer
		// spre statements
		// (statements este o clasa cu un vector care contine statement)
		// in acel statements vom baga urmatoarele linii de cod care sunt identate pentru
		// acel while
		// 
		// le bagam in vectorul Blocks aceste pointere si le accesam de aici cand avem nevoie
		// dupa urmatoarea regula:
		// fie size = Blocks.size() - 1
		// se verifica identarea de pe linia curenta
		// daca e mai mare decat size: eroare
		//
		// altfel
		// se ia numarul de identari = i
		// si vom adauga statement-ul curent la Blocks[i]
		// daca size > i se da erase de la i + 1 pana la final

		void create_new_block(statements *block) {
			Blocks.push_back(block);
		}


		void add_line(statement *Statement, int identation_number) {
			if(identation_number > Blocks.size() - 1) throw syntaxError;
			
			Blocks[identation_number]->add(Statement); // am adaugat in Blocks


			// verificam daca fiecare statements din Blocks de la identation_number + 1 in sus
			// are cel putin un statement inainte de a le sterge
			for(int i = identation_number + 1; i < Blocks.size(); i++) {
				if(Blocks[i]->s.size() == 0) throw syntaxError;
			}

			if(identation_number < Blocks.size() - 1) Blocks.erase(Blocks.begin() + identation_number + 1, Blocks.end());

		}


};

statements *MainProgram = new statements;
ProgramBlocks mainblocks{};



void parse(std::vector<std::pair<std::string, int>>& tokens) {
		
	statement *Statement = new statement;

	// Daca tokens e gol, statement-ul va fi gol
	if(tokens.size() == 0) {
		mainblocks.add_line(Statement, 0);
		return;
	}

	// Primul token poate fi o identare
	int identation_number{};
	if(tokens[0].second == token_IDENTATION) {
		// daca este, pune-l in identation_number si sterge primul token
		// (identation_number) e folosit pentru a identifica unde pre arbore se
		// incadreaza aceasta linie

		identation_number = std::stoi(tokens[0].first);
		tokens.erase(tokens.begin());
	}

	switch(tokens[0].second) {
		case token_OUTPUT:
		{
			Statement->outAst_p = parseOut::parseEntry(tokens);
			mainblocks.add_line(Statement, identation_number);
			break;
		}
		case token_INPUT:
		{
			Statement->inAst_p = parseIn::parseEntry(tokens);
			mainblocks.add_line(Statement, identation_number);
			break;
		}
		case token_IDENTIFIER:
		{
			if(tokens.size() >= 3 and tokens[1].second == token_ASSIGN) {
				// practic minimul necesar ca sa fie id <- expr
				Statement->varNode_p = parseVar::parseEntry(tokens);
				mainblocks.add_line(Statement, identation_number);
			}	
			else {
				// altfel e expresie care doar incepe cu un identificator
				Statement->exprAst_p = parseExpr::parseEntry(tokens);
				mainblocks.add_line(Statement, identation_number);
			}

			break;
		}
		case token_WHILE:
		{
			Statement->whileAst_p = parseWhile::parseEntry(tokens);
			mainblocks.add_line(Statement, identation_number);
			mainblocks.create_new_block(Statement->whileAst_p->block);
			break;
		}
		default:
		{
			Statement->exprAst_p = parseExpr::parseEntry(tokens);
			mainblocks.add_line(Statement, identation_number);

			break;
		}

	}



}


void execute(statements *Statements, bool inTerminal, int &line_number) {
	// Prima si prima data cand se apeleaza la aceasta functie din main, line_number
	// ar trebui sa fie 0. Pentru fiecare statement se incrementeaza cu 1, dar exista si 
	// cazul cand avem spre ex un while loop
	// atunci se apeleaza din nou la execute tot din aceasta functiem, dar line_number-ul ramane la fel 

	for(statement* Statement : Statements->s) {
		line_number++;

		if(Statement->outAst_p) {
			interpretOut::interpretEntry(Statement->outAst_p);


			if(inTerminal) std::cout << '\n';
		}
		else if(Statement->inAst_p) {
			interpretIn::interpretEntry(Statement->inAst_p);
		}
		else if(Statement->varNode_p) {


			interpretVar::interpretEntry(Statement->varNode_p);


		}
		else if(Statement->exprAst_p) {
			interpretExpr::interpretEntry(Statement->exprAst_p);


			if(inTerminal) {
				if(Statement->exprAst_p->op == numberType) std::cout << Statement->exprAst_p->number;
				else std::cout << Statement->exprAst_p->str;
				std::cout << '\n';
			}
		}
		else if(Statement->whileAst_p) {
			// aici nu am la ce functie interpretWhile sa apelez pentru ca nu
			// ar avea sens sa existe. Avem doar un alt statements (Statement->whileAst_p->block) cu statement 
			// (un vector cu statement) ca la inceput
			// deci pur si simplu chemam din nou functia intr-un while
			//
			// dar mai intai de a executa trebuie sa copiem tree-ul pe care il avem
			// pentru ca de fiecare data cand apelam execute si pasam un tree, il modifica
			//
			// nu putem folosi new pe tot arborele pentru ca nu va merge
			// deci va trebui sa creez o functie auxiliara pentru copiat arbori
			// functia e declarata in parser
			//
			exprAst *condition_copy = new exprAst(*Statement->whileAst_p->condition);
			CopyTree::copy_exprAst(Statement->whileAst_p->condition, condition_copy);
			

			while(interpretExpr::interpretEntryReturnInt(condition_copy)) {
				statements *new_block = new statements(*Statement->whileAst_p->block);
				CopyTree::Entry(Statement->whileAst_p->block, new_block);
				

#if 0

				exprAst *expression = new exprAst(*Statement->whileAst_p->block->s[0]->varNode_p->expr);
				CopyTree::copy_exprAst(Statement->whileAst_p->block->s[0]->varNode_p->expr, expression);
				varNode *asignare = new varNode(*Statement->whileAst_p->block->s[0]->varNode_p);
				asignare->expr = expression;
				statement *statement2 = new statement(*Statement->whileAst_p->block->s[0]);
				statement2->varNode_p = asignare;
				statements *list = new statements(*Statement->whileAst_p->block);
				list->s[0] = statement2;
#endif
				

				//std::cout << variables["i"].numberValue << "\n";	
				execute(new_block, false, line_number);



				condition_copy = new exprAst(*Statement->whileAst_p->condition);
				CopyTree::copy_exprAst(Statement->whileAst_p->condition, condition_copy);
			}



						
		}
	}	

}


int main(int argc, char **argv) {
	std::string filename{};
	argc--;
	if(argc == 1) {
		filename = argv[1];
	}

	int line_number{0};


	if(argc == 0) {
		std::cout << "PUS PE PAUZA!\n";
		std::cout << "INTERPRETEAZA DOAR DINTR-O FILA DEOCAMDATA!\n";
		std::cout << "VEI PUTEA INTERPRETA SI DE AICI IN URMATORUL UPDATE\n";
		throw 69;



		/* Interpreteaza direct in terminal */

		std::cout << ">> ";
		while(getline(std::cin, sourceCode)) {
			if(sourceCode.length() == 0) {
				std::cout << ">> ";
				continue;
			}
			sourceCode2 = sourceCode;

			std::vector<std::pair<std::string, int>> tokens{}; // every token and its type on this line

			sourceCode += "$";
			while(character != '$' and p < (int) sourceCode.length() - 1) {
				int x = getNextToken();


				std::pair<std::string, int> token(currentToken, x);


				// token_FORCE_QUIT este un token care este returnat atunci cand
				//pe linie, ultimul caracter este ' ', '\t'
				// Acesta trebuie ignorat
				if(x != token_FORCE_QUIT) {
					tokens.push_back(token);
				}
			}
			p = 0;
			character = ' '; // asta putea fi setat la orice in afara de $
			
			try {
			}
			catch(errorsTypes n) {
				switch(n) {
					case syntaxError:
						errors::syntax_error(filename, sourceCode2, line_number);
						break;
					case divisionByZero:
						errors::division_by_zero(filename, sourceCode2, line_number);
						break;
					case invalidFile:
						errors::invalid_file(filename);
						break;
					case tooManyArgs:
						errors::too_many_args();
						break;
					case uninitialisedVar:
						errors::uninitialised_var(filename, sourceCode2, line_number);
						break;
					case badInput:
						errors::bad_input(filename, sourceCode2, line_number);
						break;
					case forgotThen:
						errors::forgot_then(filename, sourceCode2, line_number);
						break;
					case forgotExecute:
						errors::forgot_execute(filename, sourceCode2, line_number);
						break;
				}
			}



			std::cout << ">> ";

		}
	}
	else if(argc == 1) {
		std::vector<std::string> all_lines{}; // used for errors
			/* Citeste din fisier, pune in variabila sourceCode (declarara in lexer.h) si interpreteaza */
		try {
			mainblocks.create_new_block(MainProgram);
			
			std::ifstream sourceFile(filename);
			if(sourceFile) {

				while(getline(sourceFile, sourceCode)) {
					all_lines.push_back(sourceCode);
					sourceCode2 = sourceCode;
					sourceCode += "$";
					std::vector<std::pair<std::string, int>> tokens{}; // every token and its type on this line

					if(sourceCode.size() != 0) {

						while(character != '$' and p < (int) sourceCode.length() - 1) {
							int x = getNextToken();
							std::pair<std::string, int> token(currentToken, x);


							// Daca este "//" atunci dam skip la ce mai este din linia aceasta de cod
							if(x == token_COMMENT_LINE) {
								break;
							}


							// token_FORCE_QUIT este un token care este returnat atunci cand
							// pe linie, ultimul caracter este ' ', '\t'
							// Acesta trebuie ignorat
							if(x != token_FORCE_QUIT) {
								tokens.push_back(token);
							}
						}
					}
					line_number++;
					p = 0;
					character = ' '; // asta putea fi setat la orice in afara de $

					// daca tokens e gol inseamna ca pe aceasta linie nu exista niciun caracter sau
					// este comentat. De asemenea mai exista si linii de cod cu identare dar care nu contin
					// nimic util. Acestea pot fi considerate tot linii goale
					//
					// Daca este gol sau linia e toata comentata, dam skip (se ocupa functia parse
					// de a crea un statement gol in tree) - asta fiind necesar pentru erorile care pot
					// aparea in runtime
					//
					// In schimb daca e doar identare, vom da clear in tokens
					if(tokens.size() == 1) {
						if(tokens[0].second == token_IDENTATION) tokens.clear();
					}

					
					parse(tokens);	
				}


				// Sfarsitul parsuirii filei
				// Timpul sa executam tree-ul pe care l-am creat (variabila Statements este tot tree-ul)


				// Daca am ajuns pana aici inseamna ca in parser n-a fost nicio eroare
				// deci e safe sa facem line_number = 0 pentru a-l folosi in detectarea erorilor la
				// interpretarea tree-ului
				line_number = 0;
				execute(MainProgram, false, line_number);
					
			}
			else {
				std::cout << "Eroare: Fila inexistenta\n";
			}

		}
		catch(errorsTypes n) {
			std::string line = all_lines[line_number - 1];
			switch(n) {
				case syntaxError:
					errors::syntax_error(filename, line, line_number);
					break;
				case divisionByZero:
					errors::division_by_zero(filename, line, line_number);
					break;
				case invalidFile:
					errors::invalid_file(filename);
					break;
				case tooManyArgs:
					errors::too_many_args();
					break;
				case uninitialisedVar:
					errors::uninitialised_var(filename, line, line_number);
					break;
				case badInput:
					errors::bad_input(filename, line, line_number);
					break;
				case forgotThen:
					errors::forgot_then(filename, line, line_number);
					break;
				case forgotExecute:
					errors::forgot_execute(filename, line, line_number);
					break;


			}
		}
	}


	

	return 0;
}

