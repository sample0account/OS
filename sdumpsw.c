#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024



int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buffer[BUF_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is ready to receive data...\n");

    while (1) {

    int n = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)cliaddr, &len);
    buffer[n] = '\0';
    int received_seq_num = atoi(buffer);
    printf("Received packet with sequence number: %d\n", received_seq_num);

    // Simulate sending an ACK for the received packet
    sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)cliaddr, len);
    }

    close(sockfd);
    return 0;
}
