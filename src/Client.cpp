#include "Client.hpp"
#include "Colors.hpp"
#include <iostream>
#include <fstream>

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
	// int MAX_BUFFER_SIZE = 1024;
	// char buffer[MAX_BUFFER_SIZE];
	int MAX_BUFFER_SIZE = 16384;
	char buffer[MAX_BUFFER_SIZE];
	std::ifstream src;

	if (events & POLLIN)
	{
		read(_fd, buffer, MAX_BUFFER_SIZE);
		std::cout << CYAN << "Client Request:" << RESET << "\n" << buffer << "\n";
		memset(buffer, 0, MAX_BUFFER_SIZE);
		src.open("www/index.html", std::ofstream::in);
		if (!src.good())
		{
			std::cerr << "Open failed" << std::endl; 
			return ;
		}
		src.read(buffer, MAX_BUFFER_SIZE);	
		int streamSize = src.gcount();
		buffer[streamSize] = '\0';
		std::string response("HTTP/1.1 200 OK\nRequest status code: 200 OK\nContent-Length: " + std::to_string(streamSize) +"\nContent-Type: text/html\n\n");
		response.append(buffer, streamSize);
		write(_fd, response.c_str(), response.length());
		memset(buffer, 0, MAX_BUFFER_SIZE);

		std::cout << "\n{\n" << response << "\n}\n" << std::endl;

		// std::cout << "client handling POLLIN" << std::endl;
		// std::string response("HTTP/1.1 200 OK\nRequest status code : 200 OK\nContent-Length : 20 OK\nContent-Type: text/html; charset=utf-8\n\n<h1>RESPONSE OK</h1>");
		// write(_fd, response.c_str(), response.length());
	}
}