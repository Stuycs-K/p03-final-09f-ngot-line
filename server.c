#include "networking.h"

int main() {
    int listen_socket, client_socket;
    struct sockaddr_in server_addr;
    struct message incoming;

    //socket
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
}
