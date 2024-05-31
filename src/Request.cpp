#include "Request.hpp"
#include "Colors.hpp"


//------------------------------------------------------------------------------
//  CONSTRUCTORS & DESTRUCTORS
//------------------------------------------------------------------------------

Request::Request()
	: _completed(false),
	_isValid(false),
	_isChunked(false),
	_contentLength(-1),
	_errorCode(0)
{
}

Request::~Request()
{
}

Request::Request(Request const &other)
{
	*this = other;
}

Request &Request::operator=(Request const &other)
{
	_method = other._method;
	_target = other._target;
	_version = other._version;
	_headers = other._headers;
	_completed = other._completed;
	_contentLength = other._contentLength;
	_isValid = other._isValid;
	_headers = other._headers;
	_body = other._body;
	_isChunked = other._isChunked;
	_host = other._host;
	_errorCode = other._errorCode;
	return (*this);
}

void Request::clear()
{
	_isChunked = false;
	_completed = false;	
	_isValid = false;
	_isChunked = false;
	_contentLength = -1;
	_headers.clear();
	_body.clear();
	_method.clear();
	_target.clear();
	_version.clear();
	_errorCode = 0;
}

Request::Request(std::vector<unsigned char> content)
	: _completed(false),
	_isValid(false),
	_isChunked(false),
	_errorCode(0)
{
	if (parseContent(content) == -1)
	{
		throw (std::runtime_error("Failed to parse request"));
	}
}

Request::Request(std::vector<unsigned char> content, ConfigurationFile config)
	: _completed(false),
	_isValid(false),
	_isChunked(false),
	_errorCode(0)
{
	if (parseContent(content) == -1)
	{
		throw (std::runtime_error("Failed to parse request"));
	}
	_host = *(config.getHost(_headers["Host"]));
}

//------------------------------------------------------------------------------
//  GETTERS & SETTERS
//------------------------------------------------------------------------------

int Request::getErrorCode()
{
	return (_errorCode);
};

bool Request::getIsValid()
{
	return (_isValid);
};

bool Request::getIsChunked()
{
	return (_isChunked);
};

bool Request::getIsComplete()
{
	return (_completed);
}

ssize_t Request::getContentLength()
{
	return (_contentLength);
}

void Request::setContentLength(ssize_t value)
{
	_contentLength = value;
}

std::vector<unsigned char> Request::getBody()
{
	return (_body);
};

Host &Request::getHost()
{
	return (_host);
};

std::string Request::getVersion()
{
	return (_version);
}

std::string &Request::getTarget()
{
	return (_target);
}

std::string const &Request::getMethod() const
{
	return (_method);
}

std::unordered_map<std::string, std::string> &Request::getHeaders()
{
	return (_headers);
}

int Request::getMaxBodySizeAllowed()
{
	int maxBodySize = _host.getMaxBody();
	std::string path = _target;
	size_t firstSlash = _target.find("/");
	if (firstSlash != std::string::npos)
	{
		size_t secondSlash = _target.find("/", firstSlash + 1);
		if (secondSlash != std::string::npos)
		{
			path = _target.substr(firstSlash, secondSlash - firstSlash);
		}
	}
	std::vector<Location> locations = _host.getLocations();
	for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		std::string location = it->getLocation();
		if (location == path)
		{
			if (it->getMaxBody() != -1)
			{
				maxBodySize = it->getMaxBody();
			}
			break ;
		}
	}
	return (maxBodySize);
}

void Request::setIsComplete(bool value)
{
	_completed = value;
}

void Request::setTarget(std::string target)
{
	_target = target;
}

void Request::setIsChunked(bool chunked)
{
	_isChunked = chunked;
}

void Request::setErrorCode(int errorCode)
{
	_errorCode = errorCode;
}

//------------------------------------------------------------------------------
//  MEMBER FUNCTIONS
//------------------------------------------------------------------------------

