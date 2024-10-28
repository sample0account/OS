#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define BUF_SIZE 1024
#define WINDOW_SIZE 1  // You can adjust the window size here
#define TIMEOUT_THRESHOLD 3 // Define a threshold for timeouts (in seconds)

void send_packet(int seq_num) {
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
    int base = 0;               // Base of the window
    int next_seq_num = 0;      // Next sequence number to send
    int total_packets;          // Number of packets to send

    // Seed the random number generator
    srand(time(NULL));

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
            send_packet(next_seq_num);
            next_seq_num++;
            sleep(1);  // Simulate network delay
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
    return 0;
}

