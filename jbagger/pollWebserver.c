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

#define PORT 8080
#define MAX_BUFFER_SIZE 1024
#define MAX_CLIENTS 1024

int main()
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[MAX_BUFFER_SIZE] = {0};
	int poll_return;

	// Creating socket file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == 0)
	{
		perror(RED"Socket creation failed"RESET);
		exit(EXIT_FAILURE);
	}

	struct pollfd serverpoll;
	memset(&serverpoll, 0, sizeof(serverpoll));
	serverpoll.fd = server_fd;

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
	
	// Listen for incoming connections
	int listen_result = listen(server_fd, MAX_CLIENTS);
	if (listen_result < 0)
	{
		perror(RED"Listen failed"RESET);
		exit(EXIT_FAILURE);
	}

	// Initialize the pollfd struct for the server socket
	struct pollfd clientfds[MAX_CLIENTS];
	nfds_t nfds = 0;
	int timeout = 1 * 1000;

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		memset(&clientfds[i], 0, sizeof(clientfds[i]));
	}




	// Handle incoming connections
	while (1)
	{
		// Check for new connections or data from existing connections
		printf("Polling...\n");
		usleep(1000000);
		// if (nfds != 0)
		poll_return = poll(clientfds, nfds + 1, timeout);

		for (int i = 0; i < nfds; i++)
		{
			printf("i: %d\n", i);
			if (clientfds[i].revents & (POLLIN | POLLOUT))
			{
				printf("Client %d has data\n", clientfds[i].fd);
				ssize_t valread = read(clientfds[i].fd, buffer, MAX_BUFFER_SIZE);
				printf(CYAN"Client Request:"RESET"\n%s\n", buffer);
				char *response = "HTTP/1.1 200 OK\nRequest status code : 200 OK\n\n<h1>RESPONSE OK</h1>";
				size_t response_len = strlen(response);
				write(clientfds[i].fd, response, response_len);
				memset(buffer, 0, MAX_BUFFER_SIZE);
			}
		}

		// Close timed out clients
		for (int i = 0; i < nfds; i++)
		{
			if (clientfds[i].revents & POLLHUP)
			{
				printf("Client %d has timed out\n", clientfds[i].fd);
				close(clientfds[i].fd);
				clientfds[i].fd = -1;
				nfds--;
			}
		}

		if (poll_return < 0)
		{
			perror(RED"Poll failed"RESET);
			exit(EXIT_FAILURE);
		}

		// Check for new connections
		new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
		if (new_socket > 0)
		{
			clientfds[nfds].fd = new_socket;
			clientfds[nfds].events = POLLIN;

			// set client socket to non-blocking
			fcntl(new_socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
			
			nfds++;
			
			printf("Connection established!\n\n");
			printf("Fd: %d\n", new_socket);
			printf("Connections: %d\n", nfds);
		}
	}

	return (0);
}


// if the client has closed the connection, the writing back to the client fd will fail. We might need to check for that.