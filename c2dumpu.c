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
    int len, sin_len;

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
    sin_len = sizeof(sin);

    // Active open
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    printf("Client socket created successfully.\n");

    // Main loop: get and send lines of text
    while (1) {
        // Send message to server
        printf("Start chat with server: ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("fgets");
            continue; // Continue to the next iteration if fgets fails
        }

        // Remove newline character if present
        size_t input_len = strlen(buf);
        if (input_len > 0 && buf[input_len - 1] == '\n') {
            buf[input_len - 1] = '\0';
        }

        if (sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&sin, sin_len) < 0) {
            perror("sendto");
            exit(1);
        }

        // Receive message from server
        len = recvfrom(s, buf, MAX_LINE - 1, 0, (struct sockaddr *)&sin, &sin_len);
        if (len < 0) {
            perror("recvfrom");
            exit(1);
        }
        buf[len] = '\0'; // Null-terminate the received data
        printf("Server sends the msg: %s\n", buf);
       // fputs(buf,stdout);
    }

    close(s);
    return 0;
}
