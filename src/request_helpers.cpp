
#include "Request.hpp"

std::vector<unsigned char> getLine(std::vector<unsigned char> &data, size_t index)
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

bool lineEmpty(std::vector<unsigned char> &line)
{
	int index;

	index = skipWS(line, 0);
	return ((validIndex(line, index) && line[index] == '\n') || line.size() == 0);
}

bool isWS(unsigned char c)
{
	static unsigned char const space = 32;
	static unsigned char const horizontalTab = 9;

	return (c == space | c == horizontalTab);
}

int skipWS(std::vector<unsigned char> &data, ssize_t index)
{
	ssize_t size = data.size();

	while (index < size && (isWS(data[index]) || data[index] == '\r'))
	{
		index++;
	}
	return (index < size ? index : -1);
}

int skipToWS(std::vector<unsigned char> &data, ssize_t index)
{
	ssize_t size = data.size();

	while (index < size && !isWS(data[index])&& data[index] != '\n')
	{
		index++;
	}
	return (index < size ? index : -1);
}

bool validIndex(std::vector<unsigned char> &data, ssize_t index)
{
	return (index > 0 | index < static_cast<ssize_t>(data.size()));
}

int seekColon(std::vector<unsigned char> &data, ssize_t index)
{
	ssize_t size = data.size();

	while (index < size && data[index] != ':')
	{
		index++;
	}
	return (index < size ? index : -1);
}
