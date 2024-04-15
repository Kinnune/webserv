
#include "Response.hpp"

Response::Response(Request &request)
	: _request(request)
{
	std::unordered_map<std::string, std::string> headers;
	std::unordered_map<std::string, std::string>::iterator it;
	_version = _request.getVersion();			
	_statusCode = "";
	_statusMessage = "";
	headers = _request.getHeaders();
	for (it = headers.begin(); it != headers.end(); it++)
	{
		if (it ->first.find("Content-Length:") == std::string::npos)
		{
			_headers.insert(*it);
		}
	}
	_body.resize(0);
	completeResponse();
	if (DEBUG)
		std::cout << "----------RESPONSE----------\n" << *this << "----------------------------\n";
};

void Response::setContentLengthHeader(size_t length)
{
	_headers["Content-Length"] = std::to_string(length);
}


void Response::completeResponse()
{
	if (_request.getMethod() == "GET")
	{
		getMethod();
	}
}

void Response::body404()
{
	std::string bodyStr("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>404 Not Found</title></head><body><h1>404 Not Found</h1><p>The page you are looking for could not be found.</p></body></html>");
	_body = std::vector<unsigned char>(bodyStr.begin(), bodyStr.end());
	setContentLengthHeader(_body.size());
	_headers["Content-Type"] = "text/html; charset=utf-8";
}

void Response::setStatus(int status)
{
	_statusCode = status;
	switch (status)
	{
		case 200:
			_statusMessage = "OK";
			break ;
		case 404:
			_statusMessage = "Not Found";
			body404();
			break ;

	}
}

void Response::getMethod()
{
	// std::string filePath = _request.getTarget();
	std::string filePath = "www/builders.html";

	if (!std::__fs::filesystem::exists(filePath))
	{
		// 404
		setStatus(404);
		std::cerr << "File does not exist." << std::endl;
		return ;
	}
	std::ifstream file(filePath, std::ios::binary);
	if (!file)
	{
		// 500 internal server error
		std::cerr << "Failed to open file." << std::endl;
		return ;
	}
	_body = std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
	file.close();
	if (!file)
	{
		// 500 internal server error
		std::cerr << "Failed to read file." << std::endl;
		return ;
	}
	std::cout << "Number of bytes read: " << _body.size() << std::endl;
	for (unsigned char byte : _body)
	{
		std::cout << static_cast<int>(byte) << " ";
	}
	setStatus(200);
	//something wrong with version, shows up as "  TP/1.1"
	_version = "HTTP/1.1";
	setContentLengthHeader(_body.size());
	// _headers["Content-Type"] = detectContentType(_request.getTarget());
	_headers["Content-Type"] = "text/html; charset=utf-8";
	std::cout << std::endl;
}

std::string Response::toString()
{
	std::stringstream responseStream;

	responseStream << _version << " " << _statusCode << " " << _statusMessage << "\r\n";
	for (std::pair<std::string, std::string> header : _headers)
	{
		responseStream << header.first << ": " << header.second << "\r\n";
	}
	responseStream << "\r\n";
	for (unsigned char byte : _body)
	{
		responseStream << byte;
	}
	return (responseStream.str());
}

std::ostream &operator<<(std::ostream &o, Response response)
{
	o << response._version << " " << response._statusCode << " " << response._statusMessage << "\r\n";
	for (std::pair<std::string, std::string> header : response._headers)
	{
		o << header.first << ": " << header.second << "\n";
	}
	o << "\r\n\r\n" << std::string(response._body.begin(), response._body.end()) << std::endl;
	return (o);
}
