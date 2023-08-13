#include "HttpClient.hpp"
#include "HttpTypes.hpp"
#include "confTypes.hpp"
#include <signal.h>
#include <sys/signal.h>

HttpServer::HttpServer() {}

HttpServer::HttpServer(const HttpServer &httpServer) { *this = httpServer; }

HttpServer &HttpServer::operator=(const HttpServer &httpServer)
{
  (void)(httpServer);
  return *this;
}

HttpServer::~HttpServer()
{

  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  for (clients_t::iterator client = _clients.begin(); client != _clients.end();
       ++client)
  {
    delete *client;
  }
  _clients.clear();
}

/************************************************************************/
/********************** Setting up the HttpServer ***********************/
/************************************************************************/

HttpServer::HttpServer(servers_t &servers)
 : serversNotConnected(0), maxServerSockets(0), maxFileDescriptor(0) {

  /*** Initializes the file descriptor sets to contain no file descriptors. ***/
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);

  servers_it server = servers.begin();
  values_t ports;
  for (; server != servers.end(); server++)
  {
    ports = server->getPorts();
    values_it itPorts = ports.begin();
    for (; itPorts != ports.end(); itPorts++) {
      int serverSocket = createNewSocket(); /* A value of -1 means that creation failed. */
      if (serverSocket == -1 || setupServers(server, serverSocket, *itPorts)) {
        serversNotConnected++;
        continue;
      }
      serverSockets.insert(serverConf(serverSocket, server));
      /*** Set the max-File-Descriptor. ***/
      maxFileDescriptor = std::max(maxFileDescriptor, serverSocket);

      /*** Socket Creation logs ***/

    }
    /*** Find out the number of server sockets to be created. ***/
    maxServerSockets += server->getPorts().size();
  }

  if (serversNotConnected == maxServerSockets) {
    throw ServersNotConnected();
  }
}

int HttpServer::createNewSocket()
{

  /*** Create a socket for the server ***/
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  int flag = fcntl(serverSocket, F_GETFL, 0);
  fcntl(serverSocket, F_SETFL, flag | O_NONBLOCK);
  if (serverSocket == -1) {
    return (-1);
  }

  /*** Allows local addresses that are already in use to be bound ***/
  int yes = 1;
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1 ||
      setsockopt(serverSocket, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(yes)) == -1) {
    // std::cerr << "Error set socket option [" << serverSocket << "] : ";
    return (-1);
    // throw HttpError(*this, errno);
  }
  return (serverSocket);
}

int HttpServer::biding_socket(servers_it &server, int &socket,
                              const char *port)
{
  int statCode;
  struct addrinfo hints;
  struct addrinfo *result;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_protocol = 0;
  hints.ai_socktype = SOCK_STREAM;

  statCode = getaddrinfo(server->getHost().c_str(), port, &hints, &result);
  if (statCode == 0 && bind(socket, result->ai_addr, result->ai_addrlen) == 0)
  {
    return (0); /* Success */
  }
  values_t serverNames = server->getServerNames();
  for (values_it serverName = serverNames.begin();
       serverName != serverNames.end(); serverName++)
  {
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_protocol = 0;
    hints.ai_socktype = SOCK_STREAM;
    statCode = getaddrinfo(serverName->c_str(), port, &hints, &result);
    if (statCode != 0)
    {
      continue;
    }
    /*** Try each address until we successfully bind the socket. ***/
    for (res = result; res != NULL; res = res->ai_next)
    {
      if (bind(socket, res->ai_addr, res->ai_addrlen) == 0)
      {
        return (0); /* Success */
      }
    }
  }
  freeaddrinfo(result);
  return (1); /* Failure */
}

int HttpServer::setupServers(servers_it &server, int serverSocket,
                              const std::string &port) {
  /*** Checking the Address structures to find the best possible match. ***/
  /******* Try each address until the socket is successfully bound. ********/
  if (biding_socket(server, serverSocket, port.c_str())) {
    std::cerr << "Error binding socket   ---- [" << server->getHost().c_str() << ":"
              << port.c_str() << "]" << std::endl;
    close(serverSocket);
    return 1; /* Failure */
  }

  /*** Listen for incoming connections. ***/
  if (listen(serverSocket, SOMAXCONN) == -1) {
  std::cout << "Error listening on [" << serverSocket << "] ---- ["
            << server->getHost().c_str() << ":" << port.c_str() << "]\n";
    close(serverSocket);
    return 1; /* Failure */
  }

  // Add the server socket to the read fd set `readfds`.
  FD_SET(serverSocket, &readfds);

  std::cout << "Start listening on [" << serverSocket << "] ---- ["
            << server->getHost().c_str() << ":" << port.c_str() << "]\n";
  return (0); /* Success */
}

/************************************************************************/
/*********************** Starting the HttpServer ************************/
/************************************************************************/

