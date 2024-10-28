#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

//Make sure to use a port that doesn't conflict with Replit's webview system
#define PORT 9999
#define BUFFER_SIZE 1024

int main() {
  int server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);

  // Create socket
  if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Prepare the server address
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to localhost
  server_addr.sin_port = htons(PORT);

  // Bind the socket to the server address
  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0) {
    perror("Bind failed");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(server_socket, 5) < 0) {
    perror("Listen failed");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d...\n", PORT);
  int bytes_received;
  // Accept and handle client connections
  while (1) {
    client_socket =
        accept(server_socket, (struct sockaddr *)&server_addr, &bytes_received);
    if (client_socket < 0) {
      perror("Client connection failed");
      continue;
    }
    printf("Accepted connection\n");

    // Handle the client in a new process (forking for simplicity)
    if (fork() == 0) {
      // Child process: handle the client
      char buffer[BUFFER_SIZE];
      char *response;
      struct hostent *host;
      struct in_addr **addr_list;

      // Receive domain name from client
      bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
      if (bytes_received < 0) {
        perror("Receive failed");
        close(client_socket);
        
      }
      buffer[bytes_received] = '\0'; // Null-terminate the received data
      printf("Received domain: %s\n", buffer);
      // Get the IP address of the domain
      if ((host = gethostbyname(buffer)) == NULL) {
        response = "Could not resolve domain";
      } else {
        addr_list = (struct in_addr **)host->h_addr_list;
        response = inet_ntoa(*addr_list[0]); // Get the first IP address
      }
      // Send the IP address (or error message) to the client
      send(client_socket, response, strlen(response), 0);
      // Close the client connection
      close(client_socket);
      exit(0);
    } else {
      // Parent process: continue to accept new connections
      close(client_socket);
    }
  }

  // Close the server socket
  close(server_socket);
  return 0;
}
