#include "Client.hpp"
#include "Colors.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>		// access()
#include <sys/stat.h>	// stat()


//------------------------------------------------------------------------------
//	CONSTRUCTORS & DESTRUCTORS
//------------------------------------------------------------------------------

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
	_config = other._config;
	return (*this);
}

Client::Client(int serverFd, int port)
{
	socklen_t adressSize = sizeof(_address);

	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	_port = port;
	_fd = accept(serverFd, (struct sockaddr *)&_address, &adressSize);
	fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}


//------------------------------------------------------------------------------
//	GETTERS & SETTERS
//------------------------------------------------------------------------------

ConfigurationFile &Client::getConfig()
{
	return (_config);
}

void Client::setConfig(ConfigurationFile &config)
{
	_config = config;
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

//------------------------------------------------------------------------------
//	MEMBER FUNCTIONS
//------------------------------------------------------------------------------

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



// void Client::errorResponse(int status)
// {

// }


bool Client::fileExists(const std::string& path)
{
	std::string resourcePath;
	std::string responseStr;
	Response response(_request);

	responseStr = response.toString();
	std::cout << RED << responseStr << RESET <<std::endl;
	write(_fd, responseStr.c_str(), responseStr.length());
	return ;

	// if (_request.getMethod() == "GET")
	// {
	// 	resourcePath = std::string("../resources");
	// 	resourcePath.append(_request.getTarget());
	// 	if (DEBUG)
	// 		std::cout << "Resource path: '" << resourcePath << "'" << std::endl;

	// 	std::ifstream ifs(resourcePath);
	// 	std::string buffer((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	// 	std::string response;
	// 	std::unordered_map<std::string, std::string> headers;
	// 	std::unordered_map<std::string, std::string>::iterator it;

	// 	headers = _request.getHeaders();
	// 	response.append("HTTP/1.1 200 OK\n");
	// 	for (it = headers.begin(); it != headers.end(); it++)
	// 	{
	// 		response.append(it->first + ": ");
	// 		response.append(it->second);
	// 		response.append("\n");
	// 	}
	// 	if (_request.getTarget() == "../resources/favicon.ico")
	// 	{
	// 		// response.append("Content-Type: image/png");
	// 	}
	// 	response.append("Content-Type: text/html; charset=utf-8\n");
	// 	buffer.clear();
	// 	buffer = listDirectory(".");
	// 	response.append("Content-Length: ");
	// 	response.append(std::to_string(buffer.length()));
	// 	response.append("\r\n\r\n");
	// 	response.append(buffer);
	// 	write(_fd, response.c_str(), response.length());
	// 	if (DEBUG)
	// 		std::cout << "{" << response << "}[" << buffer << "]" << std::endl;
	// 	// memset(pageBuffer, 0, MAX_BUFFER_SIZE);
	// }
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
	ssize_t readCount = 0;	// changed to ssize_t instead of size_t, because read() returns -1 on error, and size_t is unsigned

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
