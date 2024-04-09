
#include "Server.hpp"

int main(int ac, char **av)
{
	Server server;

	if (ac != 2)
	{
		std::cout << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}

	server.initialize(av[1]);
	if (!server.readConfig())
		return 1;
	// std::vector<int> ports;
	// ports.push_back(8080);
	// ports.push_back(666);

	server.setPorts();
	server.startListen();
	server.loop();
	return 0;
}
