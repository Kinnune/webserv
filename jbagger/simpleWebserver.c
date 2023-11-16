#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER_SIZE] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror(RED"Socket creation failed"RESET);
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to localhost and the specified port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror(RED"Bind failed"RESET);
        exit(EXIT_FAILURE);
    }

	// Print the server address and port
	printf("Server address: "GREEN"%s\n"RESET, inet_ntoa(address.sin_addr));
	printf("Server listening on port "GREEN"%d\n\n"RESET, PORT);

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        perror(RED"Listen failed"RESET);
        exit(EXIT_FAILURE);
    }

    // Accept a connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror(RED"Accept failed"RESET);
        exit(EXIT_FAILURE);
    }

    // Read the data from the client and print it
    ssize_t valread = read(new_socket, buffer, MAX_BUFFER_SIZE);
    printf(CYAN"Client Request:"RESET"\n%s\n", buffer);

    // Close the sockets
    close(new_socket);
    close(server_fd);

    return 0;
}