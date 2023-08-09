#include "HttpClient.hpp"
#include "HttpTypes.hpp"
#include "confTypes.hpp"

HttpServer::HttpServer() {}

HttpServer::HttpServer(const HttpServer &httpServer)
{
	*this = httpServer;
}

HttpServer &HttpServer::operator=(const HttpServer &httpServer)
{
	(void)(httpServer);
	return *this;
}

HttpServer::~HttpServer()
{
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	for (clients_t::iterator client = _clients.begin(); client != _clients.end(); ++client)
	{
		delete *client;
	}
	_clients.clear();
}

/************************************************************************/
/********************** Setting up the HttpServer ***********************/
/************************************************************************/

HttpServer::HttpServer(servers_t &servers) : serversNotConnected(0), maxFileDescriptor(0)
{

	/*** Initializes the file descriptor sets to contain no file descriptors. ***/
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	servers_it server = servers.begin();
	values_t ports;
	for (; server != servers.end(); server++)
	{
		ports = server->getPorts();
		values_it itPorts = ports.begin();
		for (; itPorts != ports.end(); itPorts++)
		{
			int serverSocket = createNewSocket();
			setupServers(server, serverSocket, *itPorts);
			serverSockets.insert(serverConf(serverSocket, server));

			/*** Set the max-File-Descriptor. ***/
			maxFileDescriptor = std::max(maxFileDescriptor, serverSocket);
		}
	}
    if (serversNotConnected == servers.size()) {
        throw ServersNotConnected();
    }
}

int HttpServer::createNewSocket()
{

	/*** Create a socket for the server ***/
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(serverSocket, F_SETFL, O_NONBLOCK);
	if (serverSocket == -1)
	{
		std::cerr << "Error creating socket" << std::endl;
		throw HttpError(*this, errno);
        std::cout << strerror(errno) << std::endl;
	}

	/*** Allows local addresses that are already in use to be bound ***/
	int yes = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		std::cerr << "Error set socket option [" << serverSocket << "] : ";
		throw HttpError(*this, errno);
        std::cout << strerror(errno) << std::endl;
	}
	return (serverSocket);
}


int   HttpServer::biding_socket(servers_it &server, int &socket, const char* port) {
    int             statCode;
    struct addrinfo hints;
    struct addrinfo *result;
    struct addrinfo *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_protocol = 0;

    statCode = getaddrinfo(server->getHost().c_str(), port, &hints, &result);
    if (statCode == 0 && bind(socket, result->ai_addr, result->ai_addrlen) == 0) {
        return (0); /* Success */
    }
    values_t serverNames = server->getServerNames();
    for (values_it serverName = serverNames.begin(); serverName != serverNames.end(); serverName++) {
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_protocol = 0;
        statCode = getaddrinfo(serverName->c_str(), port, &hints, &result);
        if (statCode != 0) {
            continue;
        }
        /*** Try each address until we successfully bind the socket. ***/
        for (res = result; res != NULL; res = res->ai_next) {
            if (bind(socket, res->ai_addr, res->ai_addrlen) == 0) {
                return (0); /* Success */
            }
        }
    }
    freeaddrinfo(result);
    return (1);
}


void HttpServer::setupServers(servers_it &server, int serverSocket, const std::string &port)
{
    /*** Checking the Address structures to find the best possible match. ***/
    /******* Try each address until the socket is successfully bound. ********/
    if (biding_socket(server, serverSocket, port.c_str())) {
        std::cerr << "Error binding socket ["<<server->getHost().c_str()<<":"<<port.c_str()<<"] : ";
        close(serverSocket);
        serversNotConnected++;
        std::cout << strerror(errno) << std::endl;
    }

    /*** Listen for incoming connections. ***/
    if (listen(serverSocket, 1) == -1)
    {
        std::cerr << "Error listening on socket : ";
        close(serverSocket);
        std::cout << strerror(errno) << std::endl;
    }

    // Add the server socket to the read fd set `readfds`.
    FD_SET(serverSocket, &readfds);

    std::cout << "Start listening on [" << serverSocket << "] ---- [" << server->getHost().c_str() << ":" << port.c_str() << "]\n";
}

