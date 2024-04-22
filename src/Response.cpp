#include "Response.hpp"
#include "Colors.hpp"


//------------------------------------------------------------------------------
//	CONSTRUCTORS & DESTRUCTORS
//------------------------------------------------------------------------------

std::string getFileExtension(const std::string &filePath)
{
	size_t dotPosition = filePath.find_last_of('.');

	if (dotPosition != std::string::npos)
	{
		return (filePath.substr(dotPosition + 1));
	}
	return "";
}

Response::Response()
	: _version(""),
		_statusCode(""),
		_statusMessage(""),
		_headers(std::unordered_map<std::string, std::string>()),
		_body(std::vector<unsigned char>()),
		_runCGI(false),
		_waitCGI(false),
		_request(Request())
{
	(void)_waitCGI;
};

Response::Response(Request &request)
	: _request(request)
{
	_runCGI = supportedCGI();
	_waitCGI = false;
	_statusCode = "";
	_statusMessage = "";
	_version = _request.getVersion();			
	_headers = request.getHeaders();
	_body.resize(0);
	if (DEBUG)
		std::cout << "----------RESPONSE----------\n" << *this << "----------------------------\n";
};

Response::~Response() {}


//------------------------------------------------------------------------------
//	OPERATOR OVERLOADS
//------------------------------------------------------------------------------

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


//------------------------------------------------------------------------------
//	GETTERS & SETTERS
//------------------------------------------------------------------------------

void Response::setContentLengthHeader(size_t length)
{
	_headers["Content-Length"] = std::to_string(length);
}

//------------------------------------------------------------------------------

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


//------------------------------------------------------------------------------
//	MEMBER FUNCTIONS
//------------------------------------------------------------------------------

int Response::completeResponse()
{
	if (supportedCGI())
	{
		if (_runCGI)
		{
			doCGI();
		}
		if (!_waitCGI)
		{
			setStatus(200);
			_version = "HTTP/1.1";
			setContentLengthHeader(_body.size());
			_headers["Content-Type"] = "text/html";
		}
		else
		{
			childReady();
			return (0);
		}
	}
	else if (_request.getMethod() == "GET")
	{
		getMethod();
	}
	return (1);
}

void Response::body404()
{
	std::string bodyStr("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>404 Not Found</title></head><body><h1>404 Not Found</h1><p>The page you are looking for could not be found.</p></body></html>");
	_body = std::vector<unsigned char>(bodyStr.begin(), bodyStr.end());
	setContentLengthHeader(_body.size());
	_headers["Content-Type"] = "text/html; charset=utf-8";
}

//------------------------------------------------------------------------------

void removeFirstCharIfMatches(std::string& str, char matchChar)
{
	if (!str.empty() && str[0] == matchChar)
	{
		str.erase(0, 1);
	}
}

//------------------------------------------------------------------------------

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


//------------------------------------------------------------------------------
//	CGI STUFF
//------------------------------------------------------------------------------

