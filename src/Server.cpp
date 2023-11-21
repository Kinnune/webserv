
#include "Server.hpp"

Server::Server()
	: _nServers(0),
	_nClients(0)
{
	std::memset(&_pollFds[0], 0, sizeof(_pollFds));
}

void Server::setPorts(std::vector<int> ports)
{
	struct sockaddr_in address;

	_nServers = ports.size();
	for (unsigned int i = 0; i < _nServers; i++)
	{
		_ports->push_back(ports.at(i));
		address.sin_port = htons(_ports->at(i));
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		_addresses->push_back(address);
		_pollFds[i].events = POLLIN;
		_pollFds[i].fd = 0;
		_pollFds[i].revents = 0;
	}
}

void Server::startListen()
{
	for (unsigned int i = 0; i < _ports->size(); i++)
	{

		_pollFds[i].fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_pollFds[i].fd == 0)
		{
			std::cerr << RED << "Socket creation failed" << RESET << std::endl;
			exit(EXIT_FAILURE);
		}
		fcntl(_pollFds[i].fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		if (bind(_pollFds[i].fd, (struct sockaddr *)&_addresses->at(i), sizeof(struct sockaddr_in)) < 0)
		{
			std::cerr << RED << "Bind failed" << RESET << std::endl;
			exit(EXIT_FAILURE);
		}
		std::cout << "Server address: " << GREEN << inet_ntoa(_addresses->at(i).sin_addr) << RESET << std::endl;
		if (listen(_pollFds[i].fd, _maxClients) < 0)
		{
			std::cerr << RED << "Listen failed" << RESET;
			exit(EXIT_FAILURE);
		}
		std::cout << "Server listening on port " << GREEN << _ports->at(i) << RESET << std::endl;
	}
}

void Server::newClient(int i)
{
	Client newClient(_pollFds[i].fd, _ports->at(i));

	_clients.insert(std::make_pair(newClient.getFd(), newClient));
	_pollFds[getNfds()].fd = newClient.getFd();
	_pollFds[getNfds()].events = (POLLIN | POLLOUT);
	_nClients++;
}

void Server::removeClient(int fd)
{
	unsigned int i;

	_clients.erase(fd);
	for (i = _nServers; i < getNfds(); i++)
	{
		if (_pollFds[i].fd == fd)
			break ;
	}
	_pollFds[i] = _pollFds[getNfds() - 1];
	_pollFds[getNfds() - 1] = (struct pollfd){};
	_nClients--;
}

void Server::loop()
{
	int timeout = 1 * 1000;

	while (true)
	{
		std::cout << "Polling..." << std::endl;
		usleep(1000000);
		poll(_pollFds, getNfds(), timeout);
		for (unsigned int i = 0; i < getNfds(); i++)
		{
			if (i < _nServers)
			{
				if (_pollFds[i].revents & POLLIN)
				{
					newClient(i);
				}
			}
			else if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				removeClient(_pollFds[i].fd);
			}
			else if (_pollFds[i].revents)
			{
				_clients[_pollFds[i].fd].handleEvent(_pollFds[i].revents);
			}
		}
	}
}

unsigned int Server::getNfds()
{
	return (_nServers + _nClients);
}