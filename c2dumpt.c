#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define MAX_LINE 256

int main(int argc, char *argv[]) {
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buf[MAX_LINE];
    int s;
    int len;

    if (argc == 2) {
        host = argv[1];
    } else {
        fprintf(stderr, "usage: %s host\n", argv[0]);
        exit(1);
    }

    // Translate host name into peer's IP address
    hp = gethostbyname(host);
    if (!hp) {
        fprintf(stderr, "unknown host: %s\n", host);
        exit(1);
    }

    // Build address data structure
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    // Create socket
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    printf("Client socket created successfully.\n");

    // Connect to the server
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("connect");
        close(s);
        exit(1);
    }

    printf("Connected to server. Start chatting...\n");

    // Main loop: get and send lines of text
    while (1) {
        // Send message to server
        printf("Start chat with server: ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("fgets");
            break; // Exit loop if fgets fails
        }

        // Remove newline character if present
        size_t input_len = strlen(buf);
        if (input_len > 0 && buf[input_len - 1] == '\n') {
            buf[input_len - 1] = '\0';
        }

        if (send(s, buf, strlen(buf) + 1, 0) < 0) { // +1 to include null terminator
            perror("send");
            break;
        }

        // Receive message from server
        len = recv(s, buf, sizeof(buf) - 1, 0);
        if (len < 0) {
            perror("recv");
            break;
        } else if (len == 0) {
            printf("Server closed the connection.\n");
            break;
        }
        buf[len] = '\0'; // Null-terminate the received data
        printf("Server sends the msg: %s\n", buf);
    }

    close(s);
    return 0;
}
