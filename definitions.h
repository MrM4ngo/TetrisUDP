#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <ncurses.h>
#include <string.h>
#include <arpa/inet.h>

#define ROWS 20 // you can change height and width of table with ROWS and COLS 
#define COLS 15
#define TRUE 1
#define FALSE 0
#define PACKET_SIZE 1024
