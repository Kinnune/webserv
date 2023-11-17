#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>

int main()
{
	char name[255];
	int counter = 0;
	int timeout = 100;
	struct pollfd mypoll;


	memset(&mypoll, 0, sizeof(mypoll));
	mypoll.fd = 0;
	mypoll.events = POLLIN;

	printf("What is your name?\n");

	while (1)
	{
		poll(&mypoll, 1, timeout);
		if (mypoll.revents & POLLIN)
		{
			read(0, name, sizeof(name));
			printf("Hello, %s\n", name);
			break;
		}
		else
			counter++;
	}
	
	printf("You took %d milliseconds to respond.\n", counter * timeout);
	
	return (0);
}