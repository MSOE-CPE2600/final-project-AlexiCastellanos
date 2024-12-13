CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lrt
TARGETS = server client

all: $(TARGETS)

server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LIBS)

client: client.c
	$(CC) $(CFLAGS) -o client client.c $(LIBS)

clean:
	rm -f $(TARGETS) *.o voting_results.txt
	@echo "Cleaned up build files and voting results."
