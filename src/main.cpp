#include "HttpTypes.hpp"

int	main(int ac, char** av) {
	if (ac != 2) {
		std::cerr << "error: Invalide number of arguments." << std::endl;
		exit(1);
	}
	try {
		/*** Parsing the configuration file ***/ 
		Config conf(av[1]);

		/*** Multiplexing ***/
		servers_t	servers = conf.getServers();
		HttpServer	httpServer(servers);

		httpServer.start();

		// httpServer.closeServerSockets();
	}
	catch(std::exception &e)
	{
		// WHATEVER!!
		std::cout << e.what() << std::endl;
	}
	return  (0);
}
