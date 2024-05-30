
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <string.h>
#include <vector>
#include <sstream>
#include <iostream>
#include "Colors.hpp"

std::ostream &operator<<(std::ostream &o, std::vector<unsigned char> data);
	
class Buffer
{
public:
	Buffer();
	~Buffer();
	Buffer &operator=(const Buffer &other);
	unsigned char *requestEnded();
	void addToBuffer(char *data, size_t size);
	std::vector<unsigned char> spliceRequest();
	ssize_t readChunkLength();
	std::vector<unsigned char> extractChunk(size_t chunkSize);
	size_t getSize();
	unsigned char *getBegin();
	std::vector<unsigned char>::iterator getEnd();
	std::vector<unsigned char> &getData();
	void clear();
private:
	Buffer(const Buffer &other);
	std::string _endLiteral;
	std::vector<unsigned char> _data;
};

#endif
