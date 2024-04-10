//------------------------------------------------------------------------------
//	LIST OF FUNCTIONS
//------------------------------------------------------------------------------


//----SOCKET--------------------------------------------------------------------

/*
	int socket(int domain, int type, int protocol);

	- Creates a socket
	- Returns a file descriptor that can be used in other socket functions
*/

#include <sys/socket.h>

int socket_fd = socket(AF_INET, SOCK_STREAM, 0);	// Create a socket

/*	
	AF_INET		= Address Family - Internet
					- means that the socket can be used to communicate over the internet
	SOCK_STREAM	= Socket Type - Stream
					- means that the socket is a stream socket (for data transferring)
					- can also be used to create a datagram socket by using SOCK_DGRAM (for one time data packet transfer)
	0			= TCP Protocol
					- the most common protocol used for data transfer
*/


//----BIND----------------------------------------------------------------------

/*
	int bind(int socket_fd, const struct sockaddr *addr, socklen_t addrlen);

	- Binds the socket to the address and port number specified in the sockaddr structure
	- This is necessary for the server to listen on a specific port
	- Returns 0 on success, -1 on failure (and sets errno if there is an error)
*/

#include <sys/socket.h>
#define PORT 8080

struct sockaddr_in server_address;

server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = INADDR_ANY;
server_address.sin_port = htons(PORT);

if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0)
{
	perror("Invalid address/Address not supported");
	exit(EXIT_FAILURE);
}

if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
{
	perror("Bind failed");
	exit(EXIT_FAILURE);
}

/*
	INADDR_ANY		= every address (we're setting the server to listen on every available network interface)
	htons()			= host to network short (converts the port number to network byte order)
	socket_fd		= socket file descriptor
	server_address	= server address structure
	addrlen			= size of the server address structure
*/


//----LISTEN--------------------------------------------------------------------

/*
	int listen(int socket_fd, int backlog);

	- Listens for incoming connections on the socket
	- Returns 0 on success, -1 on failure (and sets errno if there is an error)
*/

#include <sys/socket.h>

if (listen(socket_fd, 3) < 0)
{
	perror("Listen failed");
	exit(EXIT_FAILURE);
}

/*
	socket_fd	= socket file descriptor
	backlog		= maximum number of pending connections that can be queued up
*/


//----ACCEPT--------------------------------------------------------------------

/*
	int accept(int socket_fd, struct sockaddr *addr, socklen_t *addrlen);

	- Accepts a new incoming connection on the socket
	- Returns a new file descriptor for the accepted connection
	- addr and addrlen are used to store the address of the client
*/

#include <sys/socket.h>

struct sockaddr_in client_address;
int connection_fd;

if ((connection_fd = accept(socket_fd, (struct sockaddr*)&client_address, (socklen_t*)&addrlen)) < 0)
{
	perror("Accept failed");
	exit(EXIT_FAILURE);
}

/*
	socket_fd		= socket file descriptor
	client_address	= client address structure
	addrlen			= size of the client address structure
*/