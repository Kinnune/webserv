
#include "Server.hpp"

int main()
{
	Server server;
	std::vector<int> ports;
	ports.push_back(8080);
	ports.push_back(666);

	server.setPorts(ports);
	server.startListen();
	server.loop();
}
