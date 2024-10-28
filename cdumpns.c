#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 9999
#define BUFFER_SIZE 1024

int main() {
  int client_socket;
  struct sockaddr_in server_addr;
  char domain[BUFFER_SIZE];
  char response[BUFFER_SIZE];

  // Create socket
  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Server address setup
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY; 
  server_addr.sin_port = htons(PORT);

  // Connect to the server
  if (connect(client_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    perror("Connection failed");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  // Get domain name from user
  printf("Enter domain name: ");
  fgets(domain, BUFFER_SIZE, stdin);
  domain[strcspn(domain, "\n")] = 0; // Remove newline character

  // Send domain name to the server
  send(client_socket, domain, strlen(domain), 0);

  // Receive response from the server
  int bytes_received = recv(client_socket, response, BUFFER_SIZE - 1, 0);
  if (bytes_received < 0) {
    perror("Receive failed");
  } else {
    response[bytes_received] = '\0'; // Null-terminate the response
    printf("Server response: %s\n", response);
  }

  // Close the socket
  close(client_socket);
  return 0;
}
s