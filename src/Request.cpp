
#include "Request.hpp"

Request::Request()
	: _names(NULL),
	_values(NULL),
	_body("")
{
}

Request::~Request()
{
	delete (_names);
	delete (_values);
}

Request::Request(Request const &other)
	: _names(NULL),
	_values(NULL)
{
	*this = other;
}

Request &Request::operator=(Request const &other)
{
	delete (_names);
	delete (_values);
	copyHeader(other);
	_method = other._method;
	_target = other._target;
	_version = other._version;
	return (*this);
}

Request::Request(std::vector<unsigned char> &content)
{
	parseContent(content);

}

void Request::copyHeader(Request const &other)
{
	for (_header_count = 0; _header_count >= other._header_count; _header_count++)
	{
		_names[_header_count] = other._names[_header_count];
		_values[_header_count] = other._values[_header_count];
	}
}


bool isWS(unsigned char c)
{
	static unsigned char const space = 32;
	static unsigned char const horizontalTab = 9;

	return (c == space | c == horizontalTab);
}

int skipWS(std::vector<unsigned char> &data, int index)
{
	size_t size = data.size();

	while (index < size && isWS(data[index]))
	{
		index++;
	}
	return (index < size ? index : -1);
}

int find_version()
{

}

int Request::firstLine(std::vector<unsigned char>data)
{
	int i;
	int index = skipWS(data, index);

	//--match the first field which should be the method
	for (i = 0; i < METHOD_COUNT; i++)
	{
		if (std::strncmp(_supportedMethods[i].c_str(), (char *)data[index], data.size() - index))
		{
			_methodType = (METHOD_TYPE)i;
			_method = _supportedMethods[i];
			break ;
		}
	}
	if (i == METHOD_COUNT)
	{
		return (-1);
	}
	index = skipWS(data, index);
	if (index < 0)
	{
		return (-1);
	}
	
}


std::ostream &operator<<(std::ostream &o, std::vector<unsigned char>data)
{
	for ( std::vector<unsigned char>::iterator it = data.begin(); it < data.end(); it++)
	{
		o << *it << "(" << (unsigned int)*it << ")";
	}
	return(o);
}

std::vector<unsigned char> getLine(std::vector<unsigned char> data, size_t index)
{
	std::vector<unsigned char>::iterator it;

	for (it = data.begin() + index; it < data.end(); it++)
	{
		if (*it == '\n')
		{
			it++;
			break ;
		}
	}
	return (std::vector<unsigned char>(data.begin() + index, it));
}

// The line terminator for message-header fields is the sequence CRLF. However,
// we recommend that applications, when parsing such headers,
// recognize a single LF as a line terminator and ignore the leading CR.
int Request::parseContent(std::vector<unsigned char> &data)
{
	int i;
	int index = 0;
	std::vector<unsigned char> line;

	line = getLine(data, index);
	std::cout << line << std::endl;
	firstLine(line);
	return (0);
}
