
#include "confAST.hpp"
#include "confTypes.hpp"
#include "confParser.hpp"


// Config
Config::Config(std::string const &path) {
	_servers = Parser(path)();
};


void Config::DisplayValues(values_t val) {
	for(values_it it = val.begin(); it != val.end(); it++)
		std::cout << *it << " ";
	std::cout << "\n";
};

void Config::DisplayServerLocation(Location &l) {
	std::cout << "\t\tRedirections: "; DisplayValues(l.getRedirect());
	std::cout << "\t\tRoot: " << l.getRoot() << "\n";
	std::cout << "\t\tIndexs: "; DisplayValues(l.getIndex());
	std::cout << "\t\tauto Index: " << l.isAutoIndex() << "\n";
	std::cout << "\t\tis POST allowed: " << l.isAllowed(POST) << "\n";
	std::cout << "\t\tis GET allowed: " << l.isAllowed(GET) << "\n";
	std::cout << "\t\tis DELETE allowed: " << l.isAllowed(DELETE) << "\n";
};
void Config::DisplayServerDirectives(Server &s) {
	std::cout << "\tPorst: "; DisplayValues(s.getPorts());
	std::cout << "\tHost: " << s.getHost() << "\n";
	std::cout << "\tServer Names: "; DisplayValues(s.getServerNames());
	std::cout << "\tError page: " << s.getErrorPage() << "\n";
	std::cout << "\tMax: " << s.getMax() << "\n";
};

void Config::Display() {
	int n = 0;
	for(servers_it it = _servers.begin(); it != _servers.end(); it++)
	{
		std::cout << "SERVER [ " << ++n << " ]\n";
		DisplayServerDirectives(*it);
		locations_t locs = it->getLocations();
		int j = 0;
		for (locations_it l = locs.begin(); l != locs.end(); l++)
		{
			std::cout << "\tLoaction [ " << ++j << " ]\n";
			DisplayServerLocation(l->second);
		}
	}
};


servers_t Config::getServers() { return _servers; }

// Server

locations_t     &Server::getLocations() { return _locations; };
directives_t    &Server::getDirectives() { return _directives; };

// SEND ME YOUR LOACTION,, Let's focus on communicating
Location		Server::at(std::string location) {
	locations_it it = _locations.find(location);

	if (it == _locations.end()){
	  LOG_THROW();
	  throw std::exception();
	}
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

