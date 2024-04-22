#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdio.h>
#include <unordered_map>
#include <dirent.h>
#include <fstream>
#include <unistd.h>

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
		bool respond();
		std::string  listDirectory(std::string path);
		void handleEvent(short events);
		void setConfig(ConfigurationFile &config);
		ConfigurationFile &getConfig();
	private:
		ConfigurationFile _config;
		bool isFile(const std::string &path);
		bool isDirectory(const std::string &path);
		bool locationExists(const std::string &path);
		bool allowedMethod(std::vector<std::string> methods, std::string method);
		void updateResourcePath();
		void handleLocation(Host &host, Location &loc);
		void handleNoLocation(Host &host);
		void updateAutoIndex(autoIndexState state);
		void lookForIndexFile();
		std::string _resourcePath;
		int _statusCode;
		autoIndexState _autoIndex;
		int _fd;
		int _port;
		struct sockaddr_in _address;
		Buffer _buffer;
		Request _request;
		Response _response;
		Host _host;
};


#endif
