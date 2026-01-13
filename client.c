#include "networking.h"

//when client is connected this is the logic that handles their actions
int main(int argc, char *argv[]) {
    int server_socket;
    struct message my_msg;
    char *server_ip = "127.0.0.1";

    //allows one computer to setup server and another computer to connect to it.
    if (argc > 1) server_ip = argv[1];

    printf("Enter username: ");
    fgets(my_msg.username, sizeof(my_msg.username), stdin);
    my_msg.username[strcspn(my_msg.username, "\n")] = 0;

    //client connects
    server_socket = client_tcp_handshake(server_ip);

    printf("Connected! Type messages below:\n");

    //forks the client so client can chat and receive msgs same time
    int f = fork();

    if (f == 0) {
        // CHILD PROCESS: Always listening for incoming messages
        struct message incoming;
        while (read(server_socket, &incoming, sizeof(struct message)) > 0) {
            // \r clears the current line so the prompt stays clean
            printf("\r%s: %s", incoming.username, incoming.text);
            fflush(stdout);
        }
        printf("\n[Client] Server disconnected.\n");
        exit(0);
    }
    else {
        // PARENT PROCESS: Always waiting for user to type
        while (1) {
            printf("> ");
            if (fgets(my_msg.text, sizeof(my_msg.text), stdin) == NULL) break;

            write(server_socket, &my_msg, sizeof(struct message));
        }
    }

    close(server_socket);
    return 0;
}
