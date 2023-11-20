
#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <list>

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"

class Server
{
	public:
		Server();
		void setPort(int port);
		void startListen(int port);
		void loop();
	private:
		// std::list<Client &> _clients;
		static const int _maxClients = 1024;
		int _port;
		int _fd;
		struct sockaddr_in _address;
		struct pollfd _clientFds[_maxClients];
		nfds_t _nfds;

};

#endif
