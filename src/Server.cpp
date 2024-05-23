
#include "Server.hpp"
#include "Colors.hpp"
#include <iostream>
#include <fstream>

Server& Server::getInstance()
{
	static Server instance;
	return instance;
}

Server::Server() : _nServers(0), _nClients(0), _nOtherFd(0), _didIO(0)
{
	std::memset(&_pollFds[0], 0, sizeof(_pollFds));
}

Server::~Server() {}

Server::Server(const Server &other) { (void)other; }

Server &Server::operator=(const Server &other) { (void)other; return(*this); }

void Server::initialize(std::string configFile)
{
	ConfigurationFile config(configFile);
	_config = config;
}

int Server::readConfig()
{
	return (_config.parse());
}

void Server::setPorts()
{
	struct sockaddr_in address;
	std::vector<Host> hosts;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;	// INADDR_ANY = we're responding to any address

	hosts = _config.getHosts();
	_nServers = hosts.size();

	for (unsigned int i = 0; i < _nServers; i++)
	{
		_ports.push_back(hosts.at(i).getPortInt());
		address.sin_port = htons(_ports.at(i));	// htons = host to network, short (converts port number to network byte order, which is big-endian)
		_addresses.push_back(address);
		_pollFds[i].events = POLLIN;
		_pollFds[i].fd = 0;
		_pollFds[i].revents = 0;
	}
}

void Server::startListen()
{
	for (unsigned int i = 0; i < _ports.size(); i++)
	{

		_pollFds[i].fd = socket(AF_INET, SOCK_STREAM, 0);
		
		//  fixes having to wait for bind after restart
		int enable = 1;
		setsockopt(_pollFds[i].fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

		if (_pollFds[i].fd == 0)
		{
			std::cerr << RED << "Socket creation failed" << RESET << std::endl;
			exit(EXIT_FAILURE);
		}
		fcntl(_pollFds[i].fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		// bound socket must be deleted by the caller when it is no longer needed (using unlink(2)).
	 	if (bind(_pollFds[i].fd, (struct sockaddr *)&_addresses.at(i), sizeof(struct sockaddr_in)) < 0)
		{
			std::cerr << RED << "Bind failed" << RESET << std::endl;
			exit(EXIT_FAILURE);
		}
		std::cout << "Server address: " << GREEN << inet_ntoa(_addresses.at(i).sin_addr) << RESET << std::endl;
		if (listen(_pollFds[i].fd, _maxClients) < 0)
		{
			std::cerr << RED << "Listen failed" << RESET;
			exit(EXIT_FAILURE);
		}
		std::cout << "Server listening on port " << GREEN << _ports.at(i) << RESET << std::endl;
	}
}

void Server::setDidIO(int flag) { _didIO = flag;}

int Server::getDidIO() { return(_didIO); }

void Server::newClient(int i)
{
	Client newClient(_pollFds[i].fd, _ports.at(i), _config);
	_clients.insert(std::make_pair(newClient.getFd(), newClient));
	_pollFds[getNfds()].fd = newClient.getFd();
	_pollFds[getNfds()].events = (POLLIN | POLLOUT);
	_pollFds[getNfds()].revents = 0;
	_nClients++;
}

struct pollfd &Server::newFd(int fd)
{
	unsigned int nFds = getNfds();

	std::cout << "Setting new fd: " << fd << std::endl;
	_pollFds[nFds].fd = fd;
	_pollFds[nFds].events = (POLLIN | POLLOUT);
	_pollFds[nFds].revents = 0;
	_nOtherFd++;
	return (_pollFds[nFds]);
}


void Server::removeFd(int fd)
{
	unsigned int i;

	close(fd);
	for (i = 0; i < getNfds(); i++)
	{
		if (_pollFds[i].fd == fd)
		{
			break ;
		}
	}
	for (; i < getNfds() - 1; i++)
	{
		_pollFds[i] = _pollFds[i + 1];
	}
	_nOtherFd--;
	_pollFds[getNfds()] = (struct pollfd){};
}

void Server::removeClient(int fd)
{
	unsigned int i;

	std::cout << color("Client disconnected " + std::to_string(_clients[fd].getFailFlag()), RED) << std::endl;
	close(fd);
	_clients[fd].getResponse().killChild();
	_clients.erase(fd);
	for (i = _nServers; i < getNfds(); i++)
	{
		if (_pollFds[i].fd == fd)
		{
			break ;
		}
	}
	for (; i < getNfds() - 1; i++)
	{
		_pollFds[i] = _pollFds[i + 1];
	}
	_pollFds[getNfds() - 1] = (struct pollfd){};
	_nClients--;
}

short Server::getEventsByFd(int fd)
{
	unsigned int i;

	for (i = 0; i < getNfds(); i++)
	{
		if (_pollFds[i].fd == fd)
		{
			return (_pollFds[i].revents);
		}
	}
	return (-1);
}

void Server::rotateClients(int clientFd)
{
	unsigned int i;
	pollfd tmpClient;

	tmpClient.fd = -1;
	for (i = _nServers; i < _nServers + _nClients - 1; i++)
	{
		if (_pollFds[i].fd == clientFd)
		{
			tmpClient = _pollFds[i];
		}
		if (tmpClient.fd != -1)
		{
			_pollFds[i] = _pollFds[i + 1];
		}
	}
	if (tmpClient.fd != -1)
	{
		_pollFds[i] = tmpClient;
	}
}

// set didio for pipe operations
// check that everything behaves as expected
void Server::loop()
{
	int timeout = 1 * 1000;
	time_t currentTime;

	while (true)
	{
		_didIO = 0;
		poll(_pollFds, getNfds(), timeout);
		currentTime = std::time(nullptr);
		// usleep(100000);
		// std::cout << "Polling again..." << std::endl;
		for (unsigned int i = 0; i < getNfds(); i++)
		{
			if (i >= _nServers + _nClients)
			{
				continue ;
			}
			if (i < _nServers)
			{
				if (_pollFds[i].revents & POLLIN)
				{
					newClient(i);
					_didIO = _pollFds[i].fd;
					i = getNfds();
				}
			}
			else if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL) || _clients[_pollFds[i].fd].checkTimeout(currentTime))
			{
				removeClient(_pollFds[i].fd);
			}
			else if (_pollFds[i].revents)
			{
				_clients[_pollFds[i].fd].handleEvent(_pollFds[i].revents);
				if (_clients[_pollFds[i].fd].getFailFlag())
				{
					removeClient(_pollFds[i].fd);
				}
				// if client fails to read or write in handleEvent -> remove client
			}
			if (_didIO)
			{
				rotateClients(_didIO);
				i = getNfds();
			}
		}
	}
}

unsigned int Server::getNfds()
{
	return (_nServers + _nClients + _nOtherFd);
}
