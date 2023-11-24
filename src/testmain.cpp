
#include "Client.hpp"
#include "Request.hpp"

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
	std::string request("GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:95.0) Gecko/20100101 Firefox/95.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nConnection: keep-alive");
	char *request2 = "and another test\r\n\r\n";
	Buffer buffer;
	std::string requestStr;
	std::vector<unsigned char> requestVector(request.begin(), request.end());
	Request test(requestVector);

	// buffer.addToBuffer(request, std::strlen(request));
	// buffer.addToBuffer(request2, std::strlen(request2));
	// std::string testStr((char *)buffer.getBegin(), buffer.getSize());
	// std::cout << CYAN << buffer << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// try
	// {
	// 	requestStr = buffer.spliceRequest();
	// }
	// catch (std::exception &e)
	// {
	// 	std::cout << e.what();
	// 	requestStr = "";
	// }
	// std::cout << GREEN << requestStr << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << CYAN << buffer << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << CYAN << buffer << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// try
	// {
	// 	requestStr = buffer.spliceRequest();
	// }
	// catch (std::exception &e)
	// {
	// 	std::cout << e.what();
	// 	requestStr = "";
	// }
	// std::cout << GREEN << requestStr << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << CYAN << buffer << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << CYAN << buffer << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// try
	// {
	// 	requestStr = buffer.spliceRequest();
	// }
	// catch (std::exception &e)
	// {
	// 	std::cout << e.what();
	// 	requestStr = "";
	// }
	// std::cout << GREEN << requestStr << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << CYAN << buffer << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << CYAN << buffer << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// try
	// {
	// 	requestStr = buffer.spliceRequest();
	// }
	// catch (std::exception &e)
	// {
	// 	std::cout << e.what();
	// 	requestStr = "";
	// }
	// std::cout << GREEN << requestStr << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << CYAN << buffer << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << "------------------------------------------" << std::endl;
	// std::cout << CYAN << buffer << RESET << std::endl;
	// std::cout << "------------------------------------------" << std::endl;

	return (0);
}