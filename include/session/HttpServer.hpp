
#ifndef _HTTPSERVER_HPP_
#define _HTTPSERVER_HPP_

#include "HttpMethodHelpers.hpp"
#include "HttpClient.hpp"
#include "HttpTypes.hpp"
#include <iterator>

/*** HttpServer ***/
typedef std::pair<int, servers_it>  serverConf;

/*** Clients ***/
class HttpClient;
typedef std::list<HttpClient*>  clients_t;
typedef clients_t::iterator     client_it;

/*** Sockets ***/
typedef std::map<int, servers_it>   serverSock_t;
typedef serverSock_t::iterator      serverSock_it;

class HttpServer
{
    HttpServer();
    HttpServer& operator=(const HttpServer& httpServer);

    public:
        HttpServer(servers_t& servers);
        HttpServer(const HttpServer& httpServer);
        ~HttpServer();

        /***************************************************************/
        /*********** Setting up and Starting the HttpServer ************/
        /***************************************************************/
        int     createNewSocket();
        int     setupServers(servers_it &server, int socket, const std::string& port);
        int     biding_socket(servers_it &server, int &socket, const char* port);
        void    start(HttpServer &httpServer);

        bool    waitingForActivity(fd_set &tempReadfds, fd_set &tempWritefds);
        int     acceptIncomingConnection(fd_set &tempReadfds);
        void    checkForReading(fd_set &tempReadfds);
        void    checkForWriting(fd_set &tempWritefds);

        void    closeServerSockets();


        /*************************************************/
        /*************** Helpers and Tools ***************/
        /*************************************************/
        void    removeClient(client_it& client);

        /*************************************************/
        /************** Getters and Setters **************/
        /*************************************************/
        serverSock_t&   getServerSockets();
        clients_t&      getClients();
        int             getMaxFileDescriptor();

        void            setNewFD(int newsocket);
        void            addNewClient(HttpClient* client);

        // Used to check if all servers are successfully setting up.
        size_t serversNotConnected;
        size_t maxServerSockets;

    private:
        clients_t       _clients;
        serverSock_t    serverSockets;
        fd_set          readfds;
        fd_set          writefds;
        int             maxFileDescriptor;
};

#endif  //__HTTPSERVER__HPP


