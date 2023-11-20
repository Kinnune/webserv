
#include "Server.hpp"

int main()
{
	Server server;
	server.startListen(8080);
	server.loop();
}
