#include "Colors.hpp"
#include "Client.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>		// access()
#include <sys/stat.h>	// stat()


//------------------------------------------------------------------------------
//	CONSTRUCTORS & DESTRUCTORS
//------------------------------------------------------------------------------

Client::Client()
	: _request(Request()),
	_response(Response())
{
	_timeout = std::time(nullptr);
}

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
	_timeout = other._timeout;
	return (*this);
}

Client::Client(int serverFd, int port)
{
	socklen_t adressSize = sizeof(_address);

	_statusCode = 0;
	_autoIndex = autoIndexState::NONE;
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	_port = port;
	_fd = accept(serverFd, (struct sockaddr *)&_address, &adressSize);
	fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	_timeout = std::time(nullptr);
}


//------------------------------------------------------------------------------
//	OPERATOR OVERLOADS
//------------------------------------------------------------------------------

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
//	BOOL FUNCTIONS
//------------------------------------------------------------------------------

bool Client::isFile(const std::string& path)
{
	struct stat path_stat;						// create a stat struct
	if (stat(path.c_str(), &path_stat) != 0)	// get the stats of the path, and store them in the struct
		return false;							// if the path does not exist, return false
	return S_ISREG(path_stat.st_mode);			// check if the path is a regular file
}

//------------------------------------------------------------------------------

