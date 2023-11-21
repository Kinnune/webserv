#include "Client.hpp"

Client::Client() {}

Client::~Client() {}

Client::Client(Client const &other)
{
	*this = other;
}

Client &Client::operator=(Client const &other)
{
	_fd = other._fd;
	_port = other._port;
	return (*this);
}

Client::Client(int serverFd, int port)
{
	socklen_t adressSize = sizeof(_address);

	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	_fd = accept(serverFd, (struct sockaddr *)&_address, &adressSize);
	fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

void Client::setFd(int fd)
{
	_fd = fd;
}

void Client::setPort(int port)
{
	_port = port;
}

int Client::getFd() const
{
	return (_fd);
}

int Client::getPort() const
{
	return (_port);
}

void Client::handleEvent(short events)
{
	int MAX_BUFFER_SIZE = 1024;
	char buffer[MAX_BUFFER_SIZE];

	if (events & POLLIN)
	{
		std::cout << "client handling POLLIN" << std::endl;
		read(_fd, buffer, MAX_BUFFER_SIZE);
		std::cout << CYAN << "Client Request:" << RESET << "\n" << buffer << "\n";
		std::string response("HTTP/1.1 200 OK\nRequest status code : 200 OK\nContent-Length : 20 OK\nContent-Type: text/html; charset=utf-8\n\n<h1>RESPONSE OK</h1>");
		write(_fd, response.c_str(), response.length());
		memset(buffer, 0, MAX_BUFFER_SIZE);
	}
}
