#include <iostream>
#include "../src/includes/parser.hpp"

void foo(){
	throw Parse_error(std::string("") + "testy");
};

void parser_test(char *path)
{
	// A FAILIER IS SOMEONE WHO STOP TRYING, RIGHT??
	try {
		// JUST SOME OLD STUFF, IGONRE IT, SOMETHING REALLY GOOD AWAITING YOU ..

			/*Lexer lxr(path);
			lxr.log(0);
			Token t = lxr.get_next_token();
			while (t.token_type != END)
			{
				std:: cout << "token value : ";

				std::cout << t.val << " token column: ";
				std::cout << t.column << " \n";
				t = lxr.get_next_token();
			}
			foo();*/

		// IT STARTS WITH THIS CRIPY CLASS, THE PARSER!!
		Parser pr(path);
		std::vector<server> servers = pr.parse();
		std::cout << "size: "<< servers.size() << "\n";
		// NO ONE WANT TO READ tHIS FUNCTION, BUT HEY, ALL IT DID IS GIVING YOU DATA!!
		pr.print_servers();
	} catch(Parse_error &e)
	{
		// WHATEVER!!
		std::cout << "err: "<< e.what() << "\n";
	}
}