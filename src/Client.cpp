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
	_failFlag(0),
	_sessionID("")
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
	_sessionID = other._sessionID;
	return (*this);
}

Client::Client(int serverFd, int port, ConfigurationFile &config)
{
	socklen_t adressSize = sizeof(_address);

	_sessionID = "";
	_failFlag = 0;
	_config = config;
	_statusCode = 0;
	_autoIndex = autoIndexState::NONE;
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	_port = port;
	_timeout = std::time(nullptr);
	if ((_fd = accept(serverFd, (struct sockaddr *)&_address, &adressSize)) == -1)
	{
		_failFlag = 1;
		return ;
	}
	fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}


//------------------------------------------------------------------------------
//	OPERATOR OVERLOADS
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &o, std::vector<unsigned char>data)
{
	for (std::vector<unsigned char>::iterator it = data.begin(); it < data.end(); it++)
	{
		// o << *it;
		// o << (int)*it;
		o << "'" << (int)(*it) << "'" << *it;
	}
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
		_response = Response(_request, _sessionID);
	}
	if (_response.completeResponse())
	{
		responseStr = _response.toString();
		// should also check if wrote less than length
		if (write(_fd, responseStr.c_str(), responseStr.length()) == -1)
		{
			_failFlag = 1;
		}
		Server::getInstance().setDidIO(_fd);
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

std::unordered_map<std::string, std::string> parseHeader(std::string header)
{
    std::unordered_map<std::string, std::string> headerValues;

    std::stringstream ss(header);
    std::string token;
    while (std::getline(ss, token, ';'))
	{
        std::size_t pos = token.find('=');
        if (pos != std::string::npos)
		{
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            headerValues[key] = value;
        }
    }
    return (headerValues);
}


std::string Client::generateSessionId()
{
    static unsigned int idSuffix = 0;
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration = now.time_since_epoch();
    long long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    std::stringstream ss;
    ss << milliseconds << '-' << idSuffix++;
    return (ss.str());
}

void Client::setSessionID()
{
	if (_request.getHeaders().find("Cookie") != _request.getHeaders().end())
	{
		std::unordered_map<std::string, std::string>values = parseHeader(_request.getHeaders().find("Cookie")->second);
		if (values.find("session_id") != values.end())
		{
			_sessionID = values.find("session_id")->second;
		}
		else
		{
			_sessionID = generateSessionId();
		}
	}
	else
	{
		_sessionID = generateSessionId();
	}
}

void Client::handleEvent(short events)
{
	//* we might want to malloc this buffer
	static const int MAX_BUFFER_SIZE = 4095;
	char buffer[MAX_BUFFER_SIZE + 1];
	ssize_t readCount = 0;

	if (events & POLLOUT)
	{
		if (_request.getIsComplete() || _request.tryToComplete(_buffer))
		{
			std::cerr << color("----REQUEST---------------------------------------------", PURPLE) << std::endl;
			_request.printRequest();
			std::cerr << color("--------------------------------------------------------", PURPLE) << std::endl;
			if (respond())
			{
				_request.clear();
				return ;
			}
		}
	}
	if (events & POLLIN)
	{
		readCount = read(_fd, buffer, MAX_BUFFER_SIZE);
		Server::getInstance().setDidIO(_fd);
		if (readCount < 0)
		{
			_failFlag = 1;
			return ;
		}
		buffer[readCount] = '\0';
		// std::cout << buffer << std::endl;
		_buffer.addToBuffer(&buffer[0], readCount);
	}
	if (!_request.getIsComplete() && _buffer.requestEnded() && !_request.getIsChunked())
	{
		// try
		// {
		_request = Request(_buffer.spliceRequest(), _config);
		// _request.printRequest();
		std::cout << color("MADE NEW REQUEST", RED) << std::endl;
		setSessionID();
		// }
		// catch (std::exception &e)
		// {
		// 	std::cerr << "EXCEPTION OCCURRED: " << e.what() << std::endl;
		// }
		if (!_request.getIsValid())
		{
			std::cout << "invalide" << std::endl;
			//  respond something
			//  close connection
		}
	}
	else if (_request.getIsChunked() && !_request.getIsComplete())
	{
		std::vector<unsigned char> tmp= _buffer.getData();
		ssize_t chunkSize;
		chunkSize = _buffer.readChunkLength();
		// std::cout << chunkSize << std::endl;
		if (chunkSize == 0)
		{
			std::cout << "read end of transmission chunk" << std::endl;
			_request.setIsComplete(true);
			_request.setIsChunked(false);
			_request.setContentLenght(0);
		}
		else if (chunkSize > 0)
		{
			std::cout << color(chunkSize, PURPLE) << std::endl;
			std::cout << "going to extract chunk" << std::endl;
			std::vector<unsigned char>chunk = _buffer.extractChunk(chunkSize);
			for (unsigned char c : chunk)
			{
				_request.getBody().push_back(c);
			}
			_request.setContentLenght(_request.getContentLenght() + chunkSize);
		}
 	}
}

/*
int maxBody = _request.getMaxBody();
*/