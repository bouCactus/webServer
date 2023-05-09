#include <iostream>
#include "./tests/includes/tests.hpp"
#include <exception>



int main(int ac, char **av){

	if (ac != 2)
	{
		std::cout << "please provide a confog file." << std::endl;
		return (1);
	}
	// LET'S STRAT WITH PARSAAAAAA !!
	parser_test(av[1]);
	return 0;
}