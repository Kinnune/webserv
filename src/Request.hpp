
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>

enum METHOD_TYPE
{
	GET,
	POST,
	DELETE,
	METHOD_COUNT
};

class Request
{
	public:
		Request();
		~Request();
		Request (Request const &other);
		Request &operator=(Request const &other);
		Request(std::vector<unsigned char> &content);
		void copyHeader(Request const &other);
		int firstLine(std::vector<unsigned char>data);
		int parseContent(std::vector<unsigned char> &data);
	private:
		METHOD_TYPE _methodType;
		std::string _method;
		std::string _target;
		std::string _version;
		size_t _header_count;
		std::string *_names;
		std::string *_values;
		std::string _body;
		std::unordered_map<std::string, std::string> headers;

		//--THE ENUM MUST BE IN THIS ORDER--
		const std::string _supportedMethods[METHOD_COUNT] =
		{
			"GET",
			"POST",
			"DELETE"
		};
};

#endif
