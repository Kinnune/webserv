#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdio.h>
#include <unordered_map>
#include <dirent.h>
#include <fstream>
#include <unistd.h>
#include <ctime>

#include <cstring>
#include "Server.hpp"
#include "ConfigurationFile.hpp"
#include "Request.hpp"
#include "Buffer.hpp"
#include "Response.hpp"

class Response;
class Request;

std::ostream &operator<<(std::ostream &o, std::vector<unsigned char>data);
std::ostream &operator<<(std::ostream &o, Response response);

class Client
{
	private:
		// bool isFile(const std::string &path);
		// bool isDirectory(const std::string &path);
		// bool locationExists(const std::string &path);
		// bool allowedMethod(std::vector<std::string> methods, std::string method);
		// void updateResourcePath();
		// void handleLocation(Location &loc);
		// void handleNoLocation();
		// void updateAutoIndex(autoIndexState state);
		// void lookForIndexFile();
		// std::string _resourcePath;
		int _statusCode;
		autoIndexState _autoIndex;
		int _fd;
		int _port;
		struct sockaddr_in _address;
		Buffer _buffer;
		Request _request;
		Response _response;
		std::time_t _timeout;
		ConfigurationFile _config;

	public:
		
		// Constructors/Destructors
		Client();
		~Client();
		Client(Client const &other);
		Client &operator=(Client const &other);
		Client(int fd, int port, ConfigurationFile &config);

		// Getters
		int getFd() const;
		int getPort() const;
		// Host &getHost();

		// Setters
		void setFd(int fd);
		void setPort(int port);
		// void setHost(Host &host);
		
		// Methods
		bool respond();
		std::string listDirectory(std::string path);
		void handleEvent(short events);
};


#endif
