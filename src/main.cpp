
#include "Server.hpp"

int main(int ac, char **av)
{
	Server server;

	if (ac != 2)
	{
		std::cout << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}

	server.parseConfigFile(av[1]);
	std::vector<int> ports;
	ports.push_back(8080);
	ports.push_back(666);

	server.setPorts(ports);
	server.startListen();
	server.loop();
}