/************************************************************************/
/*********************** Starting the HttpServer ************************/
/************************************************************************/

void HttpServer::start()
{
	/*** Start the server ***/

	while (true)
	{
		fd_set tempReadfds = readfds;
		fd_set tempWritefds = writefds;

		/*** Wait for any activity on the file descriptors. ***/
		if (waitingForActivity(tempReadfds, tempWritefds))
		{
			// std::cout << "Connection done\n";

			/*** Check for activity on the server socket. ***/
			if (acceptIncomingConnection(tempReadfds) == -1)
				continue;
			// std::cout << "Incoming Connection Accepted.\n";

			/*** Check for activity on client sockets and read from it. ***/
			checkForReading(tempReadfds);
			/*** Check for activity on client sockets for writing response. ***/
			checkForWriting(tempWritefds);
			// std::cout << "dora" << std::endl;
		}
	}
}

bool HttpServer::waitingForActivity(fd_set &tempReadfds, fd_set &tempWritefds)
{
	int activity = select(maxFileDescriptor + 1, &tempReadfds, &tempWritefds, NULL, NULL);
	if (activity == -1)
	{
		std::cerr << "Error select : \n ";
		throw HttpError(*this, errno);
        std::cout << strerror(errno) << std::endl;
	}
	return activity;
}

int HttpServer::acceptIncomingConnection(fd_set &tempReadfds)
{
	serverSock_it server = serverSockets.begin();

	// server->first  == serverSocket
	// server->second == serverConfiguration = servers_it

	for (; server != serverSockets.end(); server++)
	{

		if (FD_ISSET(server->first, &tempReadfds))
		{

			/*** Accept the incoming connection. ***/
			int newSocket = accept(server->first, NULL, NULL);
			if (newSocket == -1)
			{
				std::cerr << "Error accepting connection--> ";
				perror(strerror(errno));
				return -1;
			}
			fcntl(newSocket, F_SETFL, O_NONBLOCK);

			/*** Create new Client. ***/
			std::cout << "newClient" << std::endl;
			HttpClient *newClient = new HttpClient((server->second), newSocket);

			addNewClient(newClient);

			/*** Add the new Client Socket to `readfds` and update the max-File-Descriptor. ***/
			setNewFD(newSocket);

			std::cout << "New connection established. Client socket: " << newClient->getSocket() << std::endl;
		}
	}
	return 0;
}

void HttpServer::checkForReading(fd_set &tempReadfds)
{
	for (client_it client = _clients.begin(); client != _clients.end(); ++client)
	{
		int clientSocket = (*client)->getSocket();
		if (FD_ISSET(clientSocket, &tempReadfds))
		{
			/*** Handle data received from client. ***/
			std::cout << "id:" << (*client)->getSocket() << std::endl;
			char buffer[MAX_BUFFER_SIZE];
			int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
			if (std::string(buffer).find("0\r\n\r\n") != std::string::npos)
            {
				FD_CLR(clientSocket, &readfds);
				FD_SET(clientSocket, &writefds);
			}
			if (bytesRead == -1)
			{
				std::cerr << "Error receiving data from client [" << clientSocket << "] --> " << strerror(errno) << std::endl;
				close(clientSocket);
				FD_CLR(clientSocket, &readfds);
				FD_CLR(clientSocket, &writefds);
                (*client)->setClientInteraction(false);
			}
			else if (bytesRead == 0)
			{
				// client->req.setRequestEnd(true);
				std::cout << "``````````````Client disconnected. Client socket: " << clientSocket << "``````````````" << std::endl;
				close(clientSocket);
				FD_CLR(clientSocket, &readfds);
				FD_SET(clientSocket, &writefds);
                (*client)->setClientInteraction(false);
			}
			else
			{
				/*** Process the received data. ***/
				buffer[bytesRead] = '\0';
				servers_it serverConf = (*client)->getConfiguration();
				int statCode = (*client)->req.parseRequest(buffer, serverConf);
                if (statCode == 0) {
					(*client)->processRequest(serverConf);
				}
                else {
                    createErrorPageResponse(serverConf, statCode, *(*client));
                }
                (*client)->setRequestComplete(true);
                (*client)->req.closeFile();
                FD_CLR(clientSocket, &readfds);
                FD_SET(clientSocket, &writefds);
				/*** Add the client socket to the write file descriptor set for response. ***/
			}
		}
	}
}

void HttpServer::checkForWriting(fd_set &tempWritefds)
{
	client_it client = _clients.begin();
	while (client != _clients.end())
	{
		int clientSocket = (*client)->getSocket();
		if ((*client)->isRequestComplete() && FD_ISSET(clientSocket, &tempWritefds))
		{
			int tmpint = (*client)->res.getProccessPID();
			// std::cout << "the pid of tmpint" << tmpint << std::endl;
			if (tmpint == -1)
			{
				// Prepare and send a response to the client.
				int bytesSent;
				// the check change to filename because body sometime empty but not a file
				if (!(*client)->res.getFilename().empty()) 
				{
					bytesSent = (*client)->sendFileResponse((*client)->res, (*client)->getSocket());
				}
				else
				{
					std::cout << "normal sendResponse" << std::endl;
					bytesSent = (*client)->sendResponse();
				}

				if (bytesSent == -1)
				{
					std::cerr << "Error sending response to client: " << strerror(errno) << std::endl;
					close(clientSocket);
					FD_CLR(clientSocket, &readfds);
					FD_CLR(clientSocket, &writefds);
				}
				else
				{
					// Remove the client and clear its socket from the fd-sets.
					if ((*client)->isRespondComplete())
					{

						std::cout << "Response sent to client socket and delete " << clientSocket << std::endl;
						std::cout << "Going to delete: " << (*client)->res.getCGIFile().second << "\n";
						(*client)->clean();
						close(clientSocket);
						FD_CLR(clientSocket, &readfds);
						FD_CLR(clientSocket, &writefds);

						// Remove the client from the list.
						// std::cout << "client " << (*client)->getSocket() << " remove from the list" << std::endl;
						HttpClient *temp = *client;
						client = _clients.erase(client);
						delete temp;
						continue;
					}
				}
			}
			else
			{
				servers_it serverConf = (*client)->getConfiguration();
				(*client)->processRequest(serverConf);
				std::cout << "PID NOW IS : " << (*client)->res.getProccessPID() << "\n";
			}
		}
		++client;
	}
}

void HttpServer::closeServerSockets()
{
	serverSock_it serverSocket = serverSockets.begin();

	// serverSocket->first  == serverSocket

	for (; serverSocket != serverSockets.end(); serverSocket++)
	{
		close(serverSocket->first);
	}
}

/************************************************************************/
/************************* Getters and Setters **************************/
/************************************************************************/

void HttpServer::setNewFD(int newSocket)
{
	/*** Add the socket to the sets of file descriptors. ***/
	FD_SET(newSocket, &readfds);

	/*** Set the maximum file descriptor value. ***/
	maxFileDescriptor = std::max(maxFileDescriptor, newSocket);
}

serverSock_t &HttpServer::getServerSockets() { return serverSockets; }
clients_t &HttpServer::getClients() { return _clients; }
int HttpServer::getMaxFileDescriptor() { return maxFileDescriptor; }

/************************************************************************/
/************************** Helpers and Tools ***************************/
/************************************************************************/

void HttpServer::addNewClient(HttpClient *client)
{
	_clients.push_back(client);
}

void HttpServer::removeClient(client_it &client)
{
	_clients.erase(client);
}