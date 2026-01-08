#include "networking.h"


void clientLogic(int server_socket){
  char buffer[BUFFER_SIZE];

  // quit if fgets() returns null (ctrl-d or eof)
  while (printf("Enter a message: "), fgets(buffer, sizeof(buffer), stdin)) {
      // Remove the trailing newline from fgets
      char *p = strchr(buffer, '\n');
      if (p) *p = 0;

      // Send the message to the server
      // We use sizeof(buffer) or strlen(buffer) + 1 to include the null terminator
      write(server_socket, buffer, sizeof(buffer));

      // quit if read() returns 0 bytes (socket closed)
      int bytes_read = read(server_socket, buffer, sizeof(buffer));

      if (bytes_read == 0) {
          printf("\nServer closed connection.\n");
          break;
      }

      printf("received: '%s'\n", buffer);
  }

  printf("Client closed\n");
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr;
    struct message my_msg;
}
