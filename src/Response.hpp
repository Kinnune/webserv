
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "Request.hpp"
#include "ConfigurationFile.hpp"

class Response;
class ConfigurationFile;

std::ostream &operator<<(std::ostream &o, Response response);


class Response
{
	public:
		Response(Request &request);
		~Response();

		void completeResponse();
		void setStatus(int status);
		void body404();
		void getMethod();
		std::string toString();
		void setContentLengthHeader(size_t length);

		std::string _version;
		std::string _statusCode;
		std::string _statusMessage;
		std::unordered_map<std::string, std::string> _headers;
		std::vector<unsigned char> _body;

	private:
		Request _request;
};

#endif
