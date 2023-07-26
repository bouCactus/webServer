#include "HttpTypes.hpp"


HttpServer::HttpServer() {}

HttpServer::HttpServer(const HttpServer& httpServer) {
	*this = httpServer;
}

HttpServer& HttpServer::operator=(const HttpServer& httpServer) {
	(void)(httpServer);
	return *this;
}

HttpServer::~HttpServer() {

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	for (clients_t::iterator client = _clients.begin(); client != _clients.end(); ++client) {
    delete *client;
  }
  _clients.clear();
}


/************************************************************************/
/********************** Setting up the HttpServer ***********************/
/************************************************************************/

HttpServer::HttpServer(servers_t& servers) : maxFileDescriptor(0) {

	/*** Initializes the file descriptor sets to contain no file descriptors. ***/
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	servers_it server = servers.begin();
	values_t ports;
	for (; server != servers.end(); server++) {
		ports = server->getPorts();
		values_it itPorts = ports.begin();
		for (; itPorts != ports.end(); itPorts++) {
			int serverSocket = createNewSocket();
			setupServers(server, serverSocket, *itPorts);
			serverSockets.insert(serverConf(serverSocket,server));

			/*** Set the max-File-Descriptor. ***/
			maxFileDescriptor = std::max(maxFileDescriptor, serverSocket);
		}
	}
}

int	HttpServer::createNewSocket() {

	/*** Create a socket for the server ***/
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(serverSocket, F_SETFL, O_NONBLOCK);
	if (serverSocket == -1) {
		std::cerr << "Error creating socket" << std::endl;
		throw HttpError(*this, errno);
	}

	/*** Allows local addresses that are already in use to be bound ***/
	int yes = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		std::cerr << "Error set socket option ["<<serverSocket<<"] : ";
		throw HttpError(*this, errno);
	}
	return (serverSocket);
}

void	HttpServer::setupServers(servers_it &server, int serverSocket, const std::string& port) {
	/*** Set up server address. ***/
    
	sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(port.c_str()));
	serverAddr.sin_addr.s_addr = inet_addr(server->getHost().c_str());
	// serverAddr.sin_addr.s_addr = INADDR_ANY;
    // memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    
    /**********************************/
    // int status;
    // struct addrinfo hints;
    // struct addrinfo *servinfo;  // will point to the results

    // memset(&hints, 0, sizeof hints); // make sure the struct is empty
    // hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    // hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    // hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    // if ((status = getaddrinfo(server->getHost().c_str(), port.c_str(), &hints, &servinfo)) != 0) {
    //     fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    //     exit(1);
    // }
    /**********************************/

	/*** Bind the socket to the server address. ***/
	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cerr << "Error binding socket ["<<server->getHost().c_str()<<":"<<port.c_str()<<"] : ";
		close(serverSocket);
		throw HttpError(*this, errno);
	}

    /*** Listen for incoming connections. ***/
	if (listen(serverSocket, 1) == -1) {
		std::cerr << "Error listening on socket : ";
		close(serverSocket);
		throw HttpError(*this, errno);
	}

	// Add the server socket to the read fd set `readfds`.
	FD_SET(serverSocket, &readfds);

    std::cout << "Start listening on ["<<serverSocket<<"] ---- ["<<server->getHost().c_str()<<":"<<port.c_str()<<"]\n";
}


/************************************************************************/
/*********************** Starting the HttpServer ************************/
/************************************************************************/

void	HttpServer::start() {
	/*** Start the server ***/

	while (true) {
		fd_set tempReadfds = readfds;
		fd_set tempWritefds = writefds;

		/*** Wait for any activity on the file descriptors. ***/
		if (waitingForActivity(tempReadfds, tempWritefds)) {
            // std::cout << "Connection done\n";

            /*** Check for activity on the server socket. ***/
            if (acceptIncomingConnection(tempReadfds) == -1)
                continue;
            // std::cout << "Incoming Connection Accepted.\n";
            
            /*** Check for activity on client sockets and read from it. ***/
            checkForReading(tempReadfds);
            /*** Check for activity on client sockets for writing response. ***/
            checkForWriting(tempWritefds);
        }
	}
}

