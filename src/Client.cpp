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
	_response(Response()),
	_failFlag(0)
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
	_statusCode = other._statusCode;
	_autoIndex = other._autoIndex;
	_fd = other._fd;
	_port = other._port;
	_address = other._address;
	_buffer = other._buffer;
	_request = other._request;
	_response = other._response;
	_config = other._config;
	_timeout = other._timeout;
	_failFlag = other._failFlag;
	return (*this);
}

Client::Client(int serverFd, int port, ConfigurationFile &config)
{
	socklen_t adressSize = sizeof(_address);

	_config = config;
	_statusCode = 0;
	_autoIndex = autoIndexState::NONE;
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	_port = port;
	_fd = accept(serverFd, (struct sockaddr *)&_address, &adressSize);
	fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	_timeout = std::time(nullptr);
	_failFlag = 0;
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

int Client::getFd() const { return (_fd); }
int Client::getPort() const { return (_port); }
short Client::getFailFlag() { return (_failFlag); }
// Host &Client::getHost() { return (_host); }

void Client::setFd(int fd) { _fd = fd; }
void Client::setPort(int port) { _port = port; }
void Client::setFailFlag(short flag) { _failFlag = _failFlag | flag; }
// void Client::setHost(Host &host) { _host = host; }


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
		// _resourcePath = _request.getTarget();
		// updateResourcePath();
		// _request.setTarget(_resourcePath);
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

bool Client::checkTimeout(time_t currentTime)
{
	static const time_t maxTimeout = 42;

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
	char buffer[MAX_BUFFER_SIZE + 1];
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
			_request = Request(_buffer.spliceRequest(), _config);
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
