#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAXLINE 1024

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

int main() {
  int sockfd;
  char buffer[MAXLINE];
  struct sockaddr_in servaddr, cliaddr;

  // Generator polynomial (e.g., "1101" for CRC-3)
  char generator[] = "1101";
  int gen_len = strlen(generator);

  // Create socket
  if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Server address
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  // Bind the socket
  if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("Bind failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  int len, n;
  len = sizeof(cliaddr);

  // Receive binary message with CRC from client
  n = recvfrom(sockfd, buffer, MAXLINE-1, 0,
               (struct sockaddr *)&cliaddr, &len);
  buffer[n] = '\0';

  // Extract the original message from the received message with CRC
  char original_message[n - (gen_len - 1) + 1];
  strncpy(original_message, buffer, n - (gen_len - 1));
  original_message[n - (gen_len - 1)] = '\0';

  // Compute the CRC remainder
  char remainder[gen_len];
  compute_crc(buffer, generator, remainder);

  // Check for errors
  int error_detected = 0;
  for (int i = 0; i < gen_len - 1; i++) {
    if (remainder[i] != '0') {
      error_detected = 1;
      break;
    }
  }

  if (error_detected) {
    printf("Error detected in received message: %s\n", buffer);
  } else {
    printf("No error detected in received message: %s\n", original_message);
  }

  close(sockfd);
  return 0;
}
