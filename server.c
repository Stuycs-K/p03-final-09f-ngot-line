#include "networking.h"

define CLR_RESET  "\x1b[0m"
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
    int listen_socket = server_setup();
    printf("[server] Multi-user server started  on port %s...\n", PORT);

    // array of all connected client sockets
    int client_sockets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) client_sockets[i] = -1;

    fd_set read_fds;
    char buffer[sizeof(struct message)];

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
        }
    }
    return 0;
}
