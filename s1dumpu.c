#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define MAX_LINE 256

int main() {
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    int len;
    int s;

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    /* setup passive open */
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    }
    printf("Server socket was created successfully.\n");

    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("simplex-talk: bind");
        exit(1);
    }

    len = sizeof(sin);
    while (1) {
        int n = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&sin, &len);
        if (n < 0) {
            perror("Error in recvfrom");
            exit(1);
        }
        buf[n] = '\0'; // Null-terminate the received data
        fputs(buf, stdout);
    }

    close(s);
    return 0;
}