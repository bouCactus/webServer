#pragma once
#include <iostream>
#include "confTypes.hpp"




class Location {
    private:
        directives_t    _directives;
    public:
        directives_t    &getDirectives();
        bool            isAllowed(Req req);
        values_t        getRedirect();
        value_t         getRoot();
        bool            isAutoIndex();
        values_t        getIndex();

};

class Server {
    private:
        directives_t    _directives;
        locations_t     _locations;
    public:
        locations_t     &getLocations();
        directives_t    &getDirectives();
        Location        at(std::string location);
        values_t        getPorts();
        value_t         getHost();
        values_t        getServerNames();
        value_t         getErrorPage();
        value_t         getMax();
};

class Config {
    private:
        std::string _path;
        servers_t _servers;
    public:
        Config(std::string const &path);
        servers_t getServers();
        void print() ;
};

