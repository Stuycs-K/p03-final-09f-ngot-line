#include "networking.h"

int global_shmid;
struct client_info *chatroom_data;; // store pid of clients
//shuts down server, deletes the shared memory from the OS
void exit_handler(int sig) {
  printf("\n[Server] Shutting down. Cleaning up processes and memory...\n");
    //kills all active child processes
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (chatroom_data[i].pid > 0) {
          printf("[Server] Killing child process %d\n", chatroom_data[i].pid);
          kill(chatroom_data[i].pid, SIGTERM);
      }
    }

    // removes the SM segment from the OS
    shmctl(global_shmid, IPC_RMID, NULL);

    printf("[Server] Cleanup complete. until next time :D !\n");
    exit(0);
}

//handle zomb-processes
void sighandler(int sig) {
    if (sig == SIGCHLD) {
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
}


int main() {

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

    int listen_socket = server_setup();
    printf("[server] Multi-user server started on port %s...\n", PORT);

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
        }
    }



    free(hints);
    freeaddrinfo(results);
    return 0;

}
