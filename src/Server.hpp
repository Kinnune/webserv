
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
#include <map>
#include <vector>

#include "Client.hpp"

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"


class Client;

class Server
{
	public:
		Server();
		~Server();
		int parseConfigFile(std::string configFile);
		void setPorts(std::vector<int> ports);
		void startListen();
		void newClient(int i);
		void removeClient(int fd);
		void loop();
		unsigned int getNfds();
	private:
		std::map<int, Client> _clients;
		static const int _maxClients = 1024;
		std::vector<int> _ports[1];
		std::vector<struct sockaddr_in> _addresses[1];
		struct pollfd _pollFds[_maxClients];
		nfds_t _nServers;
		nfds_t _nClients;
};

#endif
