#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <stdio.h>
#include "Server.hpp"

class Client
{
	public:
		Client();
		~Client();
		void setFd(int fd);
		void setPort(int port);
		void setServer(Server &server);
		int getFd() const;
		int getPort() const;
		Server &getServer() const;
	private:
		int _fd;
		int _port;
		Server &_server;
};

#endif