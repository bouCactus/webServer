
#include "confAST.hpp"
#include "confTypes.hpp"
#include "confParser.hpp"

/**
 * @brief   		initialazing a config class.
 * @param	path	path to the config file.
 */
Config::Config(std::string const &path)
{
	_servers = Parser(path)();
};


/**
 * @brief   		print a set of values (value_t)
 * @param	val		set of value_t to be printed.
 */
void Config::DisplayValues(values_t val)
{
	for (values_it it = val.begin(); it != val.end(); it++)
		std::cout << *it << " ";
	std::cout << "\n";
};

/**
 * @brief   		print a location in the server.
 * @param	l		Location to be printed.
 */
void Config::DisplayServerLocation(Location &l)
{
	std::cout << "\t\tRedirections: ";
	DisplayValues(l.getRedirect());
	std::cout << "\t\tRoot: " << l.getRoot() << "\n";
	std::cout << "\t\tIndexs: ";
	DisplayValues(l.getIndex());
	std::cout << "\t\tauto Index: " << l.isAutoIndex() << "\n";
	std::cout << "\t\tis POST allowed: " << l.isAllowed(POST) << "\n";
	std::cout << "\t\tis GET allowed: " << l.isAllowed(GET) << "\n";
	std::cout << "\t\tis DELETE allowed: " << l.isAllowed(DELETE) << "\n";
};

/**
 * @brief   		print a Server.
 * @param	s		Server to be printed.
 */
void Config::DisplayServerDirectives(Server &s)
{
	std::cout << "\tPorst: ";
	DisplayValues(s.getPorts());
	std::cout << "\tHost: " << s.getHost() << "\n";
	std::cout << "\tServer Names: ";
	DisplayValues(s.getServerNames());
	std::cout << "\tError page: ";
	DisplayValues(s.getErrorPage());
	std::cout << "\tMax: " << s.getMax() << "\n";
};

/**
 * @brief   		Display the AST represent the Config file.
 */
void Config::Display()
{
	int n = 0;
	for (servers_it it = _servers.begin(); it != _servers.end(); it++)
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

/**
 * @brief   		get the servers block in config file.
 * @return 			list of Servers.
 */
servers_t Config::getServers() { return _servers; }

/**
 * @brief   		get the locations block in config file.
 * @return 			list of Servers.
 */
locations_t &Server::getLocations() { return _locations; };


/**
 * @brief   		get the error_pages directives in config file.
 * @return 			map of error pages <int, string>.
 */

mapErrors_t     &Server::getmapErrors(){ return _mapErrors; };


/**
 * @brief   		get the directives block in config file.
 * @return 			list of Servers.
 */
directives_t &Server::getDirectives() { return _directives; };

/**
 * @brief   				get a location block where
 * 							location is match the param.
 * @param		location	location to search for.
 * @return 					the finded location.
 */
Location Server::at(std::string location)
{
	locations_it it = _locations.find(location);

	if (it == _locations.end())
	{
		// LOG_THROW();
		throw std::exception();
	}
	return (it->second);
}

/**
 * @brief   		get the ports values in listen directive.
 * @return 			set of port values.
 */
values_t Server::getPorts()
{
	directives_it it = _directives.find("listen");
	if (it == _directives.end())
	{
		values_t t;
		t.push_back(DEF_PORT);
		return (t);
	}
	return it->second;
};

/**
 * @brief   		get the host values in host directive.
 * @return 			the host values.
 */
value_t Server::getHost()
{
	directives_it it = _directives.find("host");
	if (it == _directives.end())
		return (DEF_HOST);
	return *(it->second.begin());
};

/**
 * @brief   		get the values in server_names directive.
 * @return 			set of server_names values.
 */
values_t Server::getServerNames()
{
	directives_it it = _directives.find("server_name");
	if (it == _directives.end())
		return (values_t());
	return it->second;
};

/**
 * @brief   		get the values in error_page directive.
 * @return 			set of error_page values.
 */
values_t Server::getErrorPage()
{
	directives_it it = _directives.find("error_page");
	if (it == _directives.end())
	{
		values_t t;
		t.push_back(DEF_ERR_PAGE);
		return (t);
	}
	return it->second;
};

/**
 * @brief   		get the value of client_max_body_size directive.
 * @return 			value of client_max_body_size.
 */
value_t Server::getMax()
{
	directives_it it = _directives.find("client_max_body_size");
	if (it == _directives.end())
		return (DEF_MAX);
	return *(it->second.begin());
};

/**
 * @brief   		get all the directive in location block.
 * @return 			map of the directive in location block.
 */
directives_t &Location::getDirectives()
{
	return _directives;
};

/**
 * @brief   		check if request is allowed or not.
 * @param	req		the request to be checked.
 * @return 			bool indicate wither the request is allowed or not.
 */
bool Location::isAllowed(Req req)
{
	directives_it it = _directives.find("allow");
	std::string method;

	if (it == _directives.end())
		return (DEF_ALLOW);
	if (req == POST)
		method = "POST";
	if (req == GET)
		method = "GET";
	
	values_it v_it = find(it->second.begin(), it->second.end(), method);
	if (v_it == it->second.end())
		return false;
	return true;
};

/**
 * @brief   		get the values of return directive.
 * @return 			values of the return directive.
 */
values_t Location::getRedirect()
{
	directives_it it = _directives.find("return");
	if (it == _directives.end())
		return (values_t());
	return it->second;
};


int Location::getCgiTimeOut(){
	directives_it it = _directives.find("cgi_time_out");
	if (it == _directives.end())
		return (DEF_CGI_TIME_OUT);
	return atoi(it->second.begin()->c_str());
}
/**
 * @brief   		get the values of upload_path directive.
 * @return 			values of the upload_path directive.
 */

value_t Location::getUploadPath(){
	directives_it it = _directives.find("upload_path");
	if (it == _directives.end())
		return (value_t());
	return *(it->second.begin());
}

/**
 * @brief   		get the value of root directive.
 * @return 			value of the root directive.
 */
value_t Location::getRoot()
{
	directives_it it = _directives.find("root");
	if (it == _directives.end())
		return (DEF_ROOT);
	return *(it->second.begin());
};

/**
 * @brief   		check id autoindex is on or off
 * @return 			bool indicate wither autoindex is enabled.
 */
bool Location::isAutoIndex()
{
	directives_it it = _directives.find("autoindex");
	if (it == _directives.end())
		return (DEF_AUTOINDEX);
	if (*(it->second.begin()) == "off")
		return false;
	return true;
};

/**
 * @brief   		get the value of index directive.
 * @return 			value of the index directive.
 */
values_t Location::getIndex()
{
	directives_it it = _directives.find("index");
	if (it == _directives.end())
	{
		values_t t;
		t.push_back(DEF_INDEX);
		return (t);
	}
	return it->second;
};

CGIMap_t &Location::getCGI()
{
	return _CGIMap;
}

bool Location::isCGIAllowed(Req req)
{
	directives_it it = _directives.find("cgi_allow");
	std::string method;

	if (it == _directives.end())
		return (false);
	if (req == POST)
		method = "POST";
	if (req == GET)
		method = "GET";
	values_it v_it = find(it->second.begin(), it->second.end(), method);
	if (v_it == it->second.end())
		return false;
	return true;
};