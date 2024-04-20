#include "definitions.h"
#include "PORT.h"
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ncurses.h>

#define PACKET_SIZE 1024
#define TIMEOUT_SEC 0
#define TIMEOUT_USEC 100000

char GameOn = TRUE;
int input;
int packet_count = 0;
int retransmission_count = 0;
int lost_packet_count = 0;
int successful_transmission_count = 0;
int ack_count = 0;
int timeout_count = 0;

void print_statistics() {
    printf("\nOutput Statistics on Client:\n");
    printf("1. Number of data packets generated for transmission (initial transmission only): %d\n", packet_count);
    printf("2. Total number of data packets generated for retransmission (initial transmissions plus retransmissions): %d\n", packet_count + retransmission_count);
    printf("3. Number of data packets dropped due to loss: %d\n", lost_packet_count);
    printf("4. Number of data packets transmitted successfully (initial transmissions plus retransmissions): %d\n", successful_transmission_count);
    printf("5. Number of ACKs received: %d\n", ack_count);
    printf("6. Count of how many times the timeout expired: %d\n", timeout_count);
}

int main() {
    srand(time(0)); // Seed the random number generator

    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);

    // Create a UDP socket instead of TCP
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct timeval timeout;
    fd_set fds;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Use SOCK_DGRAM for UDP
    if (sockfd < 0) {
        perror("Error opening socket");
        endwin();
        exit(EXIT_FAILURE);
    }

    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        endwin();
        exit(EXIT_FAILURE);
    }

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(PORT);

    // No need to connect with UDP

    // Main game loop
    while (GameOn) {
        char buffer[PACKET_SIZE];
        if ((input = getch()) != ERR) {
            if (input != EOF) {
                snprintf(buffer, PACKET_SIZE, "%c", (char)input);
            }

            // Generate a data packet for transmission
            packet_count++;

            // Send the packet to the server using sendto() for UDP
            n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
            if (n < 0) {
                perror("Error writing to socket");
                endwin();
                exit(EXIT_FAILURE);
            }

            printw("Packet %d generated for transmission\n", packet_count);

            // Set timeout for select()
            timeout.tv_sec = TIMEOUT_SEC;
            timeout.tv_usec = TIMEOUT_USEC;

            FD_ZERO(&fds);
            FD_SET(sockfd, &fds);

            // Wait for ACK or timeout
            int ready = select(sockfd + 1, &fds, NULL, NULL, &timeout);
            if (ready < 0) {
                perror("Error in select");
                endwin();
                exit(EXIT_FAILURE);
            } else if (ready == 0) {
                // Timeout expired
                timeout_count++;
                printw("Timeout expired for packet numbered %d\n", packet_count);
                continue; // Retry sending packet
            }

            // ACK received
            ack_count++;
            printw("ACK %d received\n", ack_count);

            // Packet successfully transmitted
            successful_transmission_count++;
            printw("Packet %d successfully transmitted with %d data bytes\n", packet_count, n);
        } else {
            sleep(1);
            snprintf(buffer, PACKET_SIZE, " ");
            // Send an empty packet
            n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
            if (n < 0) {
                perror("Error writing to socket");
                endwin();
                exit(EXIT_FAILURE);
            }
        }
    }
    print_statistics();

    endwin(); // End curses mode
    close(sockfd); // Close the socket
    return 0;
}
