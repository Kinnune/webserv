
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

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

class Response;
class ConfigurationFile;

std::ostream &operator<<(std::ostream &o, Response response);


class Response
{
	public:
		Response();
		Response(Request &request, std::string sessionID);
		~Response();

		int completeResponse();
		void setStatus(int status);
		void body404();
		bool supportedCGI();
		std::string toString();
		void setContentLengthHeader(size_t length);
		std::string detectContentType(const std::string &filePath);
		int doCGI();
		Request &getRequest() { return (_request); }
		bool hasRequest() { return (_request.getIsComplete()); }
		bool childReady();
		bool getWaitCGI() { return (_waitCGI); }
		bool getRunCGI() { return (_runCGI); }
		void setCGIEnvironmentVariables(char **envp);
		void killChild();
		std::string listDirectory(std::string path);

		// Method handlers
		void handleGetMethod();
		void handlePostMethod();
		void handleDeleteMethod();
		void generateErrorPage();

		std::string _version;
		std::string _statusCode;
		std::string _statusMessage;
		std::unordered_map<std::string, std::string> _headers;
		std::vector<unsigned char> _body;


	private:
		bool _runCGI;
		bool _waitCGI;
		int _pipeChild[2];
		int _pipeParent[2];
		pid_t _pid;
		Request _request;
		Host _host;
};

#endif