bool    HttpServer::waitingForActivity(fd_set &tempReadfds, fd_set &tempWritefds) {

    // std::cout << "/****************************** PRINT SETS **********************************/\n";
	// for(serverSock_it server = serverSockets.begin(); server != serverSockets.end(); server++) {
    //     if (FD_ISSET(server->first, &tempReadfds))
    //         std::cout << "RD. Server socket: " << server->first << std::endl;
    // }
    // for(client_it client = _clients.begin();client != _clients.end(); ++client) {
    //     if (FD_ISSET(client->getSocket(), &tempReadfds))
    //         std::cout << "RD. Client socket: " << client->getSocket() << std::endl;
    // }
    // for(serverSock_it server = serverSockets.begin(); server != serverSockets.end(); server++) {
    //     if (FD_ISSET(server->first, &tempWritefds))
    //         std::cout << "WR. Server socket: " << server->first << std::endl;
    // }
    // for(client_it client = _clients.begin();client != _clients.end(); ++client) {
    //     if (FD_ISSET(client->getSocket(), &tempWritefds))
    //         std::cout << "WR. Client socket: " << client->getSocket() << std::endl;
    // }
    // std::cout << "/****************************************************************************/\n";

	int activity = select(maxFileDescriptor + 1, &tempReadfds, &tempWritefds, NULL, NULL);
	if (activity == -1) {

        std::cerr << "Error select : \n ";
		throw HttpError(*this,errno);
	}
    return activity;
}

int	HttpServer::acceptIncomingConnection(fd_set& tempReadfds) {
    serverSock_it server = serverSockets.begin();
	
    // server->first  == serverSocket
	// server->second == serverConfiguration = servers_it
   
	for(; server != serverSockets.end(); server++) {

		if (FD_ISSET(server->first, &tempReadfds)) {

			/*** Accept the incoming connection. ***/
			int newSocket = accept(server->first, NULL, NULL);
			if (newSocket == -1) {
				std::cerr << "Error accepting connection--> ";
				perror(strerror(errno));
				return -1;
			}
			fcntl(newSocket, F_SETFL, O_NONBLOCK);

			/*** Create new Client. ***/
			std::cout << "newClient" << std::endl;
            HttpClient* newClient = new HttpClient((server->second), newSocket);
			
			addNewClient(newClient);
			

			/*** Add the new Client Socket to `readfds` and update the max-File-Descriptor. ***/
			setNewFD(newSocket);

			std::cout << "New connection established. Client socket: " << newClient->getSocket()<<  std::endl;
		}
	}
	return 0;
}

void	HttpServer::checkForReading(fd_set &tempReadfds) {
	for (client_it client = _clients.begin(); client != _clients.end(); ++client) {
		int	clientSocket = (*client)->getSocket();
		if (FD_ISSET(clientSocket, &tempReadfds)) {
			/*** Handle data received from client. ***/
			std::cout << "id:" << (*client)->getSocket() << std::endl;
			char buffer[MAX_BUFFER_SIZE];
			int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (std::string(buffer).find("0\r\n\r\n")!=std::string::npos){
                FD_CLR(clientSocket, &readfds);
                FD_SET(clientSocket, &writefds);
            }
			if (bytesRead == -1) {
				std::cerr << "Error receiving data from client [" << clientSocket << "] --> " << strerror(errno) << std::endl;
                close(clientSocket);
                FD_CLR(clientSocket, &readfds);
                FD_CLR(clientSocket, &writefds);
			}
			else if (bytesRead == 0) {
				// client->req.setRequestEnd(true);
				std::cout << "``````````````Client disconnected. Client socket: " << clientSocket <<"``````````````"<< std::endl;
                close(clientSocket);
				FD_CLR(clientSocket, &readfds);
				FD_SET(clientSocket, &writefds);
			}
			else {
				/*** Process the received data. ***/
				buffer[bytesRead] = '\0';
				// for (int i= 0 ; buffer[i] != '\0' ; i++){
				// 	if (buffer[i] == '\r')
				// 		std::cout << "\\r";
				// 	else if (buffer[i] == '\n')
				// 		std::cout << "\\n";
				// 	else
				// 		std::cout << buffer[i];
	
				// }
				if ((*client)->req.parseRequest(buffer)){
					servers_it serverConf= (*client)->getConfiguration();
					(*client)->processRequest(serverConf);
					(*client)->setRequestComplete(true);
					(*client)->req.closeFile();
					FD_CLR(clientSocket, &readfds);
					FD_SET(clientSocket, &writefds);
				}
				
				/*** Add the client socket to the write file descriptor set for response. ***/
			}
		}
			
	}
}

// void	HttpServer::checkForWriting(fd_set &tempWritefds) {
// 	client_it client = _clients.begin();

// 	for (size_t i = 0; client != _clients.end(); client++, i++) {
// 		int	clientSocket = (*client)->getSocket();
// 		if ((*client)->isRequestComplete()){
// 			if (FD_ISSET(clientSocket, &tempWritefds)) {

// 				/*** Prepare and send a response to the client. ***/
// 				std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";

