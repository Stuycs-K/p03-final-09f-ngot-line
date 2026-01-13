#include "networking.h"

#define CLR_RESET  "\x1b[0m"
#define CLR_RED    "\x1b[31m"
#define CLR_GREEN  "\x1b[32m"
#define CLR_YELLOW "\x1b[33m"
#define CLR_BLUE   "\x1b[34m"
#define CLR_MAGENTA "\x1b[35m"
#define CLR_CYAN   "\x1b[36m"

char *colors[] = {CLR_RED, CLR_GREEN, CLR_YELLOW, CLR_BLUE, CLR_MAGENTA, CLR_CYAN};
struct client chatroom[MAX_CLIENTS];

void exit_handler(int sig) {
    printf("\n[Server] Shutting down. Closing all connections...\n");
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
    for (int i = 0; i < MAX_CLIENTS; i++) {
            chatroom[i].socket = -1;
            chatroom[i].color = NULL;
        }
    fd_set read_fds;
    char buffer[sizeof(struct message)];

    while (1) {
          FD_ZERO(&read_fds);
          FD_SET(listen_socket, &read_fds); // watches for new ppl
          int max_fd = listen_socket;

          //adds every active socket from our struct array to the set
          for (int i = 0; i < MAX_CLIENTS; i++) {
              if (chatroom[i].socket != -1) {
                  FD_SET(chatroom[i].socket, &read_fds);
                  if (chatroom[i].socket > max_fd) {
                      max_fd = chatroom[i].socket;
                  }
              }
          }

          //waits for activity
          int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
          if (activity < 0) {
              perror("select error");
              continue;
          }

          // handles new connection
          if (FD_ISSET(listen_socket, &read_fds)) {
              int new_fd = server_tcp_handshake(listen_socket);
              if (new_fd != -1) {
                  for (int i = 0; i < MAX_CLIENTS; i++) {
                      if (chatroom[i].socket == -1) {
                          chatroom[i].socket = new_fd;
                          chatroom[i].color = colors[i % 6]; // assigns color
                          printf("[server] New client in slot %d (Socket %d)\n", i, new_fd);
                          break;
                      }
                  }
              }
          }

          // handles incoming msgs
          for (int i = 0; i < MAX_CLIENTS; i++) {
              int curr_fd = chatroom[i].socket;

              if (curr_fd != -1 && FD_ISSET(curr_fd, &read_fds)) {
                  struct message msg;
                  int bytes = read(curr_fd, &msg, sizeof(struct message));

                  if (bytes <= 0) {
                      // Client disconnected
                      printf("[server] Socket %d disconnected.\n", curr_fd);
                      close(curr_fd);
                      chatroom[i].socket = -1;
                  } else {
                      // SERVER SIDE LOGGING (before adding color)
                      printf("[%s]: %s", msg.username, msg.text);

                      //wraps the username in color
                      char colored_name[128];
                      //color + bracket  + username + bracket + clr_reset
                      sprintf(colored_name, "%s%s%s", chatroom[i].color,  msg.username, CLR_RESET);
                      strncpy(msg.username, colored_name, sizeof(msg.username) - 1);

                      // broadcasting
                      for (int j = 0; j < MAX_CLIENTS; j++) {
                          if (chatroom[j].socket != -1) {
                              write(chatroom[j].socket, &msg, sizeof(struct message));
                          }
                      }
                  }
              }
          }
      }
      return 0;
}
