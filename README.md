[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/QfAIDTGi)
# Final Project Proposal

## Group Members:

Timmy Ngo, Ethan Lin

# Intentions:

We are intending to create a real-time chat service that allows for communication between different machines over a local network.
There aren't any command-lines tools that provide a way for synchronized and centralized communication between people on a network, so our project intends to solve this by creating a central server that manages incoming client connections and messaging between these clients.

# Intended usage:


* Client-side: Users will run the make client and be prompted to enter in a unique username. Then the user will connect to the local network's chatroom and be able to send messages.

* Messaging: Once connected, the interface will split the terminal where users can type messages from stdin.

* Commands: Users can use special commands like /exit to leave or /list to see other active users.

# Technical Details:


Some of the technical aspects we will be using in this final project include,
* Sockets to establish a connection and allow for inter-computer communication
* Allocating memory to match the size of message being sent
* Shared memory so the clients have a shared chat log
* Signals so we can catch when clients leave the chat

Timmy: Making client logic, Prompting users for username, messaging, commands
Ethan: Making the serve logic, persistent server, semaphore shared msg log,
# Intended pacing:

Jan 6-7:  Create the server logic to receive messages/ client to send messages
Jan 8-9: Client is able to send messages to the server, finishing the server-client logic with shared msg log
Jan 10-11:   Clients are able to send/receive messages between each other
Jan 12-13: MVP Should be working, else spend time to fix missing bugs
Jan 14-16: Extra features including chat colors, DM, Emojis, Shortcuts ncurses, multi-client if time allows.
