#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdio.h>
#include <unordered_map>
#include <dirent.h>
#include <fstream>
#include <unistd.h>
#include <ctime>
#include <random>
#include <chrono>
#include <sstream>

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
	// Constructors/Destructors
	Client();
	~Client();
	Client(Client const &other);
	Client &operator=(Client const &other);
	Client(int fd, int port, ConfigurationFile &config);
	// Getters
	int getFd() const;
	int getPort() const;
	short getFailFlag(void);
	Response &getResponse();
	std::string getSessionId();
	// Setters
	void setFd(int fd);
	void setPort(int port);
	void setFailFlag(short flag);
	// Methods
	bool respond();
	void handleEvent(short events);
	bool checkTimeout(time_t currentTime);
	void setSessionID();
	std::string generateSessionId();
private:
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
	short _failFlag;
	std::string _sessionID;

};


#endif
