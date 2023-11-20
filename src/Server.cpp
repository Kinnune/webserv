
#include "Server.hpp"

Server::Server()
	: _port(0),
	_nfds(0)
{
	std::memset(&_clientFds[0], 0, sizeof(_clientFds));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
	(void)_nfds;
}

void Server::setPort(int port)
{
	_port = port;
	_address.sin_port = htons(_port);
}

void Server::startListen(int port)
{
	setPort(port);
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == 0)
	{
		std::cerr << RED << "Socket creation failed" << RESET;
		exit(EXIT_FAILURE);
	}
	fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		std::cerr << RED << "Bind failed\n" << RESET;
		exit(EXIT_FAILURE);
	}
	std::cout << "Server address: " << GREEN << inet_ntoa(_address.sin_addr) << "\n" << RESET;
	if (listen(_fd, _maxClients) < 0)
	{
		std::cerr << RED << "Listen failed" << RESET;
		exit(EXIT_FAILURE);
	}
	std::cout << "Server listening on port " << GREEN << _port << RESET << "\n";
}

void Server::loop()
{
	int newClient;
	int MAX_BUFFER_SIZE = 1024;
	char buffer[MAX_BUFFER_SIZE];
	socklen_t adressSize = sizeof(_address);
	int timeout = 1 * 1000;

	while (true)
	{
		printf("Polling...\n");
		usleep(1000000);
		newClient = accept(_fd, (struct sockaddr *)&_address, &adressSize);
		if (newClient > 0)
		{
			std::cout << "new client" << std::endl;
			_clientFds[_nfds].fd = newClient;
			_clientFds[_nfds].events = POLLIN;
			fcntl(newClient, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
			_nfds++;
			//--alternatively
			//Client client(newClient);
			//_clients.addback(client);
		}
		poll(_clientFds, _nfds + 1, timeout);
		for (unsigned int i = 0; i < _nfds; i++)
		{
			if (_clientFds[i].revents & (POLLIN | POLLOUT))
			{
				std::cout << "Client" <<  _clientFds[i].fd << "has data\n";
				read(_clientFds[i].fd, buffer, MAX_BUFFER_SIZE);
				std::cout << CYAN << "Client Request:" << RESET << "\n" << buffer << "\n";
				std::string response("HTTP/1.1 200 OK\nRequest status code : 200 OK\nContent-Length : 20 OK\n\n<h1>RESPONSE OK</h1>");
				write(_clientFds[i].fd, response.c_str(), response.length());
				memset(buffer, 0, MAX_BUFFER_SIZE);
			}
		}
	}
}
