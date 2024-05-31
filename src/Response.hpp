
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <cstdio>	// remove()

#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <dirent.h>

#include "Request.hpp"
#include "ConfigurationFile.hpp"
#include "Colors.hpp"

class ConfigurationFile;



class Response
{
public:
	Response();
	Response(Request &request, std::string sessionID);
	~Response();

	int completeResponse();
	void setStatus(int status);
	bool supportedCGI();
	std::string toString();
	void setContentLengthHeader(size_t length);
	std::string detectContentType(const std::string &filePath);
	int doCGI();
	bool hasRequest();
	Request &getRequest();
	bool getWaitCGI();
	bool getRunCGI();
	void setCGIEnvironmentVariables(char **envp);
	void killChild();
	std::string listDirectory(std::string path);
	void writePipe();
	void readPipe();
	bool childReady();
	// Method handlers
	void handleGetMethod();
	void handlePostMethod();
	void handleDeleteMethod();
	void generateErrorPage();
	int _statusCodeInt;
	std::string _version;
	std::string _statusCode;
	std::string _statusMessage;
	std::unordered_map<std::string, std::string> _headers;
	std::vector<unsigned char> _body;
private:
	bool _runCGI;
	bool _waitCGI;
	bool _readPipe;
	bool _writePipe;
	bool _completed;
	int _pipeChild[2];
	int _pipeParent[2];
	pid_t _pid;
	Request _request;
	Host _host;
};

#endif