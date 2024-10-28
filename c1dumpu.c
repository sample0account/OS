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

int main(int argc, char * argv[]) {
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buf[MAX_LINE];
    int s;
    int len;

    if (argc != 2) {
        fprintf(stderr, "usage: %s host\n", argv[0]);
        exit(1);
    }

    host = argv[1];

    /* translate host name into peer’s IP address */
    hp = gethostbyname(host);
    if (!hp) {
        fprintf(stderr, "%s: unknown host: %s\n", argv[0], host);
        exit(1);
    }

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    /* create socket */
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    printf("Client socket was created successfully.\n");
    printf("Start the chat with the server\n");

    /* main loop: get and send lines of text */
    while (fgets(buf, sizeof(buf), stdin)) {
        buf[MAX_LINE-1] = '\0'; // Ensure null-terminated string
        len = strlen(buf) + 1;

        /* send message to server */
        if (sendto(s, buf, len, 0, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
            perror("sendto");
            exit(1);
        }
    }

    close(s);
    return 0;
}