#include "networking.h"

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
int server_setup() {
  //setup structs for getaddrinfo
  struct addrinfo hints, *results;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;       // ipv4
  hints.ai_socktype = SOCK_STREAM; // tcp
  hints.ai_flags = AI_PASSIVE;     // local IP

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

void err(int i, char*message){
  if(i < 0){
	  printf("Error: %s - %s\n",message, strerror(errno));
  	exit(1);
  }
}


int main() {
    //stored an array of ints (the socket IDs)
    int shmid = shmget(SHM_KEY, sizeof(int) * MAX_CLIENTS, IPC_CREAT | 0644);
    int *client_sockets = shmat(shmid, NULL, 0);

    // Initialize all slots to -1 (empty)
    for(int i=0; i<MAX_CLIENTS; i++) client_sockets[i] = -1;

    int listen_socket = server_setup();

    while (1) {
        int client_socket = server_tcp_handshake(listen_socket);

        // Find an empty spot in shared memory and save this socket
        int slot = -1;
        for(int i=0; i<MAX_CLIENTS; i++) {
            if (client_sockets[i] == -1) {
                client_sockets[i] = client_socket;
                slot = i;
                break;
            }
        }

        if (fork() == 0) {
            // CHILD PROCESS
            close(listen_socket);
            struct message msg;

            while (read(client_socket, &msg, sizeof(struct message)) > 0) {
                // BROADCAST LOGIC
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    int target_socket = client_sockets[i];

                    // The "IF" statement that answers your question:
                    // 1. Check if the slot is active (!= -1)
                    // 2. Check if the target is NOT the current sender
                    if (target_socket != -1 && target_socket != client_socket) {
                        write(target_socket, &msg, sizeof(struct message));
                    }
                }
            }

            // Cleanup on disconnect
            client_sockets[slot] = -1;
            close(client_socket);
            exit(0);
        }
    }
}