bool Client::isDirectory(const std::string& path)
{
	struct stat path_stat;
	stat(path.c_str(), &path_stat);
	return S_ISDIR(path_stat.st_mode);
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

bool Client::allowedMethod(std::vector<std::string> methods, std::string method)
{
	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
	{
		if (*it == method)
		{
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------------
//	MEMBER FUNCTIONS
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

// void Client::errorResponse(int status)
// {

// }

//------------------------------------------------------------------------------

bool Client::respond()
{
	std::string responseStr;

	if (_response.hasRequest() == false)
	{
		_resourcePath = _request.getTarget();
		updateResourcePath();
		_request.setTarget(_resourcePath);
		// std::cout << "Updated path: " << color(_resourcePath, CYAN) << std::endl;
		_response = Response(_request);
	}
	if (_response.completeResponse())
	{
		responseStr = _response.toString();
		write(_fd, responseStr.c_str(), responseStr.length());
		_response = Response();
		return (true);
	}
	return (false);
}


//------------------------------------------------------------------------------
//	UPDATE RESOURCE PATH
//------------------------------------------------------------------------------

void Client::updateResourcePath()
{
	// std::cout << "Updating resource path" << std::endl;
	// std::cout << "Path: " << color(_resourcePath, CYAN) << std::endl;
	// std::cout << "Port: " << color(_port, CYAN) << std::endl;
	// std::cout << "Hosts: " << color(_config.getHosts().size(), CYAN) << std::endl;

	for (std::vector<hostConfig>::iterator host = _config.getHosts().begin(); host != _config.getHosts().end(); host++)
	{
		if (host->portInt == _port)
		{
			// std::cout << "Host found: " << color(host->portInt, CYAN) << std::endl;
			updateAutoIndex(host->autoIndex);
			for (std::vector<locationConfig>::iterator loc = host->locations.begin(); loc != host->locations.end(); loc++)
			{
				if (locationExists(loc->location))
				{
					// std::cout << "Location found: " << color(loc->location, CYAN) << std::endl;
					updateAutoIndex(loc->autoIndex);
					handleLocation(*host, *loc);
					return ;
				}
			}
			// std::cout << color("No location found", RED) << std::endl;
			handleNoLocation(*host);
			return ;
		}
	}
	// std::cout << color("No host found", RED) << std::endl;
	// If we get this far, we have no host with the port we are looking for. This should probably be handled elsewhere.
}

//------------------------------------------------------------------------------

void Client::handleLocation(hostConfig &host, locationConfig &loc)
{
	// check if method is allowed
	if (!allowedMethod(loc.methods, _request.getMethod()))
	{
		std::cout << "Method not allowed" << std::endl;
		_statusCode = 405;
		return ;
	}

	// handle redirection
	if (loc.redirection != "" && _request.getMethod() == "GET")
	{
		std::cout << "REDIRECTION: " << loc.redirection << std::endl;
		_statusCode = 301;
		_resourcePath = loc.redirection;
		return ;
	}

	// handle root/alias
	if (loc.root != "")
	{
		std::cout << "LOC-ROOT: " << color(loc.root, GREEN) << std::endl;
		if (isFile(loc.root + _resourcePath))
		{
			std::cout << "Resource path is a file" << std::endl;
			_resourcePath = loc.root + _resourcePath;
			return ;
		}
		_resourcePath = loc.root + _resourcePath.substr(loc.location.length());
	}
	else if (loc.alias != "")
	{
		std::cout << "LOC-ALIAS: " << color(loc.alias, GREEN) << std::endl;
		_resourcePath = loc.alias;
	}
	else if (host.root != "")
	{
		std::cout << "HOST-ROOT: " << color(host.root, GREEN) << std::endl;
		_resourcePath = host.root + _resourcePath.substr(loc.location.length());
	}

	// handle index and autoindex
	if (isDirectory(_resourcePath))
	{
		_resourcePath.append("/");
		if (loc.index_pages.size() > 0)
		{
			for (std::vector<std::string>::iterator it = loc.index_pages.begin(); it != loc.index_pages.end(); it++)
			{
				if (isFile(_resourcePath + *it))
				{
					_resourcePath.append(*it);
					return ;
				}
			}
			_statusCode = 403;
		}
		else if (host.index_pages.size() > 0)
		{
			for (std::vector<std::string>::iterator it = host.index_pages.begin(); it != host.index_pages.end(); it++)
			{
				if (isFile(_resourcePath + *it))
				{
					_resourcePath.append(*it);
					return ;
				}
			}
			_statusCode = 403;
		}
		else if (_autoIndex != autoIndexState::ON)
		{
			std::cout << "No index directive found. Looking for index file" << std::endl;
			lookForIndexFile();
		}
		else if (_autoIndex == autoIndexState::ON)
		{
			std::cout << "AUTOINDEX: " << color("true", GREEN) << std::endl;
			_autoIndex = autoIndexState::ON;
		}
	}
}

//------------------------------------------------------------------------------

void Client::handleNoLocation(hostConfig &host)
{
	if (host.root != "")
	{
		std::cout << "HOST-ROOT: " << color(host.root, GREEN) << std::endl;
		_resourcePath = host.root + _resourcePath;
	}
	if (isDirectory(_resourcePath))
	{
		if (host.index_pages.size() > 0)
		{
			for (std::vector<std::string>::iterator it = host.index_pages.begin(); it != host.index_pages.end(); it++)
			{
				if (isFile(_resourcePath + *it))
				{
					_resourcePath.append(*it);
					return ;
				}
			}
			_statusCode = 403;
		}
		else if (_autoIndex != autoIndexState::ON)
		{
			std::cout << "No index directive found. Looking for index file" << std::endl;
			lookForIndexFile();
		}
		else if (_autoIndex == autoIndexState::ON)
		{
			std::cout << "AUTOINDEX: " << color("true", GREEN) << std::endl;
			_autoIndex = autoIndexState::ON;
		}
	}
}

//------------------------------------------------------------------------------

void Client::updateAutoIndex(autoIndexState state)
{
	if (state != autoIndexState::NONE)
	{
		_autoIndex = state;
	}
}

//------------------------------------------------------------------------------

void Client::lookForIndexFile()
{
	std::string path = _resourcePath;
	std::string indexFiles[] = {"index.html", "index.htm", "index.php"};
	for (int i = 0; i < 3; i++)
	{
		if (isFile(path + indexFiles[i]))
		{
			_resourcePath = path + indexFiles[i];
			return ;
		}
	}
	_statusCode = 403;
}

//------------------------------------------------------------------------------

bool Client::checkTimeout(time_t currentTime)
{
	static const time_t maxTimeout = 12;

	// std::cout << std::boolalpha << (currentTime - _timeout > maxTimeout) << "_timeout: " << _timeout << " currentTime: " << currentTime << std::endl;
	return (currentTime - _timeout > maxTimeout);
}

//------------------------------------------------------------------------------
//	HANDLE EVENT
//------------------------------------------------------------------------------

void Client::handleEvent(short events)
{
	//* we might want to malloc this buffer
	static const int MAX_BUFFER_SIZE = 4095;
	static char buffer[MAX_BUFFER_SIZE + 1];
	ssize_t readCount = 0;	// changed to ssize_t instead of size_t, because read() returns -1 on error, and size_t is unsigned
	// static bool waitCGI = false;

	if (events & POLLOUT)
	{
		static int i = 0;
		if (_request.getIsComplete() || _request.tryToComplete(_buffer))
		{
			if (DEBUG)
				std::cout << RED << "i: " << i << RESET << std::endl;
			if (DEBUG)
				_request.printRequest();
			if (respond())
			{
				_request.clear();
			}
			// mockResponse(_fd);
			i++;
		}
	}
	if (events & POLLIN)
	{
		// _timeout = std::time(nullptr);
		if (DEBUG)
			std::cout << GREEN << "READING\n" << RESET << std::endl; 
		readCount = read(_fd, buffer, MAX_BUFFER_SIZE);
		if (readCount < 0)
		{
			throw (std::runtime_error("EXCEPTION: reading failed"));
		}
		buffer[readCount] = '\0';
		std::cout << buffer << std::endl;
		_buffer.addToBuffer(&buffer[0], readCount);
		std::cout << _buffer.getData() << std::endl;
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
	else if (_request.getIsChunked() && !_request.getIsComplete())
	{
		ssize_t chunkSize;
		chunkSize = _buffer.readChunkLength();
		if (chunkSize == 0)
		{
			_request.setIsComplete(true);
			_request.setContentLenght(_request.getContentLenght() + 1);
		}
		else if (chunkSize > 0)
		{
			std::vector<unsigned char>chunk = _buffer.extractChunk(chunkSize);
			for (unsigned char c : chunk)
			{
				_request.getBody().push_back(c);
			}
			_request.setContentLenght(_request.getContentLenght() + chunkSize);
		}
		// std::cout << RED << "double newline NOT found" << RESET << std::endl;
	}
	// std::cout << _buffer.getData();
}
