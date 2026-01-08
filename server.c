#include "networking.h"
void sighandler(int sig) {
    if (sig == SIGCHLD) {
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
}


int main() {
    signal(SIGCHLD, sighandler); //trying to fix z-processes issue

    int listen_socket, client_socket;
    struct sockaddr_in server_addr;
    struct message incoming;

    listen_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(listen_socket, 5);
    printf("[server] Multi-user server started on port %d...\n", PORT);

    while (1) {
        client_socket = accept(listen_socket, NULL, NULL);

        // Fork a new process for each client
        int f = fork();
        if (f == 0) { // CHILD PROCESS
            close(listen_socket); // Child doesn't need the listener

            while (read(client_socket, &incoming, sizeof(struct message)) > 0) {
                printf("[%s]: %s", incoming.username, incoming.text);
                // Later, we will add code here to broadcast to other users
            }

            printf("[server] Client disconnected.\n");
            close(client_socket);
            exit(0); // Kill child process when client leaves
        } else { // PARENT PROCESS
            close(client_socket); // Parent doesn't need this specific client's socket
        }
    }
    return 0;
}

