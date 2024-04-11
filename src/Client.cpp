#include "Client.hpp"
#include "Colors.hpp"
#include <iostream>
#include <fstream>

Client::Client()
	: _request(Request())
{}

Client::~Client() {}

Client::Client(Client const &other)
{
	*this = other;
}

Client &Client::operator=(Client const &other)
{
	_fd = other._fd;
	_port = other._port;
	return (*this);
}

Client::Client(int serverFd, int port)
{
	socklen_t adressSize = sizeof(_address);

	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	_fd = accept(serverFd, (struct sockaddr *)&_address, &adressSize);
	fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

void Client::setFd(int fd)
{
	_fd = fd;
}

void Client::setPort(int port)
{
	_port = port;
}

int Client::getFd() const
{
	return (_fd);
}

int Client::getPort() const
{
	return (_port);
}


//read to buffer until request done;

void mockResponse(int fd)
{
	std::ifstream src;
	static const int MAX_BUFFER_SIZE = 100000;
	char bufferPic[MAX_BUFFER_SIZE + 1];

	src.open("../resources/zeus.jpg", std::ofstream::binary | std::ofstream::in);
	if (!src.good())
	{
		std::cerr << "Open failed" << std::endl; 
		return ;
	}
	src.read(bufferPic, 100000);
	int streamSize = src.gcount();
	bufferPic[streamSize] = '\0';
	std::string response("HTTP/1.1 200 OK\nRequest status code: 200 OK\nContent-Length: " + std::to_string(streamSize) +"\nContent-Type: image/jpeg\n\n");
	response.append(bufferPic, streamSize);
	write(fd, response.c_str(), response.length());
	memset(bufferPic, 0, MAX_BUFFER_SIZE);
	// std::cout << "\n{\n" << response << "\n}\n" << std::endl;

}


class Response
{
	public:
		Response(Request &request)
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
			if (DEBUG)
				std::cout << "----------RESPONSE----------\n" << *this << "----------------------------\n";
		};

		std::string _version;
		std::string _statusCode;
		std::string _statusMessage;
		std::unordered_map<std::string, std::string> _headers;
		std::vector<unsigned char> _body;

	private:
		Request _request;
};

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

// void Client::errorResponse(int status)
// {

// }

void Client::respond()
{
	std::string resourcePath;

	Response response(_request);
	if (_request.getMethod() == "GET")
	{
		resourcePath = std::string("../resources");
		resourcePath.append(_request.getTarget());
		if (DEBUG)
			std::cout << "Resource path: '" << resourcePath << "'" << std::endl;

		std::ifstream ifs(resourcePath);
		std::string buffer((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
		std::string response;
		std::unordered_map<std::string, std::string> headers;
		std::unordered_map<std::string, std::string>::iterator it;

		headers = _request.getHeaders();
		response.append("HTTP/1.1 200 OK\n");
		for (it = headers.begin(); it != headers.end(); it++)
		{
			response.append(it->first + ": ");
			response.append(it->second);
			response.append("\n");
		}
		if (_request.getTarget() == "../resources/favicon.ico")
		{
			// response.append("Content-Type: image/png");
		}
		response.append("Content-Type: text/html; charset=utf-8\n");
		buffer.clear();
		buffer = listDirectory(".");
		response.append("Content-Length: ");
		response.append(std::to_string(buffer.length()));
		response.append("\r\n\r\n");
		response.append(buffer);
		write(_fd, response.c_str(), response.length());
		if (DEBUG)
			std::cout << "{" << response << "}[" << buffer << "]" << std::endl;
		// memset(pageBuffer, 0, MAX_BUFFER_SIZE);
	}

}

std::string Client::listDirectory(std::string path)
{
	std::string directoryListResponse;
	DIR* directory = opendir(path.c_str());
	struct dirent* entry;

	if (directory != nullptr)
	{
        directoryListResponse.append("<table border=\"1\">");
        directoryListResponse.append("<tr><th>File Name</th></tr>");

		while ((entry = readdir(directory)) != nullptr)
		{
            directoryListResponse.append("<tr><td>" + std::string(entry->d_name) + "</td></tr>");
			if (DEBUG)
				std::cout << entry->d_name << std::endl;
		}

        directoryListResponse.append("</table>");
		if (DEBUG)
	        std::cout << "</table>" << std::endl;
		closedir(directory);
	}
	else
	{
		std::cerr << "Unable to open directory: " << path << std::endl;
	}
	return (directoryListResponse);
}

void Client::handleEvent(short events)
{
	//* we might want to malloc this buffer
	static const int MAX_BUFFER_SIZE = 4095;
	static char buffer[MAX_BUFFER_SIZE + 1];
	size_t readCount = 0;

	if (events & POLLOUT)
	{
		static int i = 0;
		if (_request.getIsComplete() || _request.tryToComplete(_buffer))
		{
			if (DEBUG)
				std::cout << RED << "i: " << i << RESET << std::endl;
			if (DEBUG)
				_request.printRequest();
			respond();
			_request.clear();
			// mockResponse(_fd);
			i++;
		}
	}
	if (events & POLLIN)
	{
		if (DEBUG)
			std::cout << GREEN << "READING\n" << std::endl; 
		readCount = read(_fd, buffer, MAX_BUFFER_SIZE);
		if (readCount < 0)
		{
			throw (std::runtime_error("EXCEPTION: reading failed"));
		}
		buffer[readCount] = '\0';
		_buffer.addToBuffer(&buffer[0], readCount);

	}
	if (!_request.getIsComplete() && _buffer.requestEnded())
	{
		if (DEBUG)
			std::cout << GREEN << "double newline found" << RESET << std::endl;
		if (_buffer.getSize())
			if (DEBUG)
				std::cout << CYAN << _buffer.getData() << RESET << std::endl;
		try
		{
			_request = Request(_buffer.spliceRequest());
		}
		catch (std::exception &e)
		{
			std::cerr << "EXCEPTION OCCURRED: " << e.what() << std::endl;
		}
		if (!_request.getIsValid())
		{
			//  respond something
			//  close connection
		}
	}
	else
	{
		// std::cout << RED << "double newline NOT found" << RESET << std::endl;
	}
	// std::cout << _buffer.getData();
}

std::ostream &operator<<(std::ostream &o, std::vector<unsigned char>data)
{
	o << "{";
	for (std::vector<unsigned char>::iterator it = data.begin(); it < data.end(); it++)
	{
		o << *it;
		// o << "'" << (int)(*it) << "'" << *it;
	}
	o << "}";
	return(o);
}
