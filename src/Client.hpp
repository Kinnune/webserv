#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdio.h>
#include <unordered_map>
#include <dirent.h>
#include <fstream>
#include <unistd.h>

#include "Server.hpp"
#include "Request.hpp"
#include "Buffer.hpp"

class Response;
class Request;

std::ostream &operator<<(std::ostream &o, Response response);

#define DEBUG 0

std::ostream &operator<<(std::ostream &o, std::vector<unsigned char>data);

class Client
{
	public:
		Client();
		~Client();
		Client(Client const &other);
		Client &operator=(Client const &other);
		Client(int fd, int port);
		void setFd(int fd);
		void setPort(int port);
		int getFd() const;
		int getPort() const;
		void respond();
		std::string  listDirectory(std::string path);
		void handleEvent(short events);
		// void respond();
	private:
		int _fd;
		int _port;
		struct sockaddr_in _address;
		Buffer _buffer;
		// Request &_request;
		Request _request;
};


#endif
