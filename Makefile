CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lncurses

SERVER_SRC = TetrisServer.c
CLIENT_SRC = TetrisClient.c

SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

SERVER_TARGET = TetrisServer
CLIENT_TARGET = TetrisClient

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(SERVER_OBJ) -o $(SERVER_TARGET) $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $(CLIENT_OBJ) -o $(CLIENT_TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ) $(SERVER_TARGET) $(CLIENT_TARGET)
