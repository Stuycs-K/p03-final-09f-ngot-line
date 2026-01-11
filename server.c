#include "networking.h"

int global_shmid;
struct client_info *chatroom_data;; // store pid of clients
//shuts down server, deletes the shared memory from the OS
void exit_handler(int sig) {
  printf("\n[Server] Shutting down. Cleaning up processes and memory...\n");
    //kills all active child processes
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (chatroom_data[i].pid > 0) {
          printf("[Server] Killing child process %d\n", chatroom_data[i].pid);
          kill(chatroom_data[i].pid, SIGTERM);
      }
    }

    // removes the SM segment from the OS
    shmctl(global_shmid, IPC_RMID, NULL);

    printf("[Server] Cleanup complete. until next time :D !\n");
    exit(0);
}

//handle zomb-processes
void sighandler(int sig) {
    if (sig == SIGCHLD) {
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
}


int main() {

  //Tell the OS: "If someone hits Ctrl+C (SIGINT), run my exit_handler"
  signal(SIGINT, exit_handler);
  signal(SIGCHLD, sighandler); //trying to fix z-processes issue

    // setups Shared Memory for the socket table
    // This allows children to see everyone's socket ID for broadcasting
    int shmid = shmget(SHM_KEY, sizeof(struct client_info) * MAX_CLIENTS, IPC_CREAT | 0644);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    global_shmid = shmid;
    chatroom_data = shmat(shmid, NULL, 0);

    // Initialize all slots
    for(int i = 0; i < MAX_CLIENTS; i++) {
        chatroom_data[i].socket = -1;
        chatroom_data[i].pid = -1;
    }

    int listen_socket = server_setup();
    printf("[server] Multi-user server started on port %s...\n", PORT);

    //server listens for clients to accept
    while (1) {
        int client_socket = server_tcp_handshake(listen_socket);
        //finds empty slot
        int slot = -1;
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if (chatroom_data[i].socket == -1) {
                slot = i;
                break;
            }
        }

        int f = fork();
        if (f == 0) { // --- CHILD PROCESS ---
            close(listen_socket);
            struct message incoming;

            // child records own processes fro broadcasting
            chatroom_data[slot].socket = client_socket;

            while (read(client_socket, &incoming, sizeof(struct message)) > 0) {
                printf("[%s]: %s", incoming.username, incoming.text);

                // broadcast to everyone else
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    int target_fd = chatroom_data[i].socket;
                    if (target_fd != -1 && target_fd != client_socket) {
                        write(target_fd, &incoming, sizeof(struct message));
                    }
                }
            }

            // Client disconnected: cleanup this slot
            printf("[server] Client disconnected.\n");
            chatroom_data[slot].socket = -1;
            chatroom_data[slot].pid = -1;
            close(client_socket);
            exit(0);

        } else { // --- PARENT PROCESS ---
            // parent records pid so it can kill this child later
            chatroom_data[slot].pid = f;
            close(client_socket);
        }
    }
    return 0;
}
