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
		Response &getResponse() { return (_response); }
		std::string getSessionId() { return (_sessionID); }
		// Host &getHost();

		// Setters
		void setFd(int fd);
		void setPort(int port);
		void setFailFlag(short flag);
		// void setHost(Host &host);
		
		// Methods
		bool respond();
		std::string listDirectory(std::string path);
		void handleEvent(short events);
		bool checkTimeout(time_t currentTime);
		void setSessionID();

		std::string generateSessionId() {
		    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		    std::chrono::system_clock::duration duration = now.time_since_epoch();
		    long long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

		    std::random_device rd;
		    std::mt19937 gen(rd());
		    std::uniform_int_distribution<> dis(0, 9999);
		    int randomNumber = dis(gen);
		    std::stringstream ss;
		    ss << milliseconds << '-' << randomNumber;
		    return ss.str();
		}
};


#endif
