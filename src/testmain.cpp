
#include "Client.hpp"


std::ostream &operator<<(std::ostream &o, Buffer &buff)
{
	for (unsigned int i = 0; i < buff.getSize(); i++)
	{
		o << *(buff.getBegin() + i);
	}
	return(o);
}

int main()
{
	char *request = "this is just a test\r\n\r\nahat im testing here\r\n\r\nok?";
	char *request2 = "and another test\r\n\r\n";
	Buffer buffer;
	std::string requestStr;

	buffer.addToBuffer(request, std::strlen(request));
	buffer.addToBuffer(request2, std::strlen(request2));
	std::string testStr(buffer.getBegin(), buffer.getSize());
	std::cout << CYAN << buffer << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	try
	{
		requestStr = buffer.spliceRequest();
	}
	catch (std::exception &e)
	{
		std::cout << e.what();
		requestStr = "";
	}
	std::cout << GREEN << requestStr << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << CYAN << buffer << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << CYAN << buffer << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	try
	{
		requestStr = buffer.spliceRequest();
	}
	catch (std::exception &e)
	{
		std::cout << e.what();
		requestStr = "";
	}
	std::cout << GREEN << requestStr << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << CYAN << buffer << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << CYAN << buffer << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	try
	{
		requestStr = buffer.spliceRequest();
	}
	catch (std::exception &e)
	{
		std::cout << e.what();
		requestStr = "";
	}
	std::cout << GREEN << requestStr << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << CYAN << buffer << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << CYAN << buffer << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	try
	{
		requestStr = buffer.spliceRequest();
	}
	catch (std::exception &e)
	{
		std::cout << e.what();
		requestStr = "";
	}
	std::cout << GREEN << requestStr << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << CYAN << buffer << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << "------------------------------------------" << std::endl;
	std::cout << CYAN << buffer << RESET << std::endl;
	std::cout << "------------------------------------------" << std::endl;

	return (0);
}