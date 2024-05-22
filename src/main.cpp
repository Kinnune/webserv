
#include "Server.hpp"
#include "utilityHeader.hpp"


int main(int ac, char **av)
{
	Server &server = Server::getInstance();

	if (ac != 2)
	{
		std::cout << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}

	server.initialize(av[1]);
	if (!server.readConfig())
		return 1;

	server.setPorts();
	server.startListen();
	server.loop();
	return 0;
}
