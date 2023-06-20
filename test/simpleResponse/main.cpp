#include <iostream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpMethodProcessor.hpp"
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include "HttpClient.hpp"
#include "confAST.hpp"
#include <filesystem>


int serverSetup(int port){
  int server_fd;
  struct sockaddr_in server;

  if ((server_fd = socket(AF_INET, SOCK_STREAM,0)) < 0){
    perror("Error: socket(): ");
    exit(1);
  }
  //bind the socket to the server adddress.
  server.sin_family = AF_INET;
  server.sin_port = htons(port);// 3000 = port
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (struct sockaddr * ) &server, sizeof(server)) < 0){
    perror("Error: bind()");
    exit(2);
  }

  if (listen(server_fd, 1) != 0){
    perror("Error: listen()");
    exit(3);
  }
  return (server_fd);
}

int accept_new_connection(int server, struct sockaddr_in  *addr){
  int new_socket;
  std::cout << "new connection ->>>" << std::endl;
  if ((new_socket = accept(server, (struct sockaddr*) addr,(socklen_t*) addr)) < 0 ){
    perror("Error: accept()");
    exit(4);
  }
  return (new_socket);
}

void handle_connection(int socket, servers_it& serverConf){
  HttpClient client(socket);
  std::cout << "---------reading start for here----------" << std::endl;
  int readByte;
  readByte = -1;
  while(true){
  char buffer[1024];
  if ((readByte = recv(socket, buffer, sizeof(buffer), 0)) == -1){
    perror("Rec()");
    exit(5);
  }
  buffer[1023] = '\0';
  client.req.parser(buffer);
  client.req.printRaw();
  std::cout << "----------end__________" << std::endl;
  if (std::string(buffer).find("0\r\n\r\n") != std::string::npos){
    std::cout << "-----------" << std::endl;
    break;
  }
  }
  std::cout << "-----end of the request---" << std::endl;
  client.processRequest(serverConf);
  
  std::cout << "-------------------end of response------------" << std::endl;
  close(socket);

}

int main(){
  try{
    Config config("/Users/aboudarg/project/webServer_2/test/simpleResponse/file.conf");

    servers_t servers = config.getServers();
    servers_it it = servers.begin();
    values_t ports = it->getPorts();
    values_it Pit = ports.begin();
  
  
  int server, max_fd = -1;
  fd_set ready_sockets, current_sockets;
  struct sockaddr_in server_addr;

  server = serverSetup(3000);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(3000);// 3000 = port // now ??? random
  server_addr.sin_addr.s_addr = INADDR_ANY;

  max_fd = server;

  FD_ZERO(&current_sockets);
  FD_SET(server, &current_sockets);
  int ready_dec;
  while(true){
    ready_sockets = current_sockets;
 
    ready_dec = select(max_fd + 1, &ready_sockets, NULL , NULL, NULL);
    for (int i = 0 ; i <= max_fd && ready_dec > 0 ; i++){
      if (FD_ISSET(i, &ready_sockets)){
	ready_dec -= 1;
	if (i == server){
	  int new_socket = accept_new_connection(server, &server_addr);
	  FD_SET(new_socket, &current_sockets);
	  if (new_socket > max_fd)
	    max_fd = new_socket;
	}else{
	    handle_connection(i, it);
	    close(i);
	    FD_CLR(i, &current_sockets);

	 }
			
	}
      }
    }

  close(server);

  }catch(std::exception &e){
    std::cout << "err: "<< e.what() << "\n";
  }

  std::cout << "hello world" << std::endl;
  return (0);
}
