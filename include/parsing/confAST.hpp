#pragma once
#include <iostream>
#include "confTypes.hpp"






class Location {
    private:
        directives_t    _directives;
        CGIMap_t       _CGIMap;
    public:
        directives_t    &getDirectives();
        bool            isAllowed(Req req);
        values_t        getRedirect();
        value_t         getRoot();
        bool            isAutoIndex();
        values_t        getIndex();
        CGIMap_t        &getCGI();
        bool            isCGIAllowed(Req req);
        value_t         getUploadPath();
        int             getCgiTimeOut();

};

class Server {
    private:
        directives_t    _directives;
        locations_t     _locations;
        mapErrors_t     _mapErrors;
    public:
        locations_t     &getLocations();
        directives_t    &getDirectives();
        mapErrors_t     &getmapErrors();
        Location        at(std::string location);
        values_t        getPorts();
        value_t         getHost();
        values_t        getServerNames();
        values_t        getErrorPage();
        value_t         getMax();
};

class Config {
    private:
        std::string _path;
        servers_t _servers;
    public:
        Config(std::string const &path);
        servers_t getServers();
        void DisplayValues(values_t val);
        void DisplayServerDirectives(Server &s);
        void DisplayServerLocation(Location &l);
        void Display();
};