std::string Response::detectContentType(const std::string &filePath)
{
	std::string fileExtension = getFileExtension(filePath);

	//**Incase of javascript or python files we will run the script and provide its output as html
	if (supportedCGI())
	{
		return "text/html";
	}
	else if (fileExtension == "html")
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

//------------------------------------------------------------------------------

bool Response::supportedCGI()
{
	std::string fileExtension = getFileExtension(_request.getTarget());

	//**these values could/should be read from the config
	return (fileExtension == "js" | fileExtension == "py");

	return _supportedCGI;
}

//------------------------------------------------------------------------------

bool Response::childReady()
{
	int status;
	pid_t result;
	char buffer[1024];
	ssize_t bytesRead;

	result = waitpid(_pid, &status, WNOHANG);
	// If an error occurred while waiting, print an error message
	if (result == -1)
	{
		//** setStatus(500)
		std::cerr << "Waitpid error";
		return (true);
	}
	if (result == 0)
	{
		// If the child process is still running, optionally add a delay
		// to avoid busy-waiting. Here, usleep is used to sleep for 10 milliseconds.
		return (false);
	}
	else
	{
		while (true)
		{
			// Read data from pipe from child
			bytesRead = read(_pipeParent[0], buffer, sizeof(buffer));
			if (bytesRead > 0)
			{ // If data was read successfully
				std::cout.write(buffer, bytesRead); // Write data to standard output (client response)
			}
			else if (bytesRead == 0)
			{ // End of file reached (child process exited)
				break; // Exit loop
			}
			else if (errno != EAGAIN && errno != EWOULDBLOCK)
			{ // Error other than non-blocking
				std::cerr << "Read error";
				//** setStatus(500)
				break ;
			}
			std::string bufferStr(buffer, bytesRead);
			std::vector<unsigned char>tmpVector(bufferStr.begin(), bufferStr.end());
			for (unsigned char byte : tmpVector)
			{
				_body.push_back(byte);
			}
		}
		close(_pipeParent[0]); // Close read end of pipe from child
		_waitCGI = false;
		return (true);
	}
	return (false);
}

//------------------------------------------------------------------------------

int Response::doCGI()
{
	if (pipe(_pipeChild) == -1 || pipe(_pipeParent) == -1)
	{
		std::cerr << "Pipe creation failed";
		return 1;
	}
 // Fork a child process
	_pid = fork();

	if (_pid == -1)
	{ // Fork failed
		std::cerr << "Fork failed";
		return 1;
	}
	else if (_pid == 0)
	{ // Child process
		close(_pipeChild[1]); // Close unused write end of pipe to child
		close(_pipeParent[0]); // Close unused read end of pipe from parent

		// Redirect standard input and output to pipes
		dup2(_pipeChild[0], STDIN_FILENO); // Redirect stdin to read end of pipe from parent
		dup2(_pipeParent[1], STDOUT_FILENO); // Redirect stdout to write end of pipe to parent

		// Execute the CGI script
		//**hardcoding python example
		//**TODO_set bs properly
		const char *program = "/usr/bin/python";
		const char *argument = "/Users/jbagger/code/GitHub/webserv/www/test.py";
		const char *args[] = {program, argument, nullptr};
		//**TODO set enviroment variables according to request headers
		execve(program, const_cast<char* const*>(args), nullptr);
		std::cerr << "Exec failed"; // This line is executed only if execl fails
		return 1; // Exit child process with failure status
	}
	else
	{ // Parent process
		close(_pipeChild[0]); // Close unused read end of pipe to child
		close(_pipeParent[1]); // Close unused write end of pipe from parent

		// Set non-blocking mode for pipes
		fcntl(_pipeChild[1], F_SETFL, O_NONBLOCK); // Set write end of pipe to child as non-blocking
		fcntl(_pipeParent[0], F_SETFL, O_NONBLOCK); // Set read end of pipe from child as non-blocking

		// Write HTTP request data to the child process
		std::string requestData = std::string(_request.getBody().begin(), _request.getBody().end()); // Replace with actual request data
		write(_pipeChild[1], requestData.c_str(), requestData.size()); // Write request data to pipe to child
		close(_pipeChild[1]); // Close write end of pipe to child to signal end of input

		// Read response from the child process and send it to the client
		std::cout << GREEN << "ALL GOOD SOFAR" << RESET << std::endl;
	}
	_runCGI = false;
	_waitCGI = true;
	return 0; // Exit main process with success status
}


//------------------------------------------------------------------------------
//	HANDLE METHODS
//------------------------------------------------------------------------------

void Response::getMethod()
{
	std::string filePath = _request.getTarget();
	std::string contentType = detectContentType(filePath);
	//**hardcoded favicon for now this whole filepath thing is not done correctly for now
	if (filePath.find("favicon.ico") != std::string::npos)
	{
		filePath = "www/favicon.ico";
	}
	removeFirstCharIfMatches(filePath, '/');
	//**SEPARATE THIS INTO FILE READING FUNCTION
	std::ifstream file(filePath, std::ios::binary);
	if (!file)
	{
		// 500 internal server error
		std::cerr << "Failed to open file: " << filePath << std::endl;
		// std::cerr << "Failed to open file: " << color(filePath, RED) << std::endl;
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
	//something wrong with version, shows up as "  TP/1.1"
	setStatus(200);
	_version = "HTTP/1.1";
	setContentLengthHeader(_body.size());
	_headers["Content-Type"] = contentType;
}
