
main()
{
	// Create server instance
	Server &server = Server::getInstance()
	{
		return (static Server instance);
	}

	// Create _config member variable
	server.initialize(av[1])
	{
		_config = ConfigurationFile config(configFile);
	}

	// Parse configuration file
	server.readConfig()
	{
		_config.setHosts();
	}

	// Set ports for server (from configuration file)
	server.setPorts()
	{
		for (unsigned int i = 0; i < _nServers; i++)
		{
			_ports.push_back(hosts.at(i).getPortInt());
			address.sin_port = htons(_ports.at(i));
			_addresses.push_back(address);
			_pollFds[i].events = POLLIN;
			_pollFds[i].fd = 0;
			_pollFds[i].revents = 0;
		}
	}

	// Start listening on ports
	server.startListen()
	{
		for (unsigned int i = 0; i < _ports.size(); i++)
		{
			_pollFds[i].fd = socket(AF_INET, SOCK_STREAM, 0);
			int enable = 1;
			setsockopt(_pollFds[i].fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
			if (_pollFds[i].fd == 0) { exit(1); }
			fcntl(_pollFds[i].fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
			bind(_pollFds[i].fd, (struct sockaddr *)&_addresses[i], sizeof(_addresses[i]));
			listen(_pollFds[i].fd, _MaxClients)
		}
	}

	server.loop()
	{
		while (1)
		{
			// Poll for incoming connections
			poll(_pollFds, _nServers, 1000);

			// Handle incoming connections
			for (unsigned int i = 0; i < getNfds(); i++)
			{
				// If pipe_fd, go to next iteration
				if (i >= _nServers + _nClients) { continue; }

				// If POLLIN event on server_fd, accept incoming connection
				if (i < _nServers && (_pollFds[i].revents & POLLIN))
				{
					newClient(i)
					{
						Client newClient(_pollFds[i].fd, _ports.at(i), _config);
						_clients.insert(std::make_pair(newClient.getFd(), newClient));
					}
					_didIO = _pollFds[i].fd;
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
}