bool Request::tryToComplete(Buffer &buffer)
{
	if (_contentLength > getMaxBodySizeAllowed())
	{
		_errorCode = 413;
		return (true);
	}
	if (_contentLength > 0 && buffer.getSize() >= static_cast<size_t>(_contentLength))
	{
		_body.insert(_body.end(), buffer.getData().begin(), buffer.getData().begin() + _contentLength);
		buffer.getData().erase(buffer.getData().begin(), buffer.getData().begin() + _contentLength);
		_completed = true;
		return (true);
	}
	// else if (_contentLength == CHUNKED_REQUEST)
	
	return (false);
}

int Request::headerLineParse(std::vector<unsigned char> &line)
{
	ssize_t index = 0;
	ssize_t wordSize = 0;
	std::string key;
	std::string value;

	index = skipWS(line, 0);
	if (!validIndex(line, index))
	{
		return (-1);
	}
	wordSize = seekColon(line, index);
	if (!validIndex(line, wordSize || wordSize < 1))
	{
		return (-1);
	}
	key = std::string(line.begin() + index, line.begin() + wordSize);
	index += key.size() + 1;
	if (!validIndex(line, index))
	{
		return (-1);
	}
	index = skipWS(line, index);
	if (!validIndex(line, index))
	{
		return (-1);
	}
	value = std::string(line.begin() + index, line.end());
	while (value.back() == '\n' || value.back() == '\r')
	{
		value.pop_back();
	}
	_headers[key] = value;
	return (0);
}

int Request::firstLineParse(std::vector<unsigned char> &line)
{
	ssize_t index = 0;
	ssize_t wordSize = 0;

	index = skipWS(line, 0);
	if (!validIndex(line, index))
	{
		return (-1);
	}
	index = skipWS(line, index);
	wordSize = skipToWS(line, index);
	if (!validIndex(line, index) || !validIndex(line, wordSize))
	{
		return (-1);
	}
	_method = std::string(line.begin() + index, line.begin() + wordSize);
	index  = skipToWS(line, index);
	index = skipWS(line, index);
	wordSize = skipToWS(line, index);
	if (!validIndex(line, index) || !validIndex(line, wordSize))
	{
		return (-1);
	}
	_target = std::string(line.begin() + index, line.begin() + wordSize);
	index  = skipToWS(line, index);
	index = skipWS(line, index);
	wordSize = skipToWS(line, index);
	if (!validIndex(line, index) || !validIndex(line, wordSize))
	{
		return (-1);
	}
	_version = std::string(line.begin() + index, line.begin() + wordSize);
	return (0);
}

bool Request::detectContentlength()
{
	std::unordered_map<std::string, std::string>::iterator it;
	ssize_t len = 0;

	_isChunked = false;
	_completed = true;
	for (it = _headers.begin(); it != _headers.end(); it++)
	{
		if (it->first == "Transfer-Encoding" && !strncmp(it->second.c_str(), "chunked", it->second.size()))	//J removed std:: before strncmp(), because strncmp() is a C function
		{
			len = -1;
			_isChunked = true;
			break ;
		}
		else if (it->first == "Content-Length")
		{
			try
			{
				len = std::stoi(it->second);
				if (len < 0)
				{
					return (false);
				}
			}
			catch (std::exception &e)
			{
				return (false);
			}
		}
	}
	if (len)
	{
		_completed = false;
	}
	_contentLength = len;
	return (true);
}

int Request::parseContent(std::vector<unsigned char> &data)
{
	ssize_t index = 0;
	std::vector<unsigned char> line;

	line = getLine(data, index);
	if (firstLineParse(line))
	{
		return (-1);
	}
	index += line.size();
	line = getLine(data, index);
	while (!lineEmpty(line))
	{
		if (headerLineParse(line))
		{
			return (-1);
		}
		index += line.size();
		line = getLine(data, index);
	}
	if (!detectContentlength())
	{
		return (-1);
	}
	_isValid = true;
	return (0);
}
