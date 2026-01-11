CC = gcc
CFLAGS = -Wall


#uses shell to get port from networking.h
PORT = $(shell grep "#define PORT" networking.h | cut -d '"' -f 2)

.PHONY: compile clean run_server run_client
all: compile

compile: server client

server: server.o networking.o
	$(CC) -o server server.o networking.o

client: client.o networking.o
	$(CC) -o client client.o networking.o

server.o: server.c networking.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c networking.h
	$(CC) $(CFLAGS) -c client.c

networking.o: networking.c networking.h
	$(CC) $(CFLAGS) -c networking.c

#run cmds
run_server: server
	@echo "Starting Server on Port $(PORT)....."
	./server

run_client: client
	@echo "Starting Client (connecting to localhost)..."
	./client 127.0.0.1

#cleanup
clean:
	@echo "Cleaning up files and IPC resources..."
	rm -f *.o server client
	# remove shared memory
	-ipcrm -M $(SHM_KEY) 2>/dev/null || true
