
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <string.h>	//J added for strncmp() (might want to change strncmp() to compare() or something)
#include <vector>
#include <sstream>
#include <iostream>
#include "Colors.hpp"

std::ostream &operator<<(std::ostream &o, std::vector<unsigned char>data);
// {
// 	for ( std::vector<unsigned char>::iterator it = data.begin(); it < data.end(); it++)
// 	{
// 		o << *it;
// 	}
// 	return(o);
// }

class Buffer
{
	public:
		Buffer() : _endLiteral("\r\n\r\n"), _data(0) {};
		unsigned char *requestEnded()
		{
			std::vector<unsigned char>::iterator it;
			unsigned char *position;

			// std::find_if(_data.begin(), _data.end(), std::vector<unsigned char>(_endLiteral.begin(), _endLiteral.end()));

			if (_data.size() < _endLiteral.length())
				return (NULL);
			for (it = _data.begin(); it != _data.end() - (_endLiteral.length() - 1); it++)
			{
				position = &(*it);
				// std::cout << "SEGF" << _data.end() - it << " " << _endLiteral.length()<< std::vector<unsigned char>(it, _data.end()) << std::endl;
				if (strncmp((char *)position, _endLiteral.c_str(), _endLiteral.length()) == 0)	//J removed std::, because strncmp() is a C function
				{
					return (&(*(it + _endLiteral.length())));
				}
			}
			return (NULL);
		}
		
		void addToBuffer(char *data, size_t size) { _data.insert(_data.end(), data, data + size); };
		
		std::vector<unsigned char> spliceRequest()
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
			return (request);
		}

		ssize_t readChunkLength()
		{
			// Find the position of the CRLF sequence indicating the end of the chunk size
			unsigned long endOfLinePos = std::string::npos;
			std::string lineEndLiteral("\r\n");
			for (unsigned long i = 0; i < _data.size() - (lineEndLiteral.size() + 1); ++i)
			{
			    bool found = true;
			    for (unsigned long j = 0; j < lineEndLiteral.size(); ++j)
				{
			        if (_data[i + j] != lineEndLiteral[j])
					{
			            found = false;
			            break;
			        }
			    }
			    if (found)
				{
			        endOfLinePos = i;
			        break;
			    }
			}


			// If the CRLF sequence is not found, there is not enough data in the buffer
			if (endOfLinePos == std::string::npos)
			{
				return (-1);
			}	

			// Extract the chunk length as a hexadecimal string
			std::string chunkSizeStr(_data.begin(), _data.begin() + endOfLinePos);
			// Convert the hexadecimal string to an integer
			size_t chunkSize;
			std::stringstream strStream(chunkSizeStr);
			strStream >> std::hex >> chunkSize;
			if (strStream.fail())
			{
				//* need better return here to indicate failure
				return (-1);
			}
			// Move the iterator past the CRLF sequence to remove the chunk length from the buffer

	        if (_data.size() >= chunkSize + endOfLinePos + lineEndLiteral.length())
			{
				_data.erase(_data.begin(), _data.begin() + endOfLinePos + lineEndLiteral.length());
	        }
			//std::cout << "Remaining data after reading len = " << _data << "\nwith chunkSize: " << chunkSize << std::endl;
			return (chunkSize);
		}

	    std::vector<unsigned char> extractChunk(size_t chunkSize)
		{
	        if (_data.size() < chunkSize)
			{
				return (std::vector<unsigned char>());
	        }
	        std::vector<unsigned char> chunk(_data.begin(), _data.begin() + chunkSize);
	        _data.erase(_data.begin(), _data.begin() + chunkSize);
			//std::cout << "Remaining data after extracting chunk = " << _data << "\nwith chunkSize: " << chunkSize << std::endl;
	        return (chunk);
	    }

		size_t getSize() { return(_data.size()); };
		unsigned char *getBegin() { return(&_data[0]); }
		std::vector<unsigned char>::iterator getEnd() { return(_data.end()); }
		std::vector<unsigned char> &getData() { return (_data); };
		void clear() { _data.clear(); }
	
	private:
		std::string _endLiteral;
		std::vector<unsigned char> _data;
};

#endif
