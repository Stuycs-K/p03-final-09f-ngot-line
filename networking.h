#ifndef NETWORKING_H
#define NETWORKING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/stat.h>
#include <signal.h>

#define PORT 9001
#define BUFFER_SIZE 1024

struct message {
    char username[32];
    char text[BUFFER_SIZE];
};
int client_tcp_handshake(char * server_address);

#endif

