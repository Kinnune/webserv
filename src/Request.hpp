
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "Buffer.hpp"
#include "Host.hpp"
#include "ConfigurationFile.hpp"

std::ostream &operator<<(std::ostream &o, std::vector<unsigned char>data);

class Request
{
public:
	Request();
	~Request();
	Request (Request const &other);
	Request &operator=(Request const &other);
	Request(std::vector<unsigned char> content);
	Request(std::vector<unsigned char> content, ConfigurationFile config);
	void clear();
	int firstLineParse(std::vector<unsigned char> &line);
	int headerLineParse(std::vector<unsigned char> &line);
	int parseContent(std::vector<unsigned char> &data);
	bool detectContentlength();
	bool tryToComplete(Buffer &buffer);

	// Getters
	int getErrorCode();
	bool getIsValid();
	bool getIsChunked();
	std::string getVersion();
	bool getIsComplete();
	ssize_t getContentLength();
	void setContentLength(ssize_t value);
	std::string &getTarget();
	std::vector<unsigned char> &getBody();
	Host &getHost();
	int getMaxBodySizeAllowed();
	std::string const &getMethod() const;
	std::unordered_map<std::string, std::string> &getHeaders();

	// Setters
	void setIsComplete(bool value);
	void setTarget(std::string target);
	void setIsChunked(bool chunked);
	void setErrorCode(int errorCode);
private:
	Host _host;
	std::string _method;
	std::string _target;
	std::string _version;
	std::unordered_map<std::string, std::string> _headers;
	std::vector<unsigned char> _body;
	bool _completed;
	bool _isValid;
	bool _isChunked;
	ssize_t _contentLength;
	int _errorCode;
};

std::vector<unsigned char> getLine(std::vector<unsigned char> &data, size_t index);
bool lineEmpty(std::vector<unsigned char> &line);
bool isWS(unsigned char c);
int skipWS(std::vector<unsigned char> &data, ssize_t index);
int skipToWS(std::vector<unsigned char> &data, ssize_t index);
bool validIndex(std::vector<unsigned char> &data, ssize_t index);
int seekColon(std::vector<unsigned char> &data, ssize_t 	index);

#endif
