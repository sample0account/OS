#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 1024

// Function to perform modulo-2 division and compute the remainder
void compute_crc(char *message, char *generator, char *remainder) {
    int msg_len = strlen(message);
    int gen_len = strlen(generator);
    char temp[MAXLINE];

    // Copy the message into a temporary buffer
    strcpy(temp, message);

    // Perform XOR division
    for (int i = 0; i <= msg_len - gen_len; i++) {
        if (temp[i] == '1') {
            for (int j = 0; j < gen_len; j++) {
                temp[i + j] = (temp[i + j] == generator[j]) ? '0' : '1';
            }
        }
    }

    // Copy the remainder part from the temp buffer (last gen_len-1 bits)
    strncpy(remainder, temp + msg_len - gen_len + 1, gen_len - 1);
    remainder[gen_len - 1] = '\0'; // Null-terminate the remainder
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
    char buffer[MAXLINE];

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
    char message_with_crc[MAXLINE];
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

    // Compute the CRC remainder to check for errors
    char received_remainder[gen_len];
    compute_crc(message_with_crc, generator, received_remainder);

    // Check for errors
    int error_detected = 0;
    for (int i = 0; i < gen_len - 1; i++) {
        if (received_remainder[i] != '0') {
            error_detected = 1;
            break;
        }
    }

    if (error_detected) {
        printf("Error detected in received message: %s\n", message_with_crc);
    } else {
        printf("No error detected in received message: %s\n", message_with_crc);
    }

    return 0;
}
