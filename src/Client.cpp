#include "Client.hpp"

Client::Client() {}

Client::~Client() {}

Client::Client(Client const &other)
{
	*this = other;
}

Client &Client::operator=(Client const &other)
{
	_fd = other._fd;
	_port = other._port;
	return (*this);
}

Client::Client(int serverFd, int port)
{
	socklen_t adressSize = sizeof(_address);

	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	_fd = accept(serverFd, (struct sockaddr *)&_address, &adressSize);
	fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

void Client::setFd(int fd)
{
	_fd = fd;
}

void Client::setPort(int port)
{
	_port = port;
}

int Client::getFd() const
{
	return (_fd);
}

int Client::getPort() const
{
	return (_port);
}

#include <fstream>


//read to buffer until request done;

void Client::handleEvent(short events)
{
	int MAX_BUFFER_SIZE = 64;
	char buffer[MAX_BUFFER_SIZE];
	size_t readCount = 0;
	std::ifstream src;

	if (events & POLLIN)
	{
		readCount = read(_fd, buffer, MAX_BUFFER_SIZE);
		if (readCount < 0)
		{
			throw (std::runtime_error("EXCEPTION: reading failed"));
		}
		buffer[readCount] = '\0';
		_buffer.addToBuffer(&buffer[0], readCount);
		// std::cout << buffer << std::endl;
		// std::cout << _buffer.getData() << std::endl;
		if (_buffer.requestEnded())
		{
			std::cout << GREEN << "double newline found" << RESET << std::endl;
			try
			{
				_request = Request(_buffer.spliceRequest());
			}
			catch (std::exception &e)
			{
				std::cerr << "EXCEPTION OCCURRED: " << e.what() << std::endl;
			}
			if (!_request.getIsValid())
			{
				//  respond something
				//  close connection
			}
			_request.printRequest();
			//* TODO
			//* check content length variable of request
			//* if needed keep reading
			//* if not
			//* 	make response
			//* 	_request.~Request()
			//* 	and check buffer for new requests


			// _request.printRequest();
		}
		else
		{
			std::cout << RED << "double newline NOT found" << RESET << std::endl;
		}
		// std::cout << CYAN << "Client Request:" << RESET << "\n" << buffer << "\n";
		// if (i == 0)
		// {
			// std::string response("HTTP/1.1 200 OK\nRequest status code: 200 OK\nContent-Length: 50 OK\nContent-Type: text/html; charset=utf-8\n\n<h1>RESPONSE OK</h1><img src=\"resources/zeus.jpg\">");
			// write(_fd, response.c_str(), response.length());
			// memset(buffer, 0, MAX_BUFFER_SIZE);
			// i++;
		// }
		// else
		// {
			// src.open("../resources/zeus.jpg", std::ofstream::binary | std::ofstream::in);
			// if (!src.good())
			// {
				// std::cerr << "Open failed" << std::endl; 
				// return ;
			// }
			// src.read(buffer, MAX_BUFFER_SIZE);	
			// int streamSize = src.gcount();
			// buffer[streamSize] = '\0';
			// std::string response("HTTP/1.1 200 OK\nRequest status code: 200 OK\nContent-Length: " + std::to_string(streamSize) +"\nContent-Type: image/jpeg\n\n");
			// response.append(buffer, streamSize);
			// write(_fd, response.c_str(), response.length());
			// memset(buffer, 0, MAX_BUFFER_SIZE);
// 
			// std::cout << "\n{\n" << response << "\n}\n" << std::endl;
// 
		// }
	}
}

std::ostream &operator<<(std::ostream &o, std::vector<unsigned char>data)
{
	o << "{";
	for (std::vector<unsigned char>::iterator it = data.begin(); it < data.end(); it++)
	{
		o << *it;
		// o << "'" << (int)(*it) << "'" << *it;
	}
	o << "}";
	return(o);
}
