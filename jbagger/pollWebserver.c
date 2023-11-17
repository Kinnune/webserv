#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"

#define PORT 80
#define MAX_BUFFER_SIZE 1024
#define MAX_CLIENTS 1024

int main()
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[MAX_BUFFER_SIZE] = {0};

	// Creating socket file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == 0)
	{
		perror(RED"Socket creation failed"RESET);
		exit(EXIT_FAILURE);
	}

	// Set up the server address structure
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Set the server socket to non-blocking mode
	fcntl(server_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

	// Bind the socket to localhost and the specified port
	int bind_result = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
	if (bind_result < 0)
	{
		perror(RED"Bind failed"RESET);
		exit(EXIT_FAILURE);
	}

	// Print the server address and port
	printf("Server address: "GREEN"%s\n"RESET, inet_ntoa(address.sin_addr));
	printf("Server listening on port "GREEN"%d\n\n"RESET, PORT);

	struct pollfd clientfds[MAX_CLIENTS];
	nfds_t nfds = 0;
	int timeout = 1 * 1000;

	// Listen for incoming connections
	int listen_result = listen(server_fd, MAX_CLIENTS);
	if (listen_result < 0)
	{
		perror(RED"Listen failed"RESET);
		exit(EXIT_FAILURE);
	}

	// Handle incoming connections
	while (1)
	{
		// Accept a connection
		new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
		if (new_socket < 0)
		{
			perror(RED"Accept failed"RESET);
			exit(EXIT_FAILURE);
		}
		clientfds[nfds].fd = new_socket;
		clientfds[nfds].events = POLLIN;
		nfds++;

		poll(clientfds, nfds, timeout);
	}

	return (0);
}

// accept will not block if you set the socket to non-blocking mode. You can do this with fcntl: