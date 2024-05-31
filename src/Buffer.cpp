
#include "Buffer.hpp"

Buffer::Buffer() : _endLiteral("\r\n\r\n"), _data(0) {}

Buffer::~Buffer() {}

Buffer &Buffer::operator=(const Buffer &other)
{
	_data = other._data;
	_endLiteral = other._endLiteral;
	return (*this);
}

unsigned char *Buffer::requestEnded()
{
	std::vector<unsigned char>::iterator it;
	unsigned char *position;

	if (_data.size() < _endLiteral.length())
	{
		return nullptr;
	}
	for (it = _data.begin(); it != _data.end() - (_endLiteral.length() - 1); it++)
	{
		position = &(*it);
		if (strncmp((char *)position, _endLiteral.c_str(), _endLiteral.length()) == 0)
		{
			return (&(*(it + _endLiteral.length())));
		}
	}
	return nullptr;
}

void Buffer::addToBuffer(char *data, size_t size)
{
	_data.insert(_data.end(), data, data + size);
}

std::vector<unsigned char> Buffer::spliceRequest()
{
	unsigned char *endPos;
	size_t requestSize;
	std::vector<unsigned char> request;

	endPos = requestEnded();
	if (!endPos)
	{
		throw(std::runtime_error("Exception: no request to get"));
	}
	requestSize = endPos - getBegin();
	request = std::vector<unsigned char>(_data.begin(), _data.begin() + requestSize);
	_data = std::vector<unsigned char>(_data.begin() + requestSize, _data.end());
	return request;
}

ssize_t Buffer::readChunkLength()
{
	size_t endOfLinePos = std::string::npos;
	std::string lineEndLiteral("\r\n");

	if (_data.size() < 2)
	{
		return -1;
	}
	for (size_t i = 0; i < _data.size() - 1; ++i)
	{
		if (_data[i] == '\r' && _data[i + 1] == '\n')
		{
			endOfLinePos = i;
			break;
		}
	}

	if (endOfLinePos == std::string::npos)
	{
		return -1;
	}
	std::string chunkSizeStr(_data.begin(), _data.begin() + endOfLinePos);
	size_t chunkSize;
	std::stringstream strStream(chunkSizeStr);

	strStream >> std::hex >> chunkSize;

	if (strStream.fail())
	{
		return -1;
	}
	size_t totalLength = endOfLinePos + lineEndLiteral.length() + chunkSize + lineEndLiteral.length();
	if (_data.size() >= totalLength)
	{
		_data.erase(_data.begin(), _data.begin() + endOfLinePos + lineEndLiteral.length() + chunkSize + lineEndLiteral.length());
	}
	else
	{
		return -1;
	}
	return static_cast<ssize_t>(chunkSize);
}

std::vector<unsigned char> Buffer::extractChunk(size_t chunkSize)
{
	if (_data.size() < chunkSize + 2)
	{
		return std::vector<unsigned char>();
	}

	std::vector<unsigned char> chunk(_data.begin(), _data.begin() + chunkSize);
	_data.erase(_data.begin(), _data.begin() + chunkSize + 2);
	return chunk;
}

size_t Buffer::getSize()
{
	return _data.size();
}

unsigned char *Buffer::getBegin()
{
	return &_data[0];
}

std::vector<unsigned char>::iterator Buffer::getEnd()
{
	return _data.end();
}

std::vector<unsigned char> &Buffer::getData()
{
	return _data;
}

void Buffer::clear()
{
	_data.clear();
}
