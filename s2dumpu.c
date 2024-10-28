#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define MAX_LINE 256

int main() {
    struct sockaddr_in sin, client_addr;
    char buf[MAX_LINE];
    int len, client_addr_len;
    int s;

    // Build address data structure
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    // Setup passive open
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    printf("Server socket created successfully.\n");

    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind");
        close(s); // Close the socket before exiting
        exit(1);
    }
    printf("Bind successful.\n");

    printf("Server is listening...\n");

    // Main loop: wait for connection, then receive and send text
    while (1) {
        client_addr_len = sizeof(client_addr);
        len = recvfrom(s, buf, MAX_LINE - 1, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (len < 0) {
            perror("recvfrom");
            close(s); // Close the socket before exiting
            exit(1);
        }
        buf[len] = '\0'; // Null-terminate the received data
        printf("Client sends the msg: %s\n", buf);
        //fputs(buf, stdout);

        // Send message to client
        printf("Chat with client: ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("fgets");
            continue; // Continue to the next iteration if fgets fails
        }

        // Remove newline character from the end if it exists
        size_t input_len = strlen(buf);
        if (input_len > 0 && buf[input_len - 1] == '\n') {
            buf[input_len - 1] = '\0';
        }

        if (sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&client_addr, client_addr_len) < 0) {
            perror("sendto");
            close(s); // Close the socket before exiting
            exit(1);
        }
    }

    close(s);
    return 0;
}
