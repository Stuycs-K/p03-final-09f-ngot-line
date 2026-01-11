#include "networking.h"

//handle zomb-processes
void sighandler(int sig) {
    if (sig == SIGCHLD) {
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
}


int main() {
    signal(SIGCHLD, sighandler); //trying to fix z-processes issue

    // setups Shared Memory for the socket table
    // This allows children to see everyone's socket ID for broadcasting
    int shmid = shmget(SHM_KEY, sizeof(int) * MAX_CLIENTS, IPC_CREAT | 0644);
    int *client_sockets = shmat(shmid, NULL, 0);

    // initalizing all slots to -1 (or empty)
    for(int i=0; i<MAX_CLIENTS; i++) client_sockets[i] = -1;

    int listen_socket = server_setup();
    printf("[server] Multi-user server started on port %s...\n", PORT);

    //implements our handshake logic to connect an disconnetc users
    while (1) {
        int client_socket = server_tcp_handshake(listen_socket);

        //finds an empty spot in shared memory for this new user
        int slot = -1;
        for(int i=0; i<MAX_CLIENTS; i++) {
            if (client_sockets[i] == -1) {
                client_sockets[i] = client_socket;
                slot = i;
                break;
            }
        }

        int f = fork();
        if (f == 0) { // CHILD PROCESS
            close(listen_socket);
            struct message incoming;

            // Read from THIS client
            while (read(client_socket, &incoming, sizeof(struct message)) > 0) {
                printf("[%s]: %s", incoming.username, incoming.text);

                // broadcast to everyone else in the table
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    int target_fd = client_sockets[i];
                    if (target_fd != -1 && target_fd != client_socket) {
                        write(target_fd, &incoming, sizeof(struct message));
                    }
                }
            }

            // Cleanup when they leave
            printf("[server] Client %s disconnected.\n", incoming.username);
            client_sockets[slot] = -1; // Vacate the spot
            close(client_socket);
            exit(0);
        }
        else { // PARENT PROCESS
            close(client_socket);
        }
    }
    return 0;
}
