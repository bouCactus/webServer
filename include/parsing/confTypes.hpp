#pragma once
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <list>

class Location;
class Server;

enum Key
{
	MAX,
	LISTEN,
	SERVER_NAME,
	INDEX,
	ERROR_PAGE,
	PATH,
	ROOT,
	AUTOINDEX,
	SET,
	CGI,
	HOST,
	ALLOW,
	RETURN,
	CGI_ALLOW,
	UPLOAD_PATH,
	CGI_TIME_OUT
};

enum Type
{
	SERVER,
	DIRCTIVE,
	LOCATION,
	SIMICOLONE,
	LCURL,
	RCURL,
	END
};

enum Req
{
	POST,
	GET,
	DELETE,
};

typedef std::map<std::string, std::string> CGIMap_t;
typedef CGIMap_t::iterator CGIMap_it;
typedef std::pair<std::string, std::string> CGIMap_elm;

// key and values for each directive
typedef std::string value_t;
typedef std::vector<std::string> values_t;

// server
typedef std::list<Server> servers_t;

// directive/directives
typedef std::pair<value_t, values_t> directive_t;
typedef std::map<value_t, values_t> directives_t;

// location/locations
typedef std::pair<value_t, Location> location_t;
typedef std::map<value_t, Location> locations_t;

// iterators
typedef directives_t::iterator directives_it;
typedef locations_t::iterator locations_it;
typedef values_t::iterator values_it;
typedef servers_t::iterator servers_it;

// size
typedef std::string::size_type size_s;

typedef std::map<std::string, std::string> strPair_t;
typedef strPair_t::iterator strPair_it;

// errormap
typedef std::map<std::string, std::string> mapErrors_t;
typedef mapErrors_t::iterator mapErrors_it;

#define DEF_HOST "127.0.0.1"
#define DEF_PORT "80"
#define DEF_ERR_PAGE "/home/error.html"
#define DEF_MAX "10"
#define DEF_ALLOW 0
#define DEF_ROOT "www/"
#define DEF_AUTOINDEX 0
#define DEF_INDEX "index.html"
#define DEF_CGI_TIME_OUT 10
#define DEF_UPLOAD_PATH "www/"
#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#define LOG_THROW() //std::cout << "[ throw ]: in " << __FILE__ << " (Line: " << __LINE__ << ").\n";
#else
#define LOG_THROW()
#endif
