#include "networking.h"

#define CLR_RESET  "\x1b[0m"
#define CLR_RED    "\x1b[31m"
#define CLR_GREEN  "\x1b[32m"
#define CLR_YELLOW "\x1b[33m"
#define CLR_BLUE   "\x1b[34m"
#define CLR_MAGENTA "\x1b[35m"
#define CLR_CYAN   "\x1b[36m"

char *colors[] = {CLR_RED, CLR_GREEN, CLR_YELLOW, CLR_BLUE, CLR_MAGENTA, CLR_CYAN};

void exit_handler(int sig) {
    printf("\n[Server] Shutting down. Closing all sockets...\n");
    // Just close the sockets in your array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (chatroom[i].socket != -1) {
            close(chatroom[i].socket);
        }
    }
    printf("[Server] Cleanup complete. see you next time :) !\n");
    exit(0);
}

int main() {
<<<<<<< HEAD
=======

  //Tell the OS: "If someone hits Ctrl+C (SIGINT), run my exit_handler"
  signal(SIGINT, exit_handler);
  signal(SIGCHLD, sighandler); //trying to fix z-processes issue

    // setups Shared Memory for the socket table
    // This allows children to see everyone's socket ID for broadcasting
    int shmid = shmget(SHM_KEY, sizeof(struct client_info) * MAX_CLIENTS, IPC_CREAT | 0644);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    global_shmid = shmid;
    chatroom_data = shmat(shmid, NULL, 0);

    // Initialize all slots
    for(int i = 0; i < MAX_CLIENTS; i++) {
        chatroom_data[i].socket = -1;
        chatroom_data[i].pid = -1;
        printf("Client: %d Updated", i);
    }

>>>>>>> 807cd8ea70614ca2e25239c59985c37662c2de26
    int listen_socket = server_setup();
    printf("[server] Multi-user server started  on port %s...\n", PORT);

    // array of all connected client sockets
    int client_sockets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) client_sockets[i] = -1;

    fd_set read_fds;
    char buffer[sizeof(struct message)];

<<<<<<< HEAD
    while (1) {
        // Clear and setup the fd_set
        FD_ZERO(&read_fds);
        FD_SET(listen_socket, &read_fds); // adding the listener
        int max_fd = listen_socket;

        // add all active clients to the set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] != -1) {
                FD_SET(client_sockets[i], &read_fds);
                if (client_sockets[i] > max_fd) max_fd = client_sockets[i];
            }
        }

        //waiting for activity
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select error");
            continue;
        }

        //checks for new connections
        if (FD_ISSET(listen_socket, &read_fds)) {
            int new_socket = server_tcp_handshake(listen_socket);

            // adds new socket to the array
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == -1) {
                    client_sockets[i] = new_socket;
                    printf("[server] Added new client to slot %d (Socket %d)\n", i, new_socket);
                    break;
                }
            }
        }

        // check all clients for msgs
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int curr_fd = client_sockets[i];
            if (curr_fd != -1 && FD_ISSET(curr_fd, &read_fds)) {
                struct message msg;
                int bytes_read = read(curr_fd, &msg, sizeof(struct message));

                if (bytes_read == 0) {
                    // Client disconnected
                    printf("[server] Client on socket %d disconnected.\n", curr_fd);
                    close(curr_fd);
                    client_sockets[i] = -1;
                } else {
                    // broadcasts to everyone else
                    printf("[%s]: %s", msg.username, msg.text);
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_sockets[j] != -1 != curr_fd) {
                            write(client_sockets[j], &msg, sizeof(struct message));
                        }
                    }
                }
            }
=======
    int yes = 1;
    if ( setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1 ) {
        printf("sockopt  error\n");
        printf("%s\n",strerror(errno));
        exit(-1);
    }
    int err = bind(listen_socket, results->ai_addr, results->ai_addrlen);
    if(err == -1){
        printf("Error binding: %s",strerror(errno));
        exit(1);
    }
    listen(listen_socket, MAX_CLIENTS);//3 clients can wait to be processed

    printf("Listening on port %s\n",PORT);
    //server listens for clients to accept


    socklen_t sock_size;
    struct sockaddr_storage client_address;
    sock_size = sizeof(client_address);

    fd_set read_fds;

    char buff[1025]="";

      while(1){

        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(listen_socket,&read_fds);
        int i = select(listen_socket+1, &read_fds, NULL, NULL, NULL);

        //if standard in, use fgets
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            fgets(buff, sizeof(buff), stdin);
            buff[strlen(buff)-1]=0;
            printf("Recieved from terminal: '%s'\n",buff);
        }

        // if socket
        if (FD_ISSET(listen_socket, &read_fds)) {
            //accept the connection
            int client_socket = accept(listen_socket,(struct sockaddr *)&client_address, &sock_size);
            printf("Connected, waiting for data.\n");

            //read the whole buff
            read(client_socket,buff, sizeof(buff));
            //trim the string
            buff[strlen(buff)-1]=0; //clear newline
            if(buff[strlen(buff)-1]==13){
                //clear windows line ending
                buff[strlen(buff)-1]=0;
            }

            printf("\nRecieved from client '%s'\n",buff);
            close(client_socket);
>>>>>>> 807cd8ea70614ca2e25239c59985c37662c2de26
        }
    }



    free(hints);
    freeaddrinfo(results);
    return 0;

}