// 					int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
// 					if (bytesSent == -1) {
// 						std::cerr << "Error sending response to client: " << strerror(errno) << std::endl;
//         	        	close(clientSocket);
//         	        	FD_CLR(clientSocket, &readfds);
//         	        	FD_CLR(clientSocket, &writefds);
// 					}
// 					else {
// 						std::cout << "Response sent to client socket " << clientSocket << std::endl;
// 						/*** Remove the client and clear his socket from the fd-sets. ***/
//         	        	close(clientSocket);
//         	        	FD_CLR(clientSocket, &readfds);
//         	        	FD_CLR(clientSocket, &writefds);
// 			    			std::cout << "----------------------------> size Before: " << _clients.size() << std::endl;
// 			    			std::cout << "----------------------------> size After: " << _clients.size() << std::endl;
//         	        	if (i+1 == _clients.size()) break;
// 					}

// 			}
// 		}
// 	}
// }
void HttpServer::checkForWriting(fd_set &tempWritefds) {
    client_it client = _clients.begin();
    while (client != _clients.end()) {
        int clientSocket = (*client)->getSocket();
        if ((*client)->isRequestComplete() && FD_ISSET(clientSocket, &tempWritefds)) {

            // Prepare and send a response to the client.
            std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
			int bytesSent;
			if ((*client)->res.getBody().empty()){
    			bytesSent = (*client)->sendFileResponse((*client)->res,(*client)->getSocket());
  			}else{
    			std::cout << "normal sendResponse" << std::endl;
    			bytesSent = (*client)->sendResponse();
  			}
            if (bytesSent == -1) {
                std::cerr << "Error sending response to client: " << strerror(errno) << std::endl;
                close(clientSocket);
                FD_CLR(clientSocket, &readfds);
                FD_CLR(clientSocket, &writefds);
            } else {
                // Remove the client and clear its socket from the fd-sets.
				if ((*client)->isRespondComplete()){
                std::cout << "Response sent to client socket and delete " << clientSocket << std::endl;
                close(clientSocket);
                FD_CLR(clientSocket, &readfds);
                FD_CLR(clientSocket, &writefds);
                
                // Remove the client from the list.
				std::cout << "client " << (*client)->getSocket() << " remove from the list" << std::endl;
                client = _clients.erase(client);
                
                continue;
				}
            }
        }
        ++client;
    }
}



// void HttpServer::checkForWriting(fd_set &tempWritefds) {
//     client_it client = _clients.begin();
//     while (client != _clients.end()) {
//         int clientSocket = (*client)->getSocket();
//         if ((*client)->isRequestComplete() && FD_ISSET(clientSocket, &tempWritefds)) {

//             // Prepare and send a response to the client.
//             std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
			
// 			if ((*client)->res.getBody().empty()){
//     			(*client)->sendFileResponse((*client)->res,(*client)->getSocket());
//   			}else{
//     			std::cout << "normal sendResponse" << std::endl;
//     			(*client)->sendResponse();
//   			}
//             int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
//             if (bytesSent == -1) {
//                 std::cerr << "Error sending response to client: " << strerror(errno) << std::endl;
//                 close(clientSocket);
//                 FD_CLR(clientSocket, &readfds);
//                 FD_CLR(clientSocket, &writefds);
//             } else {
//                 std::cout << "Response sent to client socket " << clientSocket << std::endl;
//                 // Remove the client and clear its socket from the fd-sets.
//                 close(clientSocket);
//                 FD_CLR(clientSocket, &readfds);
//                 FD_CLR(clientSocket, &writefds);
                
//                 // Remove the client from the list.
// 				std::cout << "client " << (*client)->getSocket() << " remove from the list" << std::endl;
//                 client = _clients.erase(client);
                
//                 continue;
//             }
//         }
//         ++client;
//     }
// }
void	HttpServer::closeServerSockets() {
	serverSock_it	serverSocket = serverSockets.begin();

	// serverSocket->first  == serverSocket

	for (; serverSocket != serverSockets.end(); serverSocket++) {
		close(serverSocket->first);
	}
}


/************************************************************************/
/************************* Getters and Setters **************************/
/************************************************************************/

void	HttpServer::setNewFD(int newSocket) {

	/*** Add the socket to the sets of file descriptors. ***/
	FD_SET(newSocket, &readfds);

	/*** Set the maximum file descriptor value. ***/
	maxFileDescriptor = std::max(maxFileDescriptor, newSocket);
}

serverSock_t&   HttpServer::getServerSockets() {return serverSockets;}
clients_t&	    HttpServer::getClients() {return _clients;}
int			    HttpServer::getMaxFileDescriptor() {return maxFileDescriptor;}


/************************************************************************/
/************************** Helpers and Tools ***************************/
/************************************************************************/
void HttpServer::addNewClient(HttpClient* client){
	_clients.push_back(client);
}

void    HttpServer::removeClient(client_it& client) {
    _clients.erase(client);
}