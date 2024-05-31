
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
	static Server& getInstance();
	void initialize(std::string configFile);
	int readConfig();
	short getEventsByFd(int fd);
	void setPorts();
	void startListen();
	void newClient(int i);
	struct pollfd &newFd(int fd);
	void removeFd(int fd);
	void removeClient(int fd);
	void loop();
	void setDidIO(int flag);
	int	 getDidIO();
	void rotateClients(int clientFd);
	unsigned int getNfds();
private:
	Server();
	~Server();
	Server(const Server &other);
	Server &operator=(const Server &other);
	ConfigurationFile _config;
	std::map<int, Client> _clients;
	static const int _maxClients = 1024;
	std::vector<int> _ports;
	std::vector<struct sockaddr_in> _addresses;
	struct pollfd _pollFds[_maxClients];
	nfds_t _nServers;
	nfds_t _nClients;
	nfds_t _nOtherFd;
	int _didIO;
};

#endif
