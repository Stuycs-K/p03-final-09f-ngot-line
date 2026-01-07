#include "networking.h"


int server_setup() {
  struct addrinfo * hints, * results;
  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_flags = AI_PASSIVE;
  getaddrinfo(NULL, PORT , hints, &results);

  int clientd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

  int yes = 1;
  int sockOpt =  setsockopt(clientd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  err(sockOpt,"sockopt  error");

  bind( clientd, results->ai_addr, results->ai_addrlen);

  free(hints);
  freeaddrinfo(results);

  return clientd;
}

int server_tcp_handshake(int listen_socket){
  int client_socket;


  return client_socket;
}

int client_tcp_handshake(char * server_address) {
  struct addrinfo * hints, * results;
  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;

  int serverd = getaddrinfo(NULL, PORT , &hints, &results);

  free(hints);
  freeaddrinfo(results);
  return serverd;
}

void err(int i, char*message){
  if(i < 0){
	  printf("Error: %s - %s\n",message, strerror(errno));
  	exit(1);
  }
}
