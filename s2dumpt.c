#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define MAX_PENDING 5
#define MAX_LINE 256

int main() {
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    int len;
    int s, new_s;
    socklen_t sin_len = sizeof(sin); // Initialize sin_len

    // Build address data structure
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    // Setup passive open
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
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

    if (listen(s, MAX_PENDING) < 0) {
        perror("listen");
        close(s); // Close the socket before exiting
        exit(1);
    }
    printf("Server is listening...\n");

    // Main loop: wait for connection, then receive and send text
    while (1) {
        if ((new_s = accept(s, (struct sockaddr *)&sin, &sin_len)) < 0) {
            perror("accept");
            close(s); // Close the listening socket before exiting
            exit(1);
        }
        printf("Client connected.\n");

        while (1) {
            // Receive message from client
            len = recv(new_s, buf, sizeof(buf) - 1, 0);
            if (len <= 0) break;
            buf[len] = '\0'; // Null-terminate the received data
            printf("Client sends the msg: %s\n", buf);

            // Send message to client
            printf("Chat with client: ");
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                perror("fgets");
                break; // Exit loop if fgets fails
            }

            // Remove newline character if present
            size_t input_len = strlen(buf);
            if (input_len > 0 && buf[input_len - 1] == '\n') {
                buf[input_len - 1] = '\0';
            }

            if (send(new_s, buf, strlen(buf) + 1, 0) < 0) {
                perror("send");
                break;
            }
        }

        printf("Client disconnected.\n");
        close(new_s);
    }

    close(s);
    return 0;
}
