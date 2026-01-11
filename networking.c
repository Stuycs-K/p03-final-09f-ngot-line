#include "networking.h"

void err(int i, char*message){
  if(i < 0){
	  printf("Error: %s - %s\n",message, strerror(errno));
  	exit(1);
  }
}

//connects to server and returns the socket
int client_tcp_handshake(char * server_address) {
    struct addrinfo hints, *results;

    //hints for getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP


      // PORT is defined in networking.h
    int status = getaddrinfo(server_address, PORT, &hints, &results);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // made socket using the info from getaddres
    int server_socket = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    err(server_socket, "socket error");

    // connects to serve
    // start of tcp hs
    int c = connect(server_socket, results->ai_addr, results->ai_addrlen);
    err(c, "connect error");


    freeaddrinfo(results);

    return server_socket;
}

// accepts a connection and returns the new client socket
int server_tcp_handshake(int listen_socket) {
    int client_socket = accept(listen_socket, NULL, NULL);
    err(client_socket, "accept error");
    return client_socket;
}



int server_setup() {
  //setup structs for getaddrinfo
  struct addrinfo hints, *results;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;       // ipv4
  hints.ai_socktype = SOCK_STREAM; // tcp
  hints.ai_flags = AI_PASSIVE;     // so we use the local IP

  int status = getaddrinfo(NULL, PORT, &hints, &results);
  err(status, "getaddrinfo error");

  int listen_socket = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
  err(listen_socket, "socket error");

  int yes = 1;
  setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  int b = bind(listen_socket, results->ai_addr, results->ai_addrlen);
  err(b, "bind error");

  int l = listen(listen_socket, 10);
  err(l, "listen error");

  freeaddrinfo(results);
  return listen_socket;

}
