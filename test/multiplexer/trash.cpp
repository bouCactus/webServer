
// for (int clientSocket = 0; clientSocket <= maxFileDescriptor; ++clientSocket) {
// 	if (FD_ISSET(clientSocket, &tempReadfds)) {
// 		if (clientSocket == serverSocket) {
// 			// Accept the incoming connection
// 			int client = accept(serverSocket, NULL, NULL);
// 			if (client == -1) {
// 				std::cerr << "Error accepting connection" << std::endl;
// 				close(serverSocket);
// 				return -1;
// 			}
// 			// Add the new client socket to the read file descriptor set
// 			FD_SET(client, &readfds);
// 			// Update the maximum file descriptor value if necessary
// 			maxFileDescriptor = std::max(maxFileDescriptor, client);
// 			std::cout << "New connection established. Client socket: " << client << std::endl;
// 		} else {
// 			// Handle data received from client
// 			char buffer[MAX_BUFFER_SIZE];
// 			int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
// 			if (bytesRead == -1) {
// 				std::cerr << "Error receiving data from client" << std::endl;
// 				close(clientSocket);
// 				FD_CLR(clientSocket, &readfds);
// 			} else if (bytesRead == 0) {
// 				std::cout << "Client disconnected. Client socket: " << clientSocket << std::endl;
// 				std::cout << "****************************************************" << std::endl;
// 				close(clientSocket);
// 				FD_CLR(clientSocket, &readfds);
// 			} else {
// 				// Process the received data
// 				buffer[bytesRead] = '\0';
// 				std::cout << "Data received from client socket " << clientSocket << ": " << buffer << std::endl;
// 				// Send a response to the client
// 				std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
// 				int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
// 				if (bytesSent == -1) {
// 					std::cerr << "Error sending response to client" << std::endl;
// 				} else {
// 					std::cout << "Response sent to client socket " << clientSocket << std::endl;
// 				}
// 			}
// 		}
// 	}
// }