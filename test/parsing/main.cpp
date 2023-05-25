#include <iostream>

using namespace std;

#include <iostream>
#include "confAST.hpp"
#include "confParseError.hpp"
#include "confTypes.hpp"

void foo(){
	throw Parse_error(std::string("") + "testy");
};

void print_set(values_t val)
{
	values_it vi = val.begin();
	for (; vi != val.end(); vi++)
		std:: cout << *vi << " ";
	std::cout << "\n";
}
void parser_test(char *path)
{
	// A FAILIER IS SOMEONE WHO STOP TRYING, RIGHT??
	try {
		Config conf(path);
		//conf.print();
		servers_t s = conf.getServers();

		servers_it ser = s.begin();
		std:: cout << "Ports: ";
		print_set(ser->getPorts());
		std:: cout << "Host: ";
		std::cout << ser->getHost() << "\n";
		std::cout << "Server Names: ";
		print_set(ser->getServerNames());
		std:: cout << "Error Page: ";
		std::cout << ser->getErrorPage() << "\n";
		std:: cout << "Max: ";
		std::cout << ser->getMax() << "\n";

		std::cout << ser->at("/fu").isAutoIndex();

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