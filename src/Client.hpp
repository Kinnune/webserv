#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdio.h>
#include <unordered_map>
#include "Server.hpp"
#include "ConfigurationFile.hpp"
#include "Request.hpp"
#include "Buffer.hpp"

class Request;

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
		void handleEvent(short events);
		void setConfig(ConfigurationFile &config);
		ConfigurationFile &getConfig();
	private:
		ConfigurationFile _config;
		bool fileExists(const std::string &path);
		bool isFile(const std::string &path);
		bool isDirectory(const std::string &path);
		void updateResourcePath();
		std::string _resourcePath;
		int _fd;
		int _port;
		struct sockaddr_in _address;
		Buffer _buffer;
		// Request &_request;
		Request _request;
};


#endif
