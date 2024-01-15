#include <iostream>
#include <fstream>
#include <utility>
#include "interpreter.hpp"


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
		std::vector<statement*> BlocksStatement{};


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
		// daca e mai mare decat size - 1: eroare
		//
		// altfel
		// se ia numarul de identari = i
		// si vom adauga statement-ul curent la Blocks[i]
		// daca Blocks.size() - 1 > i se da erase de la i + 1 pana la final
		//
		// BlocksStatement este un vector care contine statement-ul corespunzator pentru block-ul de cod din Blocks
		// spre ex:
		// sa zicem ca avem un statement numit Statement cu whileAst_p valid
		// in cazul asta
		// "Statement->whileAst_p->block" este blocul de cod
		// iar statementul corespunzator acestui bloc de cod este "Statement"
		//
		// Va fi useful cand va trebui sa accesez statement-ul corespunzator blocului de cod de tip doAst_p spre ex
		// pentru ca sintaxa este:
		// executa
		//     [...]
		// cat timp [expr]
		//
		// cand dau de "executa" eu doar creez un statement cu doAst_p valid, iar apoi bun blocul de cod in "doAst_p->block"
		// de aici, "doAst_p->condition" ramane invalid (nu, nu fac look ahead)
		// iar cand ajuns la linia de cod unde este conditia (cat timp [expr]), am pierdut deja statement-ul
		//	
		// dar, daca am BlocksStatement pot accesa fiecare statement usor
		// doar Blocks[0] nu are un statement real (pentru ca acela este main program)



		// block-ul si statement-ul din care face parte 
		void create_new_block(statements *block, statement *Statement) {
			Blocks.push_back(block);
			BlocksStatement.push_back(Statement);
		}
		void delete_last_block() {
			Blocks.pop_back();
			BlocksStatement.pop_back();
		}
		void delete_x_to_last_block(int start, bool all) {
			int BlocksSize = (int) Blocks.size();
			int BlocksStatementSize = (int) BlocksStatement.size();
			for(int i = start; i < BlocksSize; i++) {
				if(all and Blocks[i]) {
					delete Blocks[i];
				}
			}
			if(start < BlocksSize) Blocks.erase(Blocks.begin(), Blocks.begin() + BlocksSize);
			for(int i = start; i < BlocksStatementSize; i++) {
				if(all and BlocksStatement[i]) delete BlocksStatement[i];
			}
			if(start < BlocksStatementSize) BlocksStatement.erase(BlocksStatement.begin(), BlocksStatement.begin() + BlocksStatementSize);
		}


		void add_line(statement *Statement, int identation_number) {
			Blocks[identation_number]->add(Statement); // am adaugat in Blocks
													   
			for(int i = identation_number + 1; i < (int) Blocks.size(); i++) {
				if(BlocksStatement[i]->doAst_p and !BlocksStatement[i]->doAst_p->isCondition) throw syntaxError;
			}

		}

		// update in functie de identation_number
		// practic in functie de identation_number va sterge (daca e nevoie) ultimele blocuri din vectorul Blocks
		void update(int identation_number, bool second, int &line_number, bool isRealLine) {

			if(identation_number > (int) Blocks.size() - 1) throw syntaxError;

			// verificam daca fiecare statements din Blocks de la identation_number + 1 in sus
			// are cel putin un statement inainte de a le sterge
			for(int i = identation_number + 1; i < (int) Blocks.size(); i++) {
				if(Blocks[i]->s.size() == 0 or isNone(Blocks[i]->s)) {
					line_number -= Blocks[i]->s.size();
					if(isRealLine) line_number--;
					throw syntaxError;
				}
			}

			for(int i = identation_number + 2; i < (int) Blocks.size(); i++) {
				if(BlocksStatement[i]->doAst_p and !BlocksStatement[i]->doAst_p->isCondition) throw syntaxError;
			}


			if(identation_number < (int) Blocks.size() - 1) {
				// ce sa stergem?
				// in mod normal stergem de le identation_number + 1 pana la final
				// problema e la do while loops
				// ex:
				// executa
				//     scrie 1
				// cat timp Adevarat
				//
				// acel cat timp Adevarat face parte din doAst chiar daca nu este identat ca restul codului
				// deci prima data stergem de la identation_number + 2 pana la final
				if(identation_number < (int) Blocks.size() - 2) {
					Blocks.erase(Blocks.begin() + identation_number + 2, Blocks.end());
					BlocksStatement.erase(BlocksStatement.begin() + identation_number + 2, BlocksStatement.end());
				}


				// iar acum verificam daca ultimul statement este un do while (sau un else if / else dupa caz)
				// daca este, nu putem sterge direct acel identation_number + 1, pentru ca trebuie mai intai sa parsam
				// aceasta linie ca sa vedem acel cat timp, deci dam skip deocamdata
				if(!second) {
					Blocks.erase(Blocks.begin() + identation_number + 1, Blocks.end());
					BlocksStatement.erase(BlocksStatement.begin() + identation_number + 1, BlocksStatement.end());
				}

			}

		}
};

