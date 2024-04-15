#include "Client.hpp"
#include "Colors.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>		// access()
#include <sys/stat.h>	// stat()


//------------------------------------------------------------------------------
//	CONSTRUCTORS & DESTRUCTORS
//------------------------------------------------------------------------------

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
	_config = other._config;
	return (*this);
}

Client::Client(int serverFd, int port)
{
	socklen_t adressSize = sizeof(_address);

	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	_port = port;
	_fd = accept(serverFd, (struct sockaddr *)&_address, &adressSize);
	fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}


//------------------------------------------------------------------------------
//	GETTERS & SETTERS
//------------------------------------------------------------------------------

ConfigurationFile &Client::getConfig()
{
	return (_config);
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

//------------------------------------------------------------------------------
//	MEMBER FUNCTIONS
//------------------------------------------------------------------------------

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

std::string Client::listDirectory(std::string path)
{
	std::string directoryListResponse;
	DIR* directory = opendir(path.c_str());
	struct dirent* entry;

	if (directory != nullptr)
	{
        directoryListResponse.append("<table border=\"1\">");
        directoryListResponse.append("<tr><th>File Name</th></tr>");

		while ((entry = readdir(directory)) != nullptr)
		{
            directoryListResponse.append("<tr><td>" + std::string(entry->d_name) + "</td></tr>");
			if (DEBUG)
				std::cout << entry->d_name << std::endl;
		}

        directoryListResponse.append("</table>");
		if (DEBUG)
	        std::cout << "</table>" << std::endl;
		closedir(directory);
	}
	else
	{
		std::cerr << "Unable to open directory: " << path << std::endl;
	}
	return (directoryListResponse);
}

// void Client::errorResponse(int status)
// {

// }


void Client::respond()
{
	std::string resourcePath;
	std::string responseStr;
	Response response(_request);

	responseStr = response.toString();
	std::cout << RED << responseStr << RESET <<std::endl;
	write(_fd, responseStr.c_str(), responseStr.length());
}

bool Client::isFile(const std::string& path)
{
	struct stat path_stat;				// create a stat struct
	stat(path.c_str(), &path_stat);		// get the stats of the path, and store them in the struct
	return S_ISREG(path_stat.st_mode);	// check if the path is a regular file
}

bool Client::isDirectory(const std::string& path)
{
	struct stat path_stat;
	stat(path.c_str(), &path_stat);
	return S_ISDIR(path_stat.st_mode);
}

bool Client::locationExists(const std::string& location)
{
	if (std::strncmp(_resourcePath.c_str(), location.c_str(), location.length()) == 0)
	{
		if (_resourcePath.length() == location.length() || _resourcePath[location.length()] == '/')
		{
			return true;
		}
	}
	return false;
}

void Client::updateResourcePath()
{
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

	std::cout << "Updating resource path" << std::endl;
	std::cout << "Port: " << color(_port, CYAN) << std::endl;
	std::cout << "Hosts: " << color(_config.getHosts().size(), CYAN) << std::endl;
	for (std::vector<hostConfig>::iterator host = _config.getHosts().begin(); host != _config.getHosts().end(); host++)
	{
		if (host->portInt == _port)
		{
			std::cout << "Host found: " << color(host->portInt, CYAN) << std::endl;
			for (std::vector<locationConfig>::iterator loc = host->locations.begin(); loc != host->locations.end(); loc++)
			{
				if (locationExists(loc->location))
				{
					std::cout << "Location found: " << color(loc->location, CYAN) << std::endl;
					if (loc->redirection != "" && this->_request.getMethod() == "GET")
					{
						std::cout << "REDIRECTION: " << loc->redirection << std::endl;
						// set resource code to 301
						_resourcePath = loc->redirection;
						return ;
					}
					if (loc->root != "")
					{
						std::cout << "LOC-ROOT: " << color(loc->root, GREEN) << std::endl;
						_resourcePath = loc->root + _resourcePath.substr(loc->location.length());
					}
					else if (loc->alias != "")
					{
						std::cout << "LOC-ALIAS: " << color(loc->alias, GREEN) << std::endl;
						_resourcePath = loc->alias;
					}
					else if (host->root != "")
					{
						std::cout << "HOST-ROOT: " << color(host->root, GREEN) << std::endl;
						_resourcePath = host->root + _resourcePath.substr(loc->location.length());
					}
					if (isDirectory(_resourcePath))
					{
						if (loc->index != "")
						{
							std::cout << "LOC-INDEX: " << color(loc->index, GREEN) << std::endl;
							_resourcePath.append(loc->index);
						}
						else if (host->index != "")
						{
							std::cout << "HOST-INDEX: " << color(host->index, GREEN) << std::endl;
							_resourcePath.append(host->index);
						}
						else
						{
							std::cout << "No index found. Appending index.html" << std::endl;
							_resourcePath.append("index.html");
						}
					}
					return ;
				}
			}
			std::cout << color("No location found", RED) << std::endl;
			if (host->root != "")
			{
				std::cout << "HOST-ROOT: " << color(host->root, GREEN) << std::endl;
				_resourcePath = host->root + _resourcePath;
			}
			if (isDirectory(_resourcePath))
			{
				if (host->index != "")
				{
					std::cout << "HOST-INDEX: " << color(host->index, GREEN) << std::endl;
					_resourcePath.append(host->index);
				}
				else
				{
					std::cout << "No index found. Appending index.html" << std::endl;
					_resourcePath.append("index.html");
				}
			}
			return ;
		}
	}
	std::cout << color("No host found", RED) << std::endl;
	// If we get this far, we have no host with the port we are looking for. This should probably be handled elsewhere.
}

void Client::handleEvent(short events)
{
	//* we might want to malloc this buffer
	static const int MAX_BUFFER_SIZE = 4095;
	static char buffer[MAX_BUFFER_SIZE + 1];
	ssize_t readCount = 0;	// changed to ssize_t instead of size_t, because read() returns -1 on error, and size_t is unsigned

	if (events & POLLOUT)
	{
		static int i = 0;
		if (_request.getIsComplete() || _request.tryToComplete(_buffer))
		{
			if (DEBUG)
				std::cout << RED << "i: " << i << RESET << std::endl;
			if (DEBUG)
				_request.printRequest();
			respond();
			_request.clear();
			// mockResponse(_fd);
			i++;
		}
	}
	if (events & POLLIN)
	{
		if (DEBUG)
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
		if (DEBUG)
			std::cout << GREEN << "double newline found" << RESET << std::endl;
		if (_buffer.getSize())
			if (DEBUG)
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
