
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>

const int	MAX_BUFFER_SIZE = 1024;
int			maxFileDescriptor;

int main() {

	// Create a socket for the server.
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(serverSocket, F_SETFL, O_NONBLOCK);
	if (serverSocket == -1) {
		std::cerr << "Error creating socket" << std::endl;
		return -1;
	}

	// Allows local addresses that are already in use to be bound.
	int yes = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		std::cerr << "Error creating socket" << std::endl;
	    exit(1);
	}

	// Set up server address.
	sockaddr_in serverAddress;
	std::memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = inet_addr("0.0.0.0");

	// Bind the socket to the server address.
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "Error binding socket : " << strerror(errno) << std::endl;
		close(serverSocket);
		return -1;
	}

	// Listen for incoming connections.
	if (listen(serverSocket, 1) == -1) {
		std::cerr << "Error listening on socket" << std::endl;
		close(serverSocket);
		return -1;
	}

	// Find the maximum file descriptor value
	int maxFileDescriptor = serverSocket;

	std::cout << "Server listening on port 8080" << std::endl;

	// Create sets of file descriptors for select().
	fd_set readfds, writefds;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_SET(serverSocket, &readfds);

	// Initiate the maxFileDescriptor
	maxFileDescriptor = serverSocket;

	// Start the server
	while (true) {
		// Copy the file descriptor sets to temporary sets.
		fd_set tempReadfds = readfds;
		fd_set tempWritefds = writefds;

		// Wait for any activity on the file descriptors.
		int activity = select(maxFileDescriptor + 1, &tempReadfds, &tempWritefds, NULL, &tv);
		if (activity == -1) {
			std::cerr << "Error in select()" << std::endl;
			close(serverSocket);
			return -1;
		}
        if (activity) {
		// Check for activity on the server socket.
		if (FD_ISSET(serverSocket, &tempReadfds)) {
			// Accept the incoming connection.
			int clientSocket = accept(serverSocket, NULL, NULL);
			fcntl(clientSocket, F_SETFL, O_NONBLOCK);
			if (clientSocket == -1) {
				std::cerr << "Error accepting connection" << std::endl;
				close(serverSocket);
				return -1;
			}
			// Update the maxFileDescriptor.
			maxFileDescriptor = std::max(clientSocket, maxFileDescriptor);

			// Add the new client socket to the read file descriptor set.
			FD_SET(clientSocket, &readfds);
			
			std::cout << "New connection established. Client socket: " << clientSocket << std::endl;
		}

		// Check for activity on client sockets.
		for (int clientSocket = 0; clientSocket <= maxFileDescriptor; ++clientSocket) {
			if (FD_ISSET(clientSocket, &tempReadfds)) {
				// Handle data received from client.
				char buffer[MAX_BUFFER_SIZE];
				int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
				if (bytesRead == -1 && clientSocket!=serverSocket) {
					std::cerr << "Error receiving data from client" << std::endl;
					close(clientSocket);
					FD_CLR(clientSocket, &readfds);
					FD_CLR(clientSocket, &writefds);
				} else if (bytesRead == 0) {
					std::cout << "Client disconnected. Client socket: " << clientSocket << std::endl;
					close(clientSocket);
					FD_CLR(clientSocket, &readfds);
					FD_CLR(clientSocket, &writefds);
				} else {
					// Process the received data.
					buffer[bytesRead] = '\0';
					std::cout << "Data received from client socket " << clientSocket << ": \n" << buffer << std::endl;
					
					// Add the client socket to the write file descriptor set for response.
					FD_SET(clientSocket, &writefds);
				}
			}
		}

		// Check for activity on client sockets for writing response.
		for (int clientSocket = 0; clientSocket <= maxFileDescriptor; ++clientSocket) {
			if (FD_ISSET(clientSocket, &tempWritefds)) {
				// Prepare and send a response to the client.
				std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
				int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
				if (bytesSent == -1) {
					std::cerr << "Error sending response to client" << std::endl;
					close(clientSocket);
					FD_CLR(clientSocket, &readfds);
					FD_CLR(clientSocket, &writefds);
				} else {
					std::cout << "Response sent to client socket " << clientSocket << std::endl;
					// Remove the client socket from the write file descriptor set.
					FD_CLR(clientSocket, &writefds);
				}
			}
		}
        }
	}

	// Close the server socket.
	close(serverSocket);

	return 0;
}


// #include "HttpTypes.hpp"

// int	main(int ac, char** av) {
// 	if (ac != 2) {
// 		std::cerr << "error: Invalide number of arguments." << std::endl;
// 		exit(1);
// 	}
// 	try {
// 		/*** Parsing the configuration file ***/ 
// 		Config conf(av[1]);
		
// 		/*** Multiplexing ***/
// 		servers_t	servers = conf.getServers();
// 		HttpServer	servers(servers);
// 		// ...

// 	} catch(std::exception &e)
// 	{
// 		// WHATEVER!!
// 		perror(e.what());
// 	}
// 	return  (0);
// }

// int main(int argc, char *argv[])
// {
//     struct addrinfo hints, *res, *p;
//     int status;
//     char ipstr[INET6_ADDRSTRLEN];

//     if (argc != 2) {
//         fprintf(stderr,"usage: showip hostname\n");
//         return 1;
//     }

//     memset(&hints, 0, sizeof hints);
//     hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
//     hints.ai_socktype = SOCK_STREAM;

//     if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
//         fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
//         return 2;
//     }

//     printf("IP addresses for %s:\n\n", argv[1]);

//     for(p = res;p != NULL; p = p->ai_next) {
//         void *addr;
//         char *ipver;

//         // get the pointer to the address itself,
//         // different fields in IPv4 and IPv6:
//         if (p->ai_family == AF_INET) { // IPv4
//             struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
//             addr = &(ipv4->sin_addr);
//             ipver = "IPv4";
//         } else { // IPv6
//             struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
//             addr = &(ipv6->sin6_addr);
//             ipver = "IPv6";
//         }

//         // convert the IP to a string and print it:
//         inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
//         printf("  %s: %s\n", ipver, ipstr);
//     }

//     freeaddrinfo(res); // free the linked list

//     return 0;
// }