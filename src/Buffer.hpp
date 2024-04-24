
#ifndef BUFFER_HPP
#define BUFFER_HPP
#include <string.h>	//J added for strncmp() (might want to change strncmp() to compare() or something)

class Buffer
{
	public:
		Buffer() : _endLiteral("\r\n\r\n") {};
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
		
		size_t getSize() { return(_data.size()); };
		unsigned char *getBegin() { return(&_data[0]); }
		std::vector<unsigned char>::iterator getEnd() { return(_data.end()); }
		std::vector<unsigned char> &getData() { return (_data); };
	
	private:
		std::string _endLiteral;
		std::vector<unsigned char> _data;
};

#endif