statements *MainProgram = new statements;
ProgramBlocks mainblocks{};


// ultimii 2 parametri se folosesc doar cand interpretez in terminal
void parse(std::vector<std::pair<std::string, int>>& tokens, bool inTerminal, int &enter_exit, int &line_number) {
		
	statement *Statement = new statement;

	// Daca tokens e gol, statement-ul va fi gol
	if(tokens.size() == 0) {
		Statement->isNone = true;
		mainblocks.add_line(Statement, mainblocks.Blocks.size() - 1);
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

			mainblocks.update(identation_number, false, line_number, true);
			Statement->outAst_p = parseOut::parseEntry(tokens);
			mainblocks.add_line(Statement, identation_number);
			break;
		}
		case token_INPUT:
		{
			mainblocks.update(identation_number, false, line_number, true);
			Statement->inAst_p = parseIn::parseEntry(tokens);
			mainblocks.add_line(Statement, identation_number);
			break;
		}
		case token_IDENTIFIER:
		{
			mainblocks.update(identation_number, false, line_number, true);
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
		case token_EXECUTE:
		{
			if((int) tokens.size() > 1) throw syntaxError; // este necesar pt ca nu am o functie separata care sa parsuiasca linia asta
			mainblocks.update(identation_number, false, line_number, true);
			Statement->doAst_p = new doAst;
			mainblocks.add_line(Statement, identation_number);
			mainblocks.create_new_block(Statement->doAst_p->block, Statement);
			break;
		}
		case token_WHILE:
		{
			// poate fi un while loop sau un while de la sfarsitul lui do while
			// trebuie sa le parsam diferit


			mainblocks.update(identation_number, true, line_number, true);
			if(mainblocks.BlocksStatement.back()->doAst_p and identation_number == (int) mainblocks.Blocks.size() - 2) {

				mainblocks.BlocksStatement.back()->doAst_p = parseDo::parseEntry(tokens,mainblocks.BlocksStatement.back()->doAst_p);
				//mainblocks.add_line(Statement, identation_number);

				if(inTerminal) {
					// daca e in terminal sterge tot de la acest do while in sus
					// fac asta ca atunci cand esti in terminal si scrii un do while
					// dupa ce ai pus
					// cat timp [expr]
					//
					// sa nu trebuiasca sa mai pui inca 2 randuri libere in continuare
					// ca sa execute (cum e la celelalte ex: while loop)
					mainblocks.delete_x_to_last_block(identation_number + 1, false);
					enter_exit = 2;
				}
			}
			else {
				mainblocks.update(identation_number, false, line_number, true);
				Statement->whileAst_p = parseWhile::parseEntry(tokens);
				mainblocks.add_line(Statement, identation_number);
				mainblocks.create_new_block(Statement->whileAst_p->block, Statement);
			}
			break;
		}
		case token_FOR:
		{
			mainblocks.update(identation_number, false, line_number, true);
			Statement->forAst_p = parseFor::parseEntry(tokens);
			mainblocks.add_line(Statement, identation_number);
			mainblocks.create_new_block(Statement->forAst_p->block, Statement);
			break;
		}
		case token_IF:
		{
			mainblocks.update(identation_number, false, line_number, true);
			Statement->ifelseAst_p = parseIfelse::parseEntryIf(tokens);
			mainblocks.add_line(Statement, identation_number);
			mainblocks.create_new_block(Statement->ifelseAst_p->blocks[0], Statement);
			break;
		}
		case token_ELSE_IF:
		{

			mainblocks.update(identation_number, true, line_number, true);
			if(mainblocks.BlocksStatement.back()->ifelseAst_p and identation_number == (int) mainblocks.Blocks.size() - 2) {
				mainblocks.BlocksStatement.back()->ifelseAst_p = parseIfelse::parseEntryElseIf(tokens, mainblocks.BlocksStatement.back()->ifelseAst_p);
			//	mainblocks.add_line(Statement, identation_number + 1);
				
				mainblocks.Blocks.pop_back();
				mainblocks.Blocks.push_back(mainblocks.BlocksStatement.back()->ifelseAst_p->blocks.back());
			}
			else {
				throw elseWithoutIf;
			}

			break;
		}
		case token_ELSE:
		{
			if((int) tokens.size() > 1) throw syntaxError; // este necesar pt ca nu am o functie separata care sa parsuiasca linia asta
			//if(mainblocks.BlocksStatement.back()->ifelseAst_p->blockElse) throw onlyOneElseError;
			mainblocks.update(identation_number, true, line_number, true);
			if(mainblocks.BlocksStatement.back()->ifelseAst_p and identation_number == (int) mainblocks.Blocks.size() - 2) {
				if(mainblocks.Blocks[identation_number]->s.back()->ifelseAst_p->blockElse) throw onlyOneElseError;

			//	mainblocks.add_line(Statement, identation_number);
				
				mainblocks.Blocks.pop_back();
				mainblocks.BlocksStatement.back()->ifelseAst_p->blockElse = new statements;
				mainblocks.Blocks.push_back(mainblocks.BlocksStatement.back()->ifelseAst_p->blockElse);
			}
			else {
				throw elseWithoutIf;
			}
			break;

		}
		default:
		{
			mainblocks.update(identation_number, false, line_number, true);
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
			if(Statement->whileAst_p->block->s.size() == 0) throw syntaxError;
			exprAst *condition_copy = new exprAst(*Statement->whileAst_p->condition);
			CopyTree::copy_exprAst(Statement->whileAst_p->condition, condition_copy);

			int line_number_copy = line_number;
			
			if(!interpretExpr::interpretEntryReturnInt(condition_copy)) {
				line_number += getAllLinesNumber(Statement->whileAst_p->block->s);
			}
				
			while(interpretExpr::interpretEntryReturnInt(condition_copy)) {
				statements* new_block = new statements(*Statement->whileAst_p->block);
				CopyTree::Entry(Statement->whileAst_p->block, new_block);


				line_number = line_number_copy;
				execute(new_block, false, line_number);

				condition_copy = new exprAst(*Statement->whileAst_p->condition);
				CopyTree::copy_exprAst(Statement->whileAst_p->condition, condition_copy);
			}
		}
		else if(Statement->forAst_p) {
			// NU, nu pot pune chestia asta intr-o functie separata cum am facut ca la
			// scrie, citeste, whatever
			// pentru ca trebuie sa apelez la functia execute() tot de aici
			// nu pot sa o chem din interpret.h >:(

			if(Statement->forAst_p->block->s.size() == 0) throw syntaxError;
			varNode *it = new varNode(*Statement->forAst_p->assign);
			it->expr = new exprAst(*Statement->forAst_p->assign->expr);
			CopyTree::copy_exprAst(Statement->forAst_p->assign->expr, it->expr);
			interpretVar::interpretEntry(it);
			if(variables[it->varName].type != numberType) throw syntaxError;

			exprAst *up_copy = new exprAst(*Statement->forAst_p->UpperBoundary);
			CopyTree::copy_exprAst(Statement->forAst_p->UpperBoundary, up_copy);

			int line_number_copy = line_number;
			if(Statement->forAst_p->default_step) {
				if(variables[Statement->forAst_p->iteratorName].numberValue <= interpretExpr::interpretEntryReturnDouble(up_copy)) {
					up_copy = new exprAst(*Statement->forAst_p->UpperBoundary);
					CopyTree::copy_exprAst(Statement->forAst_p->UpperBoundary, up_copy);


					for(double i = variables[Statement->forAst_p->iteratorName].numberValue; i <= interpretExpr::interpretEntryReturnDouble(up_copy); i++) {
						line_number = line_number_copy;
						statements *new_block = new statements(*Statement->forAst_p->block);
						CopyTree::Entry(Statement->forAst_p->block, new_block);

						execute(new_block, false, line_number);

						up_copy = new exprAst(*Statement->forAst_p->UpperBoundary);
						CopyTree::copy_exprAst(Statement->forAst_p->UpperBoundary, up_copy);

						variables[Statement->forAst_p->iteratorName].numberValue++;
					}
				}
				else {
					up_copy = new exprAst(*Statement->forAst_p->UpperBoundary);
					CopyTree::copy_exprAst(Statement->forAst_p->UpperBoundary, up_copy);


					for(double i = variables[Statement->forAst_p->iteratorName].numberValue; i >= interpretExpr::interpretEntryReturnDouble(up_copy); i--) {
						line_number = line_number_copy;
						statements *new_block = new statements(*Statement->forAst_p->block);
						CopyTree::Entry(Statement->forAst_p->block, new_block);

						execute(new_block, false, line_number);

						up_copy = new exprAst(*Statement->forAst_p->UpperBoundary);
						CopyTree::copy_exprAst(Statement->forAst_p->UpperBoundary, up_copy);

						variables[Statement->forAst_p->iteratorName].numberValue--;
					}

				}
			}
			else {
				

				if(variables[Statement->forAst_p->iteratorName].numberValue < interpretExpr::interpretEntryReturnDouble(up_copy)) {
					up_copy = new exprAst(*Statement->forAst_p->UpperBoundary);
					CopyTree::copy_exprAst(Statement->forAst_p->UpperBoundary, up_copy);


					while(variables[Statement->forAst_p->iteratorName].numberValue <= interpretExpr::interpretEntryReturnDouble(up_copy)) {
						line_number = line_number_copy;
						statements *new_block = new statements(*Statement->forAst_p->block);
						CopyTree::Entry(Statement->forAst_p->block, new_block);

						execute(new_block, false, line_number);

						up_copy = new exprAst(*Statement->forAst_p->UpperBoundary);
						CopyTree::copy_exprAst(Statement->forAst_p->UpperBoundary, up_copy);


						varNode *step_copy = new varNode(*Statement->forAst_p->step);
						step_copy->expr = new exprAst(*Statement->forAst_p->step->expr);
						CopyTree::copy_exprAst(Statement->forAst_p->step->expr, step_copy->expr);
						interpretVar::interpretEntry(step_copy);
					}
				}
				else if(variables[Statement->forAst_p->iteratorName].numberValue > interpretExpr::interpretEntryReturnDouble(up_copy)) {
					up_copy = new exprAst(*Statement->forAst_p->UpperBoundary);
					CopyTree::copy_exprAst(Statement->forAst_p->UpperBoundary, up_copy);


					while(variables[Statement->forAst_p->iteratorName].numberValue >= interpretExpr::interpretEntryReturnDouble(up_copy)) {
						line_number = line_number_copy;
						statements *new_block = new statements(*Statement->forAst_p->block);
						CopyTree::Entry(Statement->forAst_p->block, new_block);

						execute(new_block, false, line_number);

						up_copy = new exprAst(*Statement->forAst_p->UpperBoundary);
						CopyTree::copy_exprAst(Statement->forAst_p->UpperBoundary, up_copy);

						

						varNode *step_copy = new varNode(*Statement->forAst_p->step);
						step_copy->expr = new exprAst(*Statement->forAst_p->step->expr);
						CopyTree::copy_exprAst(Statement->forAst_p->step->expr, step_copy->expr);
						interpretVar::interpretEntry(step_copy);
					}
				}
				else {
					statements *new_block = new statements(*Statement->forAst_p->block);
					CopyTree::Entry(Statement->forAst_p->block, new_block);
					execute(new_block, false, line_number);
				}
			}
		}
		else if(Statement->doAst_p) {

			if(Statement->doAst_p->block->s.size() == 0) throw syntaxError;
			exprAst *condition_copy;
			
			int line_number_copy = line_number;


			do {
				line_number = line_number_copy;
				condition_copy = new exprAst(*Statement->doAst_p->condition);
				CopyTree::copy_exprAst(Statement->doAst_p->condition, condition_copy);

				statements* new_block = new statements(*Statement->doAst_p->block);
				CopyTree::Entry(Statement->doAst_p->block, new_block);

				execute(new_block, false, line_number);
				line_number++;

			}
			while(interpretExpr::interpretEntryReturnInt(condition_copy));


		}
		else if(Statement->ifelseAst_p) {
			if(Statement->ifelseAst_p->blocks.size() == 0) throw syntaxError;

			bool taken = false;

			for(int i{}; i < (int) Statement->ifelseAst_p->blocksExpr.size(); i++) {
				exprAst *expression = new exprAst(*Statement->ifelseAst_p->blocksExpr[i]);
				CopyTree::copy_exprAst(Statement->ifelseAst_p->blocksExpr[i], expression);
				if(interpretExpr::interpretEntryReturnInt(expression) and !taken) {
					taken = true;

					statements *new_block = new statements(*Statement->ifelseAst_p->blocks[i]);
					CopyTree::Entry(Statement->ifelseAst_p->blocks[i], new_block);

					execute(new_block, false, line_number);
					line_number++;

				}
				else {
					line_number += getAllLinesNumber(Statement->ifelseAst_p->blocks[i]->s) + 1;
				}
				
			}

			if(!taken and Statement->ifelseAst_p->blockElse) {
				statements *new_block = new statements(*Statement->ifelseAst_p->blockElse);
				CopyTree::Entry(Statement->ifelseAst_p->blockElse, new_block);
				execute(new_block, false, line_number);
			}
			else if(Statement->ifelseAst_p->blockElse) {
				line_number += getAllLinesNumber(Statement->ifelseAst_p->blockElse->s) + 1;
			}
			line_number--;


			
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
		/* Interpreteaza direct in terminal */

		int enter_exit{};
		std::vector<std::string> all_lines{}; // used for errors
		delete MainProgram;

		statement *FakeStatement{};



		std::cout << ">> ";
		while(getline(std::cin, sourceCode)) {
			// main program nu are Statement, deci vom face unul "fals"
			// executam la aproape fiecare linie deci 
			if((int) mainblocks.Blocks.size() == 0) {
				FakeStatement = new statement;
				MainProgram = new statements;
				mainblocks.create_new_block(MainProgram, FakeStatement);
			}
			
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
			try {
				if((int) mainblocks.Blocks.size() > 1 and (int) tokens.size() == 0) {
					enter_exit++;
					if(enter_exit == 2) {
						parse(tokens, true, enter_exit, line_number);
						mainblocks.update(0, false, line_number, false);
					}
				}
				else {
					enter_exit = 0;
				}
			
				parse(tokens, true, enter_exit, line_number);

				// acum executam chestia asta pe loc
				// atat timp cat mainblocks.Blocks size nu este mai mare decat 1
				if((int) mainblocks.Blocks.size() == 1) {
					line_number = 0;
					execute(MainProgram, true, line_number);
					line_number = 0;

					delete MainProgram;
					delete FakeStatement;
					mainblocks.delete_last_block();
					all_lines.clear();

					if(enter_exit == 2) {
						std::cout << "\n";
						enter_exit = 0;
					}
					std::cout << ">> ";
				}
				else {
					std::cout << ".. ";
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
					case onlyOneElseError:
						errors::only_one_else_error(filename, line, line_number);
						break;
					case elseWithoutIf: 
						errors::else_without_if(filename, line, line_number);
						break;
				}

				line_number = 0;
				mainblocks.delete_x_to_last_block(0, true);

				//delete MainProgram;
				//delete FakeStatement;
				//mainblocks.delete_last_block();
				all_lines.clear();

				std::cout << "\n>> ";
			}
		}
	}
	else if(argc == 1) {
			/* Citeste din fisier, pune in variabila sourceCode (declarara in lexer.h) si interpreteaza */
			std::vector<std::string> all_lines{}; // used for errors
		try {
			// main program nu are Statement, deci vom face unul "fals"
			statement *FakeStatement = new statement;
			mainblocks.create_new_block(MainProgram, FakeStatement);
			
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
					
					// al treilea parametru se foloseste doar in terminal
					// si n-am chef sa fac function overloading, deci folosim
					// o variabila trash
					int trash{};
					parse(tokens, false, trash, line_number);	
				}


				// Sfarsitul parsuirii filei
				// Timpul sa executam tree-ul pe care l-am creat (variabila Statements este tot tree-ul)


				// Daca am ajuns pana aici inseamna ca in parser n-a fost nicio eroare
				// deci e safe sa facem line_number = 0 pentru a-l folosi in detectarea erorilor la
				// interpretarea tree-ului
				
				
				mainblocks.update(0, false, line_number, false);
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
				case onlyOneElseError:
					errors::only_one_else_error(filename, line, line_number);
					break;
				case elseWithoutIf: 
					errors::else_without_if(filename, line, line_number);
					break;
			}
		}
	}


	

	return 0;
}

