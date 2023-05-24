
#include "confAST.hpp"
#include "confTypes.hpp"
#include "confParser.hpp"


// Config
Config::Config(std::string const &path) {
	_servers = Parser(path)();
};

void Config::print()  {
	servers_it s_it = _servers.begin();
	int i = 0;

	for (; s_it != _servers.end() && ++i; s_it++ )
	{
		std::cout << "******* server [ " << i;
		std::cout << " ] *******\n";
		directives_t d = s_it->getDirectives();
		directives_it d_it = d.begin();
		for (; d_it != d.end(); d_it++)
		{
			std::cout << d_it->first << " : ";
			values_it v_it = d_it->second.begin();
			for (; v_it != d_it->second.end(); v_it++)
				std::cout << *v_it << " ";
			std::cout << "\n";
		}
		locations_t l = s_it->getLocations();
		locations_it l_it = l.begin();
		for (; l_it != l.end(); l_it++)
		{
			std::cout << "---- location [ ";
			std::cout << l_it->first << " ] ------\n";
			
			directives_t &d = l_it->second.getDirectives();
			directives_it d_it = d.begin();
			for (; d_it != d.end(); d_it++)
			{
				std::cout << d_it->first << " : ";
				values_it v_it = d_it->second.begin();
				for (; v_it != d_it->second.end(); v_it++)
					std::cout << *v_it << " ";
				std::cout << "\n";
			}
		}

	}
}

servers_t Config::getServers() { return _servers; }

// Server

locations_t     &Server::getLocations() { return _locations; };
directives_t    &Server::getDirectives() { return _directives; };

// SEND ME YOUR LOACTION,, Let's focus on communicating
Location		Server::at(std::string location) {
	locations_it it = _locations.find(location);
	if (it == _locations.end())
		LOG_THROW(); throw std::exception();
	return (it->second);
}

values_t    Server::getPorts(){
    directives_it it = _directives.find("listen");
    if (it == _directives.end())
    {
		values_t t;
		t.insert(DEF_PORT);
		return  (t);
    }
    return  it->second;
};

value_t         Server::getHost(){
	directives_it it = _directives.find("host");
    if (it == _directives.end())
		return  (DEF_HOST);
    return  *(it->second.begin());
};

values_t        Server::getServerNames(){
	directives_it it = _directives.find("server_name");
    if (it == _directives.end())
		return  (values_t());
    return  it->second;
};

value_t         Server::getErrorPage(){
	directives_it it = _directives.find("error_page");
    if (it == _directives.end())
		return  (DEF_ERR_PAGE);
    return  *(it->second.begin());
};

value_t         Server::getMax(){
	directives_it it = _directives.find("client_max_body_size");
    if (it == _directives.end())
		return  (DEF_MAX);
    return  *(it->second.begin());
};

// Location
directives_t    &Location::getDirectives() { 
	return _directives; 
};


bool            Location::isAllowed(Req req){
	directives_it it = _directives.find("allow");
	std::string method;

	if (it == _directives.end())
		return (DEF_ALLOW);
	if (req == POST) method = "POST";
	if (req == GET) method = "GET";
	values_it v_it = it->second.find(method);
	if (v_it == it->second.end())
		return false;
	return true;
};

values_t        Location::getRedirect(){
	directives_it it = _directives.find("return");
	if (it == _directives.end())
		return  (values_t());
	return it->second;
};

value_t         Location::getRoot(){
	directives_it it = _directives.find("root");
	if (it == _directives.end())
		return  (DEF_ROOT);
	return *(it->second.begin());
};

bool            Location::isAutoIndex(){
	directives_it it = _directives.find("autoindex");
	if (it == _directives.end())
		return  (DEF_AUTOINDEX);
	if (*(it->second.begin()) == "off") return false;
	return true;
};

values_t        Location::getIndex(){
	directives_it it = _directives.find("index");
	if (it == _directives.end())
	{
		values_t t;
		t.insert(DEF_INDEX);
		return  (t);
	}
	return it->second;
};

