#include <iostream>
using namespace std;

#include <iostream>
#include "confAST.hpp"
#include "error.hpp"
#include "types.hpp"
#include <fstream>

void parser_test(char *path)
{
	// A FAILIER IS SOMEONE WHO STOP TRYING, RIGHT??
	try {
		Config conf(path);
		conf.Display();
	} catch(std::exception &e)
	{
		// WHATEVER!!
		std::cout << "err: "<< e.what() << "\n";
	}
}

int main(int ac, char *av[]) {

    if (ac != 2) return 1;
    parser_test(av[1]);
    return (0);
}