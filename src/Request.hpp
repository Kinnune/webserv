
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>

class Request
{
	public:
		Request();
		~Request();
		Request (Request const &other);
		Request &operator=(Request const &other);
		Request(std::vector<unsigned char> &content);
		void printRequest();
		int firstLineParse(std::vector<unsigned char> &line);
		int headerLineParse(std::vector<unsigned char> &line);
		int parseContent(std::vector<unsigned char> &data);
		bool detectContentLenght();

	private:
		std::string _method;
		std::string _target;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
		bool _completed;
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
