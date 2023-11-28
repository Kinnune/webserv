
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

std::ostream &operator<<(std::ostream &o, std::vector<unsigned char>data)
{
	for ( std::vector<unsigned char>::iterator it = data.begin(); it < data.end(); it++)
	{
		o << *it;
	}
	return(o);
}

int main()
{
	std::string request("GET /index.html HTTP/1.1\t\nHost: www.example.com\t\nContent-Length: 123\t\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:95.0) Gecko/20100101 Firefox/95.0\t\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\t\nAccept-Language: en-US,en;q=0.5\t\nConnection: keep-alive\t\n\t\n");
	std::vector<unsigned char> requestVector(request.begin(), request.end());

	Request test2, test;
	try
	{
		test = Request(requestVector);
		test2 = test;
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	std::cout << "------------------------------------------" << std::endl;
	test.printRequest();
	std::cout << "------------------------------------------" << std::endl;


	return (0);
}
