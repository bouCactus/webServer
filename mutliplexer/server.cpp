#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>


int serverSetup(int port){
  int server_fd;
  struct sockaddr_in server;
  char buffer[1024];

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
  std::cout << "he run from here ->>>" << std::endl;
  if ((new_socket = accept(server, (struct sockaddr*) addr,(socklen_t*) addr)) < 0 ){
    perror("Error: accept()");
    exit(4);
  }
  return (new_socket);
}

void handle_connection(int socket){
  char buffer[1024];
  if (recv(socket, buffer, sizeof(buffer), 0) == -1){
    perror("Rec()");
    exit(5);
  }
  if (send(socket, "404 not found", 13, 0) < 0 ){
    perror("Send()");
    exit(6);
  }
  close(socket);
  std::cout << buffer << std::endl;
}
int main(){
  int server[2];
  server[0] = serverSetup(3000);
  server[1] = serverSetup(80);
  struct sockaddr_in server_addr[2];
  
  server_addr[0].sin_family = AF_INET;
  server_addr[0].sin_port = htons(3000);// 3000 = port
  server_addr[0].sin_addr.s_addr = INADDR_ANY;

  server_addr[1].sin_family = AF_INET;
  server_addr[1].sin_port = htons(3000);// 3000 = port
  server_addr[1].sin_addr.s_addr = INADDR_ANY;
  fd_set ready_sockets, current_sockets;

  FD_ZERO(&current_sockets);
  FD_SET(server[0], &current_sockets);
  FD_SET(server[1], &current_sockets);
  
  while(true){
    ready_sockets = current_sockets;

    select(FD_SETSIZE, &ready_sockets, NULL , NULL, NULL);
    for (int i = 0 ; i < FD_SETSIZE ; i++){
      if (FD_ISSET(i, &ready_sockets)){
	for(int k = 0; k < 2 ; k++){
	  if (i == server[k]){
	    int client_socket = accept_new_connection(server[k], &server_addr[k]);
	    FD_SET(client_socket, &current_sockets);
	  }
	  else{
	    handle_connection(i);
	    FD_CLR(i, &current_sockets);
	  }
	}
      }
    }

  }
  close(server[0]);
  close(server[1]);
  std::cout << "hello world" << std::endl;
  return (0);
}


/*
  int main(){

  int server_fd, new_socket;
  struct sockaddr_in server;
  char buffer[1024];

  if ((server_fd = socket(AF_INET, SOCK_STREAM,0)) < 0){
    perror("Error: socket(): ");
    exit(1);
  }

  //bind the socket to the server adddress.
  server.sin_family = AF_INET;
  server.sin_port = htons(3000);// 3000 = port
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (struct sockaddr * ) &server, sizeof(server)) < 0){
    perror("Error: bind()");
    exit(2);
  }

  if (listen(server_fd, 1) != 0){
    perror("Error: listen()");
    exit(3);
  }
  while(true){
    if ((new_socket = accept(server_fd, (struct sockaddr*) &server,(socklen_t*) &server)) < 0 ){
      perror("Error: accept()");
      exit(4);
    }

  if (recv(new_socket, buffer, sizeof(buffer), 0) == -1){
    perror("Rec()");
      exit(5);
  }
  if (send(new_socket, "404 not found", 13, 0) < 0 ){
    perror("Send()");
    exit(6);
  }
  close(new_socket);
  std::cout << buffer << std::endl;
  }
  close(server_fd);
  std::cout << "hello world" << std::endl;
  return (0);
}

*/