void HttpServer::start(HttpServer	&httpServer) {
  /*** Start the server ***/

  while (true)
  {
    fd_set tempReadfds = readfds;
    fd_set tempWritefds = writefds;

    /*** Wait for any activity on the file descriptors. ***/
    if (waitingForActivity(tempReadfds, tempWritefds)) {
      // std::cout << "Connection done\n";

      /*** Check for activity on the server socket. ***/
      if (acceptIncomingConnection(tempReadfds) == -1)
        continue;

      /*** Check for activity on client sockets and read from it. ***/
      checkForReading(tempReadfds);
      /*** Check for activity on client sockets for writing response. ***/
      checkForWriting(tempWritefds);
    }
    else {
      /*** Restart the server in case of select() failure. ***/
      FD_ZERO(&readfds);
      serverSock_it serverSocket = httpServer.getServerSockets().begin();
      serverSock_it it_end = httpServer.getServerSockets().end();
      for (; serverSocket != it_end; ++serverSocket) {
        FD_SET(serverSocket->first, &readfds);
      }
    }
  }
}

bool HttpServer::waitingForActivity(fd_set &tempReadfds, fd_set &tempWritefds)
{
  int activity = select(maxFileDescriptor + 1, &tempReadfds, &tempWritefds, NULL, NULL);
  if (activity == -1)
  {
    std::cerr << "Error select.";
    return (-1);
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
      int flag = fcntl(newSocket, F_GETFL, 0);
      fcntl(newSocket, F_SETFL, flag | O_NONBLOCK);

      /*** Create new Client. ***/
      //   //std::cout << "newClient" << std::endl;
      HttpClient *newClient = new HttpClient((server->second), newSocket);

      addNewClient(newClient);

      /*** Add the new Client Socket to `readfds` and update the
       * max-File-Descriptor. ***/
      setNewFD(newSocket);

    }
  }
  return 0;
}

void HttpServer::checkForReading(fd_set &tempReadfds)
{
  
  for (client_it client = _clients.begin(); client != _clients.end();
       ++client)
  {
    int clientSocket = (*client)->getSocket();
    if (FD_ISSET(clientSocket, &tempReadfds))
    {

      char buffer[MAX_BUFFER_SIZE];
      int bytesRead = recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
      if (std::string(buffer).find("0\r\n\r\n") != std::string::npos) {
        FD_CLR(clientSocket, &readfds);
        FD_SET(clientSocket, &writefds);
      }

      if (bytesRead == -1)
      {
        std::cerr << "Error receiving data from client [" << clientSocket
                  << "] --> " << strerror(errno) << std::endl;
        close(clientSocket);
        FD_CLR(clientSocket, &readfds);
        FD_CLR(clientSocket, &writefds);
        (*client)->setClientInteraction(false);
        // Remove the client from the list.
        this->removeClient(client);
      }
      else if (bytesRead == 0)
      {
        close(clientSocket);
        FD_CLR(clientSocket, &readfds);
        FD_CLR(clientSocket, &writefds);
        this->removeClient(client);
      }
      else
      {
        /*** Process the received data. ***/
        servers_it serverConf = (*client)->getConfiguration();
        try {
        if ((*client)->req.parseRequest(buffer, bytesRead, serverConf)) {
          (*client)->processRequest(serverConf);
          (*client)->setRequestComplete(true);
          (*client)->req.closeFile();
          FD_CLR(clientSocket, &readfds);
          FD_SET(clientSocket, &writefds);
        }
        } catch(int statusCode) {
          //std::cout << e.what() << std::endl;
           createErrorPageResponse(serverConf, statusCode, *(*client));
          (*client)->setRequestComplete(true);
          (*client)->req.closeFile();
          FD_CLR(clientSocket, &readfds);
          FD_SET(clientSocket, &writefds);
        }

        /*** Add the client socket to the write file descriptor set for
         * response. ***/
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
    if (FD_ISSET(clientSocket, &tempWritefds) &&
        (*client)->isRequestComplete())
    {
      int tmpint = (*client)->res.getProccessPID();
      if (tmpint == -1)
      {
        // Prepare and send a response to the client.
        int bytesSent = 0;
        // the check change to filename because body sometime empty but not a
        // file
        if (!(*client)->res.getFilename().empty())
        {
          bytesSent = (*client)->sendFileResponse((*client)->res,
                                                  (*client)->getSocket());
        } else {
          bytesSent = (*client)->sendResponse();
        }

        if (bytesSent == -1 || (*client)->isRespondComplete()) {
          (*client)->clean(_clients);
          close(clientSocket);
          FD_CLR(clientSocket, &readfds);
          FD_CLR(clientSocket, &writefds);

          // Remove the client from the list.
          this->removeClient(client);
          continue;
        }
      }
      else
      {
        servers_it serverConf = (*client)->getConfiguration();
        (*client)->processRequest(serverConf);
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
  HttpClient *temp = *client;
  client = _clients.erase(client);
  delete temp;
}