CC = gcc
CFLAGS = -Wall
.PHONY: compile clean run_server run_client
all: compile

compile: server client

server: server.o networking.o
	$(CC) -o server server.o networking.o

client: client.o networking.o
	$(CC) -o client client.o networking.o

server.o: server.c networking.h
	$(CC) $(CFLAGS) -c server.c

run_server: server
	@echo "Starting Server..."
	./server

run_client: client
	@echo "Starting Client..."
	./client 127.0.0.1

client.o: client.c networking.h
	$(CC) $(CFLAGS) -c client.c

networking.o: networking.c networking.h
	$(CC) $(CFLAGS) -c networking.c

clean:
	rm -f *.o server client
