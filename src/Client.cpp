#include "Client.hpp"
#include "Colors.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>	// access()

Client::Client()
	: _request(Request())
{}

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

void Client::setConfig(ConfigurationFile &config)
{
	_config = config;
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


//read to buffer until request done;

void mockResponse(int fd)
{
	std::ifstream src;
	static const int MAX_BUFFER_SIZE = 100000;
	char bufferPic[MAX_BUFFER_SIZE + 1];

	src.open("../resources/zeus.jpg", std::ofstream::binary | std::ofstream::in);
	if (!src.good())
	{
		std::cerr << "Open failed" << std::endl; 
		return ;
	}
	src.read(bufferPic, 100000);
	int streamSize = src.gcount();
	bufferPic[streamSize] = '\0';
	std::string response("HTTP/1.1 200 OK\nRequest status code: 200 OK\nContent-Length: " + std::to_string(streamSize) +"\nContent-Type: image/jpeg\n\n");
	response.append(bufferPic, streamSize);
	write(fd, response.c_str(), response.length());
	memset(bufferPic, 0, MAX_BUFFER_SIZE);
	// std::cout << "\n{\n" << response << "\n}\n" << std::endl;

}

// #include <unistd.h>

// class Response
// {
// 	public:
// 		Response(Request request)
// 		{
// 		}

// };


bool fileExists(const std::string& filename)
{
    return access(filename.c_str(), F_OK) == 0;
}

void Client::updateResourcePath()
{
	_resourcePath = _request.getTarget();

	/*
		if path is a location:
			1. if location has a root directive, append root to path
			   else if location has an alias directive, switch path with alias
			   else if server has a root directive, append root to path
			   else use path as is
			2. if path is a directory:
					1. if location has an index directive, append index to path
					   else if server has an index directive, append index to path
					   else append index.html to path
			   else if path is a file, use path as is
		else if server has a root directive:
			1. append root to path
			2. if path is a directory:
					1. if server has an index directive, append index to path
					   else append index.html to path
			   else if path is a file, use path as is
		else if path is a directory:
			1. append index.html to path
		else use path as is
	*/

	for (std::vector<hostConfig>::iterator host = _config.getHosts().begin(); host != _config.getHosts().end(); host++)
	{
		if (host->portInt == _port)
		{
			for (std::vector<locationConfig>::iterator loc = host->locations.begin(); loc != host->locations.end(); loc++)
			{
				if (_resourcePath.find(loc->location) != std::string::npos)	// might need to change so that it location only can exist at the start of path
				{
					if (loc->root != "")
					{
						_resourcePath = loc->root + _resourcePath;
					}
					else if (loc->alias != "")
					{
						_resourcePath = loc->alias;
					}
					else if (host->root != "")
					{
						_resourcePath = host->root + _resourcePath;
					}
					if (fileExists(_resourcePath))
					{
						if (loc->index != "")
						{
							_resourcePath.append(loc->index);
						}
						else if (host->index != "")
						{
							_resourcePath.append(host->index);
						}
						else
						{
							_resourcePath.append("index.html");
						}
					}
					return ;
				}
			}
			if (host->root != "")
			{
				_resourcePath = host->root + _resourcePath;
			}
			if (fileExists(_resourcePath))
			{
				if (host->index != "")
				{
					_resourcePath.append(host->index);
				}
				else
				{
					_resourcePath.append("index.html");
				}
			}
			return ;
		}
	}
	// If we get this far, we have no host with the port we are looking for. This should probably be handled elsewhere.
}

void Client::respond()
{
	if (_request.getMethod() == "GET")
	{
		updateResourcePath();
		std::cout << "Resource path: '" << _resourcePath << "'" << std::endl;

		std::ifstream ifs(_resourcePath);
		std::string buffer((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
		std::string response;
		std::unordered_map<std::string, std::string> headers;
		std::unordered_map<std::string, std::string>::iterator it;

		headers = _request.getHeaders();
		response.append("HTTP/1.1 200 OK\n");
		for (it = headers.begin(); it != headers.end(); it++)
		{
			response.append(it->first + ": ");
			response.append(it->second);
			response.append("\n");
		}
		if (_request.getTarget() == "../resources/favicon.ico")
		{
			response.append("Content-Type: image/png");
		}
		response.append("Content-Length: ");
		response.append(std::to_string(buffer.length()));
		response.append("\r\n\r\n");
		response.append(buffer);
		write(_fd, response.c_str(), response.length());
		std::cout << "{" << response << "}[" << buffer << "]" << std::endl;
		// memset(pageBuffer, 0, MAX_BUFFER_SIZE);
	}

}

void Client::handleEvent(short events)
{
	//* we might want to malloc this buffer
	static const int MAX_BUFFER_SIZE = 4095;
	static char buffer[MAX_BUFFER_SIZE + 1];
	size_t readCount = 0;

	if (events & POLLOUT)
	{
		static int i = 0;
		if (_request.getIsComplete() || _request.tryToComplete(_buffer))
		{
			std::cout << RED << "i: " << i << RESET << std::endl;
			_request.printRequest();
			respond();
			_request.clear();
			// mockResponse(_fd);
			i++;
		}
	}
	if (events & POLLIN)
	{
		std::cout << GREEN << "READING\n" << std::endl; 
		readCount = read(_fd, buffer, MAX_BUFFER_SIZE);
		if (readCount < 0)
		{
			throw (std::runtime_error("EXCEPTION: reading failed"));
		}
		buffer[readCount] = '\0';
		_buffer.addToBuffer(&buffer[0], readCount);

	}
	if (!_request.getIsComplete() && _buffer.requestEnded())
	{
		std::cout << GREEN << "double newline found" << RESET << std::endl;
		if (_buffer.getSize())
			std::cout << CYAN << _buffer.getData() << RESET << std::endl;
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
	}
	else
	{
		// std::cout << RED << "double newline NOT found" << RESET << std::endl;
	}
	// std::cout << _buffer.getData();
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
