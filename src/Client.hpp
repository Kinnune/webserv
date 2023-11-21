#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdio.h>
#include "Server.hpp"

class Buffer
{
	public:
		Buffer() : _endLiteral("\r\n\r\n") {};
		char *requestEnded()
		{
			std::vector<char>::iterator it;

			if (_data.size() < _endLiteral.length())
				return (NULL);
			for (it = _data.begin(); it != _data.end() - (_endLiteral.length() - 1); it++)
			{
				char *position = &(*it);
				if (std::strncmp(position, _endLiteral.c_str(), 4) == 0)
				{
					return (&(*(it + 4)));
				}
			}
			return (NULL);
		}
		void addToBuffer(char *data, size_t size) { _data.insert(_data.end(), data, data + size); };
		std::string spliceRequest()
		{
			char *endPos;
			size_t requestSize;
			std::string request;

			endPos = requestEnded();
			if (!endPos)
			{
				throw(std::runtime_error("Exception: no request to get"));
			}
			requestSize = endPos - getBegin();
			request = std::string(_data.begin(), _data.begin() + requestSize);
			_data.erase(_data.begin(), _data.begin() + requestSize);
			return (request);
		}
		size_t getSize() { return(_data.size()); };
		char *getBegin() {return(&_data[0]);}
	private:
		std::string _endLiteral;
		// std::string _buffer;
		size_t _contentLenght;
		bool _chunked;
		std::vector<char> _data;
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
		std::string _request;
};

#endif
