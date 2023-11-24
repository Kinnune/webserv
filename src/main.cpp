
#include "Server.hpp"

int main(int ac, char **av)
{
	Server server;
	(void)av;

	if (ac != 2)
	{
		std::cout << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}

	server.initialize(av[1]);
	server.readConfig();
	std::vector<int> ports;
	ports.push_back(8080);
	ports.push_back(666);

	server.setPorts(ports);
	server.startListen();
	server.loop();
}
