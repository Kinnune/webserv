
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "Buffer.hpp"

std::ostream &operator<<(std::ostream &o, std::vector<unsigned char>data);


// -1 is magic number for chunked requests
#define CHUNKED_REQUEST -1

#define DEBUG 0

class Request
{
	public:
		Request();
		~Request();
		Request (Request const &other);
		Request &operator=(Request const &other);
		Request(std::vector<unsigned char> content);
		void clear();
		void printRequest();
		int firstLineParse(std::vector<unsigned char> &line);
		int headerLineParse(std::vector<unsigned char> &line);
		int parseContent(std::vector<unsigned char> &data);
		bool detectContentLenght();
		std::string const &getMethod() const;
		std::string &getTarget() { return (_target); }
		bool tryToComplete(Buffer &buffer);
		bool getIsValid() { return (_isValid); };
		std::string getVersion() { return (_version); };
		bool getIsComplete() { return (_completed); }
		std::vector<unsigned char> getBody() { return (_body); };
		std::unordered_map<std::string, std::string> &getHeaders();
	private:
		std::string _method;
		std::string _target;
		std::string _version;
		std::unordered_map<std::string, std::string> _headers;
		std::vector<unsigned char> _body;
		bool _completed;
		bool _isValid;
		bool _isChunked;
		//  will set _contentLength to -1 if chunked content
		ssize_t _contentLength;
};

std::vector<unsigned char> getLine(std::vector<unsigned char> &data, size_t index);
bool lineEmpty(std::vector<unsigned char> &line);
bool isWS(unsigned char c);
int skipWS(std::vector<unsigned char> &data, ssize_t index);
int skipToWS(std::vector<unsigned char> &data, ssize_t index);
bool validIndex(std::vector<unsigned char> &data, ssize_t index);
int seekColon(std::vector<unsigned char> &data, ssize_t 	index);

#endif
