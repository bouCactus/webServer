#ifndef	_HTTPTYPES_HPP_
#define	_HTTPTYPES_HPP_

#include <iostream>
#include <sys/types.h>     // getaddrinfo()
#include <netinet/in.h>    // sockaddr_in
#include <netdb.h>         // getaddrinfo()
#include <cstring>         // memset()
#include <cerrno>
#include <sys/select.h>    // select()
#include <sys/socket.h>    // socket(), setsockopt(), bind(), getaddrinfo()
#include <fcntl.h>         // fcntl()
#include <arpa/inet.h>     // inet_addr()
#include <cstdlib>         // atoi()
#include <algorithm>

#include "HttpServer.hpp"

class HttpServer;

#define	MAX_BUFFER_SIZE 1024

class HttpError : public std::exception {
	int _error;
	const char* what() const throw() {
		return (strerror(this->_error));
	}
	public:
		HttpError(HttpServer &httpServer, int error) : _error(error) {
			client_it	client = httpServer.getClients().begin();

			for (; client != httpServer.getClients().end(); client++) {
				close(client->getServerSocket());
				close(client->getSocket());
			}
		}
};

#endif	//__HTTPTYPES__HPP
