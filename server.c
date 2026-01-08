#include "networking.h"

int main() {
    int listen_socket, client_socket;
    struct sockaddr_in server_addr;
    struct message incoming;

    //socket
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);

      // address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on any interface

    // bind and listen
    bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(listen_socket, 5);
    printf("[server] Listening on port %d...\n", PORT);

    //accepting connection
    client_socket = accept(listen_socket, NULL, NULL);
    printf("[server] Connected to a client!\n");

    // reading a message struct
    read(client_socket, &incoming, sizeof(struct message));
    printf("[%s]: %s\n", incoming.username, incoming.text);

    close(client_socket);
    close(listen_socket);
    return 0;
}


