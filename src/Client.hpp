#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <stdio.h>
#include "Server.hpp"

class Client
{
	public:
		Client();
		~Client();
		Client(Client const &other);
		Client &operator=(Client const &other);
		Client(int fd, int port);
		void setFd(int fd);
		void setPort(int port);
		int getFd() const;
		int getPort() const;
		void handleEvent(short events);
	private:
		int _fd;
		int _port;
		struct sockaddr_in _address;
};

#endif
