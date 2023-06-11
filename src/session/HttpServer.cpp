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
	_clients.clear();
	FD_ZERO(&readfds);
}


/************************************************************************/
/********************** Setting up the HttpServer ***********************/
/************************************************************************/

HttpServer::HttpServer(servers_t& servers) : maxFileDescriptor(0) {
	servers_it server = servers.begin();
	values_t ports;	// set of strings

	for (; server != servers.end(); server++) {
		ports = server->getPorts();
		values_it itPorts = ports.begin();
		for (; itPorts != ports.end(); itPorts++) {
			int serverSocket = this->createNewSocket();
			this->setupServers(server, serverSocket, *itPorts);
			this->serverSockets.insert(serverConf(serverSocket,server));
		}
	}
}

int	HttpServer::createNewSocket() {
	int serverSocket;

	/*** Create a socket for the server ***/
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0) < 0)) {
		// std::cerr << "Error creating socket" << std::endl;
		HttpError(*this, errno);
	}
	fcntl(serverSocket, F_SETFL, O_NONBLOCK);

	/*** Allows local addresses that are already in use to be bound ***/
	int yes = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		// std::cerr << "Error creating socket" << std::endl;
		HttpError(*this, errno);
	}
	return (serverSocket);
}

void	HttpServer::setupServers(servers_it &server, int serverSocket, const std::string& port) {
	/*** Set up server address. ***/
	sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(port.c_str()));
	// serverAddr.sin_addr.s_addr = inet_addr(server->getHost());
	serverAddr.sin_addr.s_addr = INADDR_ANY;
(	void)(server);

	/*** Bind the socket to the server address. ***/
	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		// std::cerr << "Error binding socket" << std::endl;
		close(serverSocket);
		HttpError(*this, errno);
	}

	/*** Listen for incoming connections. ***/
	if (listen(serverSocket, 1) == -1) {
		// std::cerr << "Error listening on socket" << std::endl;
		close(serverSocket);
		HttpError(*this, errno);
	}
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
		waitingForActivity(tempReadfds, tempWritefds);

		/*** Check for activity on the server socket. ***/
		if (acceptIncomingConnection(tempReadfds) == -1)
			continue;

		/*** Check for activity on client sockets and read from it. ***/
		checkForReading(tempReadfds);

		/*** Check for activity on client sockets for writing response. ***/
		checkForWriting(tempReadfds);
	}
}

void	HttpServer::waitingForActivity(fd_set &tempReadfds, fd_set &tempWritefds) {
	int activity = select(this->maxFileDescriptor + 1, &tempReadfds, &tempWritefds, NULL, NULL);
	if (activity == -1) {
		// std::cerr << "Error in select()" << std::endl;
		HttpError(*this,errno);
	}
}

int	HttpServer::acceptIncomingConnection(fd_set& tempReadfds) {
	serverSock_it server;

	// server->first  == serverSocket
	// server->second == serverConfiguration

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
			this->_clients.push_back(HttpClient(server->second, server->first, newSocket));
			this->setNewFD(newSocket);

			/*** Add the new Client Socket to the read file descriptor set. ***/
			FD_SET(newSocket, &readfds);

			/*** Update the max-File-Descriptor. ***/
			maxFileDescriptor = std::max(maxFileDescriptor, newSocket);
			std::cout << "New connection established. Client socket: " << newSocket << std::endl;
		}
	}
	return 0;
}

void	HttpServer::checkForReading(fd_set &tempReadfds) {
	clients_t clients = this->getClients();
	client_it client = clients.begin();

	for (; client != clients.end(); ++client) {
		int	clientSocket = client->getSocket();
		if (FD_ISSET(clientSocket, &tempReadfds)) {

			/*** Handle data received from client. ***/
			char buffer[MAX_BUFFER_SIZE];
			int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
			if (bytesRead == -1) {
				std::cerr << "Error receiving data from client" << std::endl;
				close(clientSocket);
				FD_CLR(clientSocket, &readfds);
				FD_CLR(clientSocket, &writefds);
			}
			else if (bytesRead == 0) {
				std::cout << "Client disconnected. Client socket: " << clientSocket << std::endl;
				close(clientSocket);
				FD_CLR(clientSocket, &readfds);
				FD_CLR(clientSocket, &writefds);
			}
			else {
				/*** Process the received data. ***/
				buffer[bytesRead] = '\0';
				std::cout << "Data received from client socket " << clientSocket << ": \n" << buffer << std::endl;

				/*** Add the client socket to the write file descriptor set for response. ***/
				FD_SET(clientSocket, &writefds);
			}
		}
	}
}

void	HttpServer::checkForWriting(fd_set &tempWritefds) {
	clients_t clients = this->getClients();
	client_it client = clients.begin();

	for (; client != clients.end(); ++client) {
		int	clientSocket = client->getSocket();
		if (FD_ISSET(clientSocket, &tempWritefds)) {

			/*** Prepare and send a response to the client. ***/
			std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
			int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
			if (bytesSent == -1) {
				std::cerr << "Error sending response to client" << std::endl;
				close(clientSocket);
				FD_CLR(clientSocket, &readfds);
				FD_CLR(clientSocket, &writefds);
			}
			else {
				std::cout << "Response sent to client socket " << clientSocket << std::endl;

				/*** Remove the client socket from the write file descriptor set. ***/
				FD_CLR(clientSocket, &writefds);
			}
		}
	}
}

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

	/*** Set the maximum file descriptor value. ***/
	maxFileDescriptor = std::max(maxFileDescriptor, newSocket);

	/*** Add the socket to the sets of file descriptors. ***/
	FD_SET(newSocket, &readfds);
}

clients_t&	HttpServer::getClients() {return this->_clients;}
int			HttpServer::getMaxFileDescriptor() {return this->maxFileDescriptor;}
