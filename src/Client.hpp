#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdio.h>
#include <unordered_map>
#include "Server.hpp"
#include "Request.hpp"

struct HttpRequest {
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

std::ostream &operator<<(std::ostream &o, std::vector<unsigned char>data);
class Buffer
{
	public:
		Buffer() : _endLiteral("\r\n\r\n") {};
		unsigned char *requestEnded()
		{
			std::vector<unsigned char>::iterator it;
			unsigned char *position;

			// std::find_if(_data.begin(), _data.end(), std::vector<unsigned char>(_endLiteral.begin(), _endLiteral.end()));

			if (_data.size() < _endLiteral.length())
				return (NULL);
			for (it = _data.begin(); it != _data.end() - (_endLiteral.length() - 1); it++)
			{
				position = &(*it);
				// std::cout << "SEGF" << _data.end() - it << " " << _endLiteral.length()<< std::vector<unsigned char>(it, _data.end()) << std::endl;
				if (std::strncmp((char *)position, _endLiteral.c_str(), _endLiteral.length()) == 0)
				{
					return (&(*(it + _endLiteral.length())));
				}
			}
			return (NULL);
		}
		void addToBuffer(char *data, size_t size) { _data.insert(_data.end(), data, data + size); };
		std::vector<unsigned char> spliceRequest()
		{
			unsigned char *endPos;
			size_t requestSize;
			std::vector<unsigned char> request;

			endPos = requestEnded();
			if (!endPos)
			{
				throw(std::runtime_error("Exception: no request to get"));
			}
			requestSize = endPos - getBegin();
			request = std::vector<unsigned char>(_data.begin(), _data.begin() + requestSize);
			_data.erase(_data.begin(), _data.begin() + requestSize);
			return (request);
		}
		size_t getSize() { return(_data.size()); };
		unsigned char *getBegin() { return(&_data[0]); }
		std::vector<unsigned char>::iterator getEnd() { return(_data.end()); }
		std::vector<unsigned char> &getData() { return (_data); };
	private:
		std::string _endLiteral;
		std::vector<unsigned char> _data;
};

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
		void handleEvent(short events);
	private:
		int _fd;
		int _port;
		struct sockaddr_in _address;
		// std::string _request;
		Buffer _buffer;
		Request _request;
};


#endif
