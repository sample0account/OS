#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024
#define WINDOW_SIZE 4  // Adjust the window size here
#define TIMEOUT_THRESHOLD 3 // Timeout threshold in seconds

void send_packet(int sockfd, struct sockaddr_in *servaddr, int seq_num) {
    char buffer[BUF_SIZE];
    sprintf(buffer, "%d", seq_num);
    sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)servaddr, sizeof(*servaddr));
    printf("Sent packet with sequence number: %d\n", seq_num);
}

int simulate_ack(int seq_num) {
    // Simulate a random chance of not receiving an ACK
    return rand() % 10 < 8; // 80% chance of receiving ACK, 20% chance of timeout
}

void receive_ack(int ack_num) {
    printf("Received ACK for packet with sequence number: %d\n", ack_num);
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    int base = 0;               // Base of the window
    int next_seq_num = 0;      // Next sequence number to send
    int total_packets;          // Number of packets to send

    // Seed the random number generator
    srand(time(NULL));

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Get the number of packets from the user
    printf("Enter the total number of packets to be sent: ");
    scanf("%d", &total_packets);

    if (total_packets <= 0) {
        printf("Please enter a positive number of packets.\n");
        return 1;
    }

    while (base < total_packets) {
        // Send packets in the window
        while (next_seq_num < base + WINDOW_SIZE && next_seq_num < total_packets) {
            send_packet(sockfd, &servaddr, next_seq_num);
            next_seq_num++;
        }

        // Simulate receiving an ACK for the base packet
        if (simulate_ack(base)) {
            receive_ack(base);
            base++;  // Slide the window
        } else {
            printf("Timeout! No ACK received for packet with sequence number: %d\n", base);
            // Resend packets starting from the base
            printf("Resending packets from sequence number: %d\n", base);
            next_seq_num = base;  // Reset next_seq_num to base
        }
    }

    printf("All packets sent and acknowledged.\n");
    close(sockfd);
    return 0;
}
