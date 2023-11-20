
#include "Server.hpp"

Server::Server()
	: _port(0),
	_nfds(1)
{
	std::memset(&_pollFds[0], 0, sizeof(_pollFds));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
}

void Server::setPort(int port)
{
	_port = port;
	_address.sin_port = htons(_port);
}

void Server::setServerPoll()
{
	_pollFds[0].events = POLLIN;
	_pollFds[0].fd = _fd;
	_pollFds[0].revents = 0;
}

void Server::startListen(int port)
{
	setPort(port);
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == 0)
	{
		std::cerr << RED << "Socket creation failed" << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
	fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		std::cerr << RED << "Bind failed" << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Server address: " << GREEN << inet_ntoa(_address.sin_addr) << RESET << std::endl;
	if (listen(_fd, _maxClients) < 0)
	{
		std::cerr << RED << "Listen failed" << RESET;
		exit(EXIT_FAILURE);
	}
	setServerPoll();
	std::cout << "Server listening on port " << GREEN << _port << RESET << std::endl;
}

void Server::newClient()
{
	Client newClient(_fd, _port);

	_clients.insert(std::make_pair(newClient.getFd(), newClient));
	_pollFds[_nfds].fd = newClient.getFd();
	_pollFds[_nfds].events = (POLLIN | POLLOUT);
	_nfds++;
}

void Server::removeClient(int fd)
{
	unsigned int i;
	std::cout << _clients.size() << "CLIENTS" << std::endl;
	_clients.erase(fd);
	std::cout << "CLIENT " << fd << "REMOVED" << std::endl;
	std::cout << _clients.size() << "CLIENTS" << std::endl;

	for (i = 0; i < _nfds; i++)
	{
		if (_pollFds[i].fd == fd)
			break ;
	}
	std::cout << _pollFds[i].events << " "  << _pollFds[i].fd << " " << _pollFds[i].revents << std::endl;
	_pollFds[i] = _pollFds[_nfds - 1];
	_pollFds[_nfds - 1] = (struct pollfd){};
	std::cout << _pollFds[i].events << " "  << _pollFds[i].fd << " " << _pollFds[i].revents << std::endl;
}

void Server::loop()
{
	int timeout = 1 * 1000;

	while (true)
	{
		std::cout << "Polling..." << std::endl;
		usleep(1000000);
		poll(_pollFds, _nfds, timeout);
		for (unsigned int i = 0; i < _nfds; i++)
		{
			if (_pollFds[i].fd == _fd && _pollFds[i].revents & POLLIN)
			{
				newClient();
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



				// std::cout << "YEA NEW CLIENT" << std::endl;
				//  Make new client object
				// newClient = accept(_fd, (struct sockaddr *)&_address, &adressSize);
				//  Handle client read
				// std::cout << "Client" <<  _pollFds[i].fd << "has data\n";
				// read(_pollFds[i].fd, buffer, MAX_BUFFER_SIZE);
				// std::cout << CYAN << "Client Request:" << RESET << "\n" << buffer << "\n";
				// std::string response("HTTP/1.1 200 OK\nRequest status code : 200 OK\nContent-Length : 20 OK\nContent-Type: text/html; charset=utf-8\n\n<h1>RESPONSE OK</h1>");
				// write(_pollFds[i].fd, response.c_str(), response.length());
				// memset(buffer, 0, MAX_BUFFER_SIZE);
		// if (newClient > 0)
		// {
		// 	std::cout << "new client" << std::endl;
		// 	_pollFds[_nfds].fd = newClient;
		// 	_pollFds[_nfds].events = POLLIN;
		// 	fcntl(newClient, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		// 	_nfds++;
			//--alternatively
			//Client client(newClient);
			//_clients.addback(client);
		// }
