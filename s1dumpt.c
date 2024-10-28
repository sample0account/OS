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

void handle_request(int client_sock) {
    char buf[MAX_LINE];
    int n;

    // Receive the HTTP request from the client
    while ((n = recv(client_sock, buf, sizeof(buf) - 1, 0)) > 0) {
        buf[n] = '\0'; // Null-terminate the buffer
        printf("Received request:\n%s\n", buf);

        // Simple HTTP response
        const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nContent-Type: text/plain\r\n\r\nHello, World!";
        send(client_sock, response, strlen(response), 0);
    }

    if (n < 0) {
        perror("recv");
    }

    // Close the client socket
    close(client_sock);
}

int main() {
    struct sockaddr_in sin;
    int s, new_s;
    socklen_t client_len;

    // Initialize the sockaddr_in structure
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    // Create a socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // Bind the socket to the address and port
    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind");
        close(s);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(s, MAX_PENDING) < 0) {
        perror("listen");
        close(s);
        exit(1);
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    // Main loop: accept and handle connections
    while (1) {
        client_len = sizeof(sin);
        if ((new_s = accept(s, (struct sockaddr *)&sin, &client_len)) < 0) {
            perror("accept");
            close(s);
            exit(1);
        }

        handle_request(new_s);
    }

    // Close the listening socket (never reached due to infinite loop)
    close(s);
    return 0;
}
