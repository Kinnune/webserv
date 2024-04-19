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
	: _request(Request()),
	_response(Response())
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

	_statusCode = 0;
	_autoIndex = autoIndexState::NONE;
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	_port = port;
	_fd = accept(serverFd, (struct sockaddr *)&_address, &adressSize);
	fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
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
	struct stat path_stat;				// create a stat struct
	stat(path.c_str(), &path_stat);		// get the stats of the path, and store them in the struct
	return S_ISREG(path_stat.st_mode);	// check if the path is a regular file
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
	std::string resourcePath;
	std::string responseStr;


	// Response response(_request);
	// response.completeResponse();

	if (_response.hasRequest() == false)
	{
		_resourcePath = _request.getTarget();
		updateResourcePath();
		_request.setTarget(_resourcePath);
		std::cout << "Updated path: " << color(_resourcePath, CYAN) << std::endl;
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
	std::cout << "Updating resource path" << std::endl;
	std::cout << "Path: " << color(_resourcePath, CYAN) << std::endl;
	std::cout << "Port: " << color(_port, CYAN) << std::endl;
	std::cout << "Hosts: " << color(_config.getHosts().size(), CYAN) << std::endl;

	for (std::vector<hostConfig>::iterator host = _config.getHosts().begin(); host != _config.getHosts().end(); host++)
	{
		if (host->portInt == _port)
		{
			std::cout << "Host found: " << color(host->portInt, CYAN) << std::endl;
			_autoIndex = host->autoIndex;
			for (std::vector<locationConfig>::iterator loc = host->locations.begin(); loc != host->locations.end(); loc++)
			{
				if (locationExists(loc->location))
				{
					std::cout << "Location found: " << color(loc->location, CYAN) << std::endl;
					handleLocation(*host, *loc);
					return ;
				}
			}
			std::cout << color("No location found", RED) << std::endl;
			handleNoLocation(*host);
			return ;
		}
	}
	std::cout << color("No host found", RED) << std::endl;
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

	
	if (isDirectory(_resourcePath))
	{
		updateAutoIndex(loc.autoIndex);
		_resourcePath.append("/");
		if (loc.index != "")
		{
			std::cout << "LOC-INDEX: " << color(loc.index, GREEN) << std::endl;
			_resourcePath.append(loc.index);
		}
		else if (host.index != "")
		{
			std::cout << "HOST-INDEX: " << color(host.index, GREEN) << std::endl;
			_resourcePath.append(host.index);
		}
		else if (_autoIndex == autoIndexState::ON)
		{
			std::cout << "AUTOINDEX: " << color("true", GREEN) << std::endl;
			_autoIndex = autoIndexState::ON;
		}
		else
		{
			std::cout << "No index found. Appending index.html" << std::endl;
			_resourcePath.append("index.html");
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
		if (host.index != "")
		{
			std::cout << "HOST-INDEX: " << color(host.index, GREEN) << std::endl;
			_resourcePath.append(host.index);
		}
		else
		{
			std::cout << "No index found. Appending index.html" << std::endl;
			_resourcePath.append("index.html");
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
		if (DEBUG)
			std::cout << GREEN << "READING\n" << std::endl; 
		readCount = read(_fd, buffer, MAX_BUFFER_SIZE);
		std::cout << buffer << std::endl;
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
