
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
#include "ConfigurationFile.hpp"

class Client;

class Server
{
	public:
		Server();
		~Server();
		void initialize(std::string configFile);
		void readConfig();
		void setPorts(std::vector<int> ports);
		void startListen();
		void newClient(int i);
		void removeClient(int fd);
		void loop();
		unsigned int getNfds();
	private:
		ConfigurationFile _config;
		std::map<int, Client> _clients;
		static const int _maxClients = 1024;
		std::vector<int> _ports[1];
		std::vector<struct sockaddr_in> _addresses[1];
		struct pollfd _pollFds[_maxClients];
		nfds_t _nServers;
		nfds_t _nClients;
};

#endif
