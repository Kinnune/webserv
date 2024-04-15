
#include "Response.hpp"

Response::Response(Request &request)
	: _request(request)
{
	_version = _request.getVersion();			
	_statusCode = "";
	_statusMessage = "";
	_headers = request.getHeaders();
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
	_statusCode = std::to_string(status);
	switch (status)
	{
		case 200:
			_statusMessage = "OK";
			break ;
		case 404:
			_statusMessage = "Not Found";
			_version = "HTTP/1.1";
			body404();
			break ;

	}
}

void removeFirstCharIfMatches(std::string& str, char matchChar)
{
	if (!str.empty() && str[0] == matchChar)
	{
		str.erase(0, 1);
	}
}

std::string getFileExtension(const std::string &filePath)
{
    size_t dotPosition = filePath.find_last_of('.');

    if (dotPosition != std::string::npos)
	{
        return (filePath.substr(dotPosition + 1));
    }
    return "";
}

std::string detectContentType(const std::string &filePath)
{
	std::string fileExtension = getFileExtension(filePath);

	if (fileExtension == "html") 
	{
	    return "text/html";
	} 
	else if (fileExtension == "css") 
	{
	    return "text/css";
	} 
	else if (fileExtension == "jpeg" || fileExtension == "jpg") 
	{
	    return "image/jpeg";
	} 
	else if (fileExtension == "png") 
	{
	    return "image/png";
	} 
	else if (fileExtension == "gif") 
	{
	    return "image/gif";
	} 
	else if (fileExtension == "json") 
	{
	    return "application/json";
	} 
	else if (fileExtension == "xml") 
	{
	    return "application/xml";
	} 
	else if (fileExtension == "pdf") 
	{
	    return "application/pdf";
	} 
	else if (fileExtension == "mp3") 
	{
	    return "audio/mpeg";
	} 
	else if (fileExtension == "mp4") 
	{
	    return "video/mp4";
	} 
	else if (fileExtension == "webm") 
	{
	    return "video/webm";
	} 
	else if (fileExtension == "txt") 
	{
	    return "text/plain";
	} 
	else if (fileExtension == "ico") 
	{
	    return "image/x-icon";
	} 
	else 
	{
	    return "application/octet-stream";
	}
}

#include "Colors.hpp"

void Response::getMethod()
{
	std::string filePath = _request.getTarget();
	//**hardcoded favicon for now this whole filepath thing is not done correctly now for now
	std::cout << CYAN << filePath << RESET << std::endl;
	if (filePath.find("favicon.ico") != std::string::npos)
	{
		filePath = "www/favicon.ico";
	}

	removeFirstCharIfMatches(filePath, '/');

	// std::string filePath = "www/index.html";
	// if (!std::__fs::filesystem::exists(filePath))
	// {
	// 	// 404
	// 	setStatus(404);
	// 	std::cerr << "File does not exist." << std::endl;
	// 	return ;
	// }
	std::ifstream file(filePath, std::ios::binary);
	if (!file)
	{
		// 500 internal server error
		std::cerr << "Failed to open file." << std::endl;
		setStatus(404);
		return ;
	}
	_body = std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
	file.close();
	if (!file)
	{
		// 500 internal server error
		setStatus(404);
		std::cerr << "Failed to read file." << std::endl;
		return ;
	}
	std::cout << "Number of bytes read: " << _body.size() << std::endl;
	// for (unsigned char byte : _body)
	// {
	// 	std::cout << static_cast<int>(byte) << " ";
	// }
	setStatus(200);
	//something wrong with version, shows up as "  TP/1.1"
	_version = "HTTP/1.1";
	setContentLengthHeader(_body.size());
	_headers["Content-Type"] = detectContentType(filePath);

	// _headers["Content-Type"] = "text/html; charset=utf-8";

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
