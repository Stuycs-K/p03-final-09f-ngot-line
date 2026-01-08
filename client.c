#include "networking.h"


void clientLogic(int server_socket){
  char buffer[BUFFER_SIZE];

  // quit if fgets() returns null (ctrl-d or eof)
  while (printf("Enter a message: "), fgets(buffer, sizeof(buffer), stdin)) {

      char *p = strchr(buffer, '\n');
      if (p) *p = 0;

      // Send the msg to the server
      // We use sizeof(buffer) or strlen(buffer) + 1() null terminator
      write(server_socket, buffer, sizeof(buffer));

      // quit if read() returns 0 bytes (socket closed)
      int bytes_read = read(server_socket, buffer, sizeof(buffer));

      if (bytes_read == 0) {
          printf("\nMsg empty?\n");
      }

      printf("received: '%s'\n", buffer);
  }

  printf("Client closed\n");
}
int main() {
    int server_socket;
    struct sockaddr_in server_addr;
    struct message my_msg;

    printf("Enter username: ");
    fgets(my_msg.username, sizeof(my_msg.username), stdin);
    my_msg.username[strcspn(my_msg.username, "\n")] = 0; // Remove newline

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_aton("127.0.0.1", &server_addr.sin_addr);

    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        return 1;
    }

    printf("Connected! Type messages below:\n");
    while (1) {
        printf("> ");
        fgets(my_msg.text, sizeof(my_msg.text), stdin);
        write(server_socket, &my_msg, sizeof(struct message));
    }

    close(server_socket);
    return 0;
}




