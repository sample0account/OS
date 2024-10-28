#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAXLINE 1024

// Function to perform modulo-2 division and compute the remainder
void compute_crc(char *message, char *generator, char *remainder) {
    int msg_len = strlen(message);
    int gen_len = strlen(generator);

    // Copy the message into the remainder
    strcpy(remainder, message);

    for (int i = 0; i <= msg_len - gen_len; i++) {
        // Perform XOR if the current bit is 1
        if (remainder[i] == '1') {
            for (int j = 0; j < gen_len; j++) {
                remainder[i + j] = (remainder[i + j] == generator[j]) ? '0' : '1';
            }
        }
    }
    // The remainder is now the last (gen_len - 1) bits of the remainder array
    memcpy(remainder, remainder + msg_len - gen_len + 1, gen_len - 1);
    remainder[gen_len - 1] = '\0';
}

// Function to change a bit at a given position in the message
void change_bit(char *message, int position) {
    if (message[position] == '0') {
        message[position] = '1';
    } else {
        message[position] = '0';
    }
}

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr;

    // Generator polynomial (e.g., "1101" for CRC-3)
    char generator[] = "1101";
    int gen_len = strlen(generator);

    // Input binary message
    printf("Enter a binary message: ");
    scanf("%s", buffer);

    // Append zeros to the message
    int msg_len = strlen(buffer);
    for (int i = 0; i < gen_len - 1; i++) {
        buffer[msg_len + i] = '0';
    }
    buffer[msg_len + gen_len - 1] = '\0';

    // Compute the CRC remainder
    char remainder[gen_len];
    compute_crc(buffer, generator, remainder);

    // Display CRC remainder
    printf("CRC remainder: %s\n", remainder);

    // Append the remainder to the original message
    char message_with_crc[msg_len + gen_len];
    strncpy(message_with_crc, buffer, msg_len);
    strncat(message_with_crc, remainder, gen_len - 1);
    message_with_crc[msg_len + gen_len - 1] = '\0';
    printf("Message with CRC bits: %s\n", message_with_crc);

    // Ask user whether to send the correct bit or change a bit
    char choice;
    printf("Do you want to send the message as is (y/n)? ");
    scanf(" %c", &choice);

    if (choice == 'n') {
        int position;
        printf("Enter the position (0-based index) of the bit to change: ");
        scanf("%d", &position);
        if (position >= 0 && position < msg_len + gen_len - 1) {
            change_bit(message_with_crc, position);
            printf("Modified message with CRC bits: %s\n", message_with_crc);
        } else {
            printf("Invalid position!\n");
            return EXIT_FAILURE;
        }
    }

    // Create socket
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Send binary message with CRC to server
    sendto(sockfd, message_with_crc, strlen(message_with_crc), 0,(struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Binary message sent: %s\n", message_with_crc);
    close(sockfd);
    return 0;
}
