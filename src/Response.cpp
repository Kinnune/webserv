#include "Response.hpp"
#include "Colors.hpp"
#include "Server.hpp"
// #include "utilityHeader.hpp"

//------------------------------------------------------------------------------
//	CONSTRUCTORS & DESTRUCTORS
//------------------------------------------------------------------------------

std::string getFileExtension(const std::string &filePath)
{
	size_t dotPosition = filePath.find_last_of('.');

	if (dotPosition != std::string::npos)
	{
		return (filePath.substr(dotPosition));
	}
	return "";
}

Response::Response()
	: _statusCodeInt(0),
		_version(""),
		_statusCode(""),
		_statusMessage(""),
		_headers(std::unordered_map<std::string, std::string>()),
		_body(std::vector<unsigned char>()),
		_runCGI(false),
		_waitCGI(false),
		_readPipe(false),
		_writePipe(false),
		_pid(0),
		_request(Request())
{
	(void)_waitCGI;
};

Response::Response(Request &request, std::string sessionID)
	: _request(request)
{
	_pid = 0;
	_runCGI = supportedCGI();
	_statusCodeInt = 0;
	_statusCode = "";
	_statusMessage = "";
	_version = _request.getVersion();			
	_headers = request.getHeaders();
	_host = request.getHost();
	_body.resize(0);
	_waitCGI = false;
	_readPipe = false;
	_writePipe = false;
	_runCGI = supportedCGI();
	if (_headers.find("Cookie") == _headers.end())
	{
		_headers["Set-Cookie"] = "session_id=" + sessionID;
	}
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
	_version = "HTTP/1.1";
	switch (status)
	{
		case 200:
			_statusMessage = "OK";
			break ;
		case 201:
			_statusMessage = "Created";
			break ;
		case 204:
			_statusMessage = "No Content";
			break ;
		case 301:
			_statusMessage = "Moved Permanently";
			break ;
		case 400:
			_statusMessage = "Bad Request";
			break ;
		case 403:
			_statusMessage = "Forbidden";
			break ;
		case 404:
			_statusMessage = "Not Found";
			break ;
		case 405:
			_statusMessage = "Method Not Allowed";
			break ;
		case 500:
			_statusMessage = "Internal Server Error";
			break ;
		case 501:
			_statusMessage = "Not Implemented";
			break ;
		default:
			_statusMessage = "Internal Server Error";
			break ;
	}
}

void Response::generateErrorPage()
{
	// Search for custom error page
	for (std::pair<std::string, std::string> errorPage : _host.getErrorPages())
	{
		if (errorPage.first == _statusCode)
		{
			std::string errorPageName = errorPage.second + "/" + _statusCode + ".html";
			if (_host.isFile(errorPageName) == false)
			{
				break ;
			}
			std::ifstream file(errorPageName, std::ios::binary);
			_body = std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
			setContentLengthHeader(_body.size());
			_headers["Content-Type"] = "text/html";
			return ;
		}
	}

	// Default error page
	std::string errorPage = "<!DOCTYPE html> \
	<html lang=\"en\"> \
	<head> \
	<title>Error: " + _statusCode + "</title> \
	</head> \
	<body> \
		<h1>Oops! Something went wrong.</h1> \
		<p>Error, " + _statusCode + " (" + _statusMessage + ")," + " occurred while processing your request.</p> \
	</body> \
	</html>";
	_body = std::vector<unsigned char>(errorPage.begin(), errorPage.end());
	setContentLengthHeader(_body.size());
	_headers["Content-Type"] = "text/html";
}

//------------------------------------------------------------------------------
//	MEMBER FUNCTIONS
//------------------------------------------------------------------------------


void Response::killChild()
{
	if (_readPipe)
	{
		Server::getInstance().removeFd(_pipeParent[0]);
	}
	if (_writePipe)
	{
		Server::getInstance().removeFd(_pipeChild[1]);
	}
	if (_waitCGI && _pid > 0)
	{
		kill(_pid, SIGKILL);
	}
}

void printBits(short num)
{
	int i = sizeof(short) * 8;
	i--;
	for (; i >= 0; i--)
	{
		std::cout << ((num >> i) & 1);
	}
	std::cout << std::endl;
}

void Response::writePipe()
{
	static int writeOffset = 0;
	int tmpOffset;
	Server &server = Server::getInstance();

	if (!(server.getEventsByFd(_pipeChild[1]) & POLLOUT))
	{
		return ;
	}
	std::vector<unsigned char> bodyData = _request.getBody();
	std::string requestData(bodyData.begin() + writeOffset, bodyData.end());
	tmpOffset = writeOffset;
	tmpOffset +=  write(_pipeChild[1], requestData.c_str(), requestData.size());
	server.setDidIO(_pipeChild[1]);
	if (tmpOffset < writeOffset)
	{
		//**WRITE ERROR
	}
	if (writeOffset - tmpOffset <= 0)
	{
		close(_pipeChild[0]);
		close(_pipeParent[1]);
		writeOffset = 0;
		_writePipe = false;
 		server.removeFd(_pipeChild[1]);
	}
}

void Response::readPipe()
{
	char buffer[1024];
	ssize_t bytesRead = -1;
	Server &server = Server::getInstance();

	// std::cout << "Server events: " << server.getEventsByFd(_pipeParent[0]) << std::endl;
	// if (server.getEventsByFd(_pipeParent[0]) & POLLIN)
	// {
		bytesRead = read(_pipeParent[0], buffer, sizeof(buffer));
		server.setDidIO(_pipeParent[0]);
	// }
	// else
	// {
	// 	// return ;
	// }
	if (bytesRead > 0)
	{ // If data was read successfully
		// std::cout.write(buffer, bytesRead); // Write data to standard output (client response)
	}
	else if (bytesRead == 0)
	{ // End of file reached (child process exited)
		_waitCGI = false;
		_readPipe = false;
		close(_pipeChild[1]);
		close(_pipeParent[0]);
		server.removeFd(_pipeParent[0]);
		return ;
	}
	else if (bytesRead < 0)
	{ // Error other than non-blocking
		std::cerr << "Read error";
		//** setStatus(500)
		return ;
	}
	std::string bufferStr(buffer, bytesRead);
	// std::cout << color(bufferStr, YELLOW) << std::endl;
	// exit(0);
	std::vector<unsigned char>tmpVector(bufferStr.begin(), bufferStr.end());
	for (unsigned char byte : tmpVector)
	{
		_body.push_back(byte);
	}

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

std::string Response::listDirectory(std::string path)
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
            directoryListResponse.append("<tr><td><a href=\""+ std::string(entry->d_name) +"\">" + std::string(entry->d_name) + "</a></td></tr>");
		}

        directoryListResponse.append("</table>");
		closedir(directory);
	}
	else
	{
		std::cerr << "Unable to open directory: " << path << std::endl;
	}
	return (directoryListResponse);
}

//------------------------------------------------------------------------------
//	CGI STUFF
//------------------------------------------------------------------------------

std::string Response::detectContentType(const std::string &filePath)
{
	std::string fileExtension = getFileExtension(filePath);

	if (!fileExtension.empty() && fileExtension[0] == '.')
	{
		fileExtension = fileExtension.substr(1);
	}
	// std::cout << "file extension recognized as: " << fileExtension << std::endl;
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
	if (_host.isAllowedCGI(_request.getTarget(), fileExtension))
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------

bool Response::childReady()
{
	int status;
	pid_t result;

	result = waitpid(_pid, &status, WNOHANG);
	if (result == 0)
	{
		return (false);
	}
	else
	{
		return (true);
	}
	return (false);
}

//------------------------------------------------------------------------------

#define MAX_ENV_VARS 100 

void Response::setCGIEnvironmentVariables(char **envp)
{
    int index = 0;

    envp[index++] = strdup(("SERVER_PROTOCOL=" + _version).c_str());
    envp[index++] = strdup(("REQUEST_METHOD=" + _request.getMethod()).c_str());

    for (std::pair<std::string, std::string> header : _headers)
	{
        std::string envVarName = "HTTP_" + header.first;
        std::string envVarValue = header.second;
        std::replace(envVarName.begin(), envVarName.end(), '-', '_'); // Replace '-' with '_'
        std::transform(envVarName.begin(), envVarName.end(), envVarName.begin(), ::toupper); // Convert to uppercase
        envp[index++] = strdup((envVarName + "=" + envVarValue).c_str());
    }
    std::string contentLength = std::to_string(_body.size());
    envp[index++] = strdup(("CONTENT_LENGTH=" + contentLength).c_str());

    // Set the last element of the array to nullptr as required by execve
    envp[index] = nullptr;
}

//------------------------------------------------------------------------------
int Response::doCGI()
{

	int statusCodeInt = _statusCodeInt;
	std::string program = _host.getInterpreter(_request.getTarget(), getFileExtension(_request.getTarget()));
	std::string path = _host.updateResourcePath(_request.getTarget(), statusCodeInt);
	std::cout << "Requested path: " << _request.getTarget() << std::endl;
	std::cout << "Updated path: " << path << std::endl;
	_statusCodeInt = statusCodeInt;
	if (!_host.isFile(path))
	{
		std::cerr << "Is NOT a file" << std::endl;
		setStatus(404);
		generateErrorPage();
		return (1);
	}
	std::cerr << "IS a file" << std::endl;
	
	Server &server = Server::getInstance();
	// std::cout << "path: " << path << " " << access(path.c_str(), F_OK) << std::endl;
	// std::cout << "status: " << _statusCodeInt << std::endl;
	// if (access(_host.updateResourcePath(_request.getTarget(), _statusCodeInt).c_str(), F_OK) == -1)
	// {
	// 	std::cout << "No access" << std::endl;
	// 	setStatus(404);
	// 	generateErrorPage();
	// 	_runCGI = false;
	// 	return (0);
	// }
	if (pipe(_pipeChild) == -1 || pipe(_pipeParent) == -1)
	{
		std::cerr << "Pipe creation failed";
		setStatus(500);
		generateErrorPage();
		return (1);
	}
	_pid = fork();
	if (_pid == -1)
	{
		std::cerr << "Fork failed";
		setStatus(500);
		generateErrorPage();
		return (1);
	}
	else if (_pid == 0)
	{ // Child process
		close(_pipeChild[1]);
		close(_pipeParent[0]);
		dup2(_pipeChild[0], STDIN_FILENO);
		dup2(_pipeParent[1], STDOUT_FILENO);

		std::string argument = path; 
		if (getFileExtension(_request.getTarget()) == ".out")
		{
			program = argument;
		}
		std::cerr << RED <<  "Program: " << program << RESET << std::endl;
		const char *args[] = {program.c_str(), argument.c_str(), nullptr};
		char **env = (char **)malloc(sizeof(char*) * (MAX_ENV_VARS + 1));
		setCGIEnvironmentVariables(env);
		std::cerr << "Exec: " << program << std::endl;;
		execve(program.c_str(), const_cast<char* const*>(args), const_cast<char* const*>(env));

		exit(EXIT_FAILURE);
		setStatus(500);
		generateErrorPage();
		return (1);
	}
	else
	{ // Parent process
		fcntl(_pipeChild[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		fcntl(_pipeParent[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		server.newFd(_pipeChild[1]);
		server.newFd(_pipeParent[0]);
		_writePipe = true;
		_readPipe = true;
		_statusCode = "200";
		_statusCodeInt = 200;
	}
	_runCGI = false;
	_waitCGI = true;
	return (0);
}

//------------------------------------------------------------------------------
//	HANDLE METHODS
//------------------------------------------------------------------------------
int Response::completeResponse()
{
	/*
		Added check for content type that isn't urlencoded.
		If it isn't, return 501 (Not Implemented).
		Don't know yet how to get multiform data, but we're not handling it at the moment. Therefore the check.
	*/
	if (_headers.find("Content-Type") != _headers.end() && _headers["Content-Type"] != "application/x-www-form-urlencoded")
	{
		setStatus(501);
		generateErrorPage();
		return (1);
	}

	if (supportedCGI())
	{
		if (_runCGI)
		{
			// std::cout << "Running CGI" << std::endl;
			doCGI();
			return (0);
		}
		if (!_waitCGI)
		{
			// std::cout << "CGI ready" << std::endl;
			setStatus(200);
			_version = "HTTP/1.1";
			setContentLengthHeader(_body.size());
			_headers["Content-Type"] = "text/html";
		}
		else if (_writePipe)
		{
			// std::cout << "Writing pipe" << std::endl;
			writePipe();
			return (0);
		}
		else if (!childReady())
		{
			return (0);
		}
		else if (_readPipe)
		{
			// std::cout << "Reading pipe" << std::endl;
			readPipe();
			return (0);
		}
	}
	else if (_request.getMethod() == "GET")
	{
		handleGetMethod();
	}
	else if (_request.getMethod() == "POST")
	{
		handlePostMethod();
	}
	else if (_request.getMethod() == "DELETE")
	{
		// handleDeleteMethod();
	}
	return (1);
}
//------------------------------------------------------------------------------

void Response::handleGetMethod()
{
	// update path and status code
	std::string filePath = _host.updateResourcePath(_request.getTarget(), _statusCodeInt);

	// Check if the requested method is allowed
	if (_host.isAllowedMethod(_request.getTarget(), "GET") == false)
		_statusCodeInt = 405;

	setStatus(_statusCodeInt);

	// handle redirection if necessary
	if (_statusCodeInt == 301)
	{
		_version = "HTTP/1.1";
		_headers["Location"] = filePath;
		setContentLengthHeader(0);
		return ;
	}

	// handle error page if necessary
	if (_statusCodeInt != 200)
	{
		generateErrorPage();
		return ;
	}

	// handle directory listing if necessary
	if(_host.getDirList())
	{
		std::ofstream file2;
		file2.open("www/directoryList.html");
		if (!file2)
		{
			setStatus(500);
			generateErrorPage();
			return ;
		}
		file2 << listDirectory(filePath);
		file2.close();
		filePath = "www/directoryList.html";
	}
	
	removeFirstCharIfMatches(filePath, '/');
	
	//**SEPARATE THIS INTO FILE READING FUNCTION
	std::ifstream file(filePath, std::ios::binary);
	if (!file)
	{
		std::cerr << "Failed to open file: " << filePath << std::endl;
		setStatus(500);
		generateErrorPage();
		return ;
	}
	_body = std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
	file.close();
	// if (!file)
	// {
	// 	// 500 internal server error
	// 	std::cerr << "Failed to read file." << std::endl;
	// 	return ;
	// }

	if (_statusCodeInt != 200)
	{
		generateErrorPage();
		return ;
	}


	// something wrong with version, shows up as "  TP/1.1"
	_version = "HTTP/1.1";
	setContentLengthHeader(_body.size());
	_headers["Content-Type"] = detectContentType(filePath);
}

//------------------------------------------------------------------------------

void Response::handlePostMethod()
{
	std::cout << "Method: " << color("POST", GREEN) << std::endl;

	// Update resource path
	std::cout << "Requested path: " << color(_request.getTarget(), YELLOW) << std::endl;
	std::string filePath = _host.updateResourcePath(_request.getTarget(), _statusCodeInt);
	std::cout << "Resource updated: " << color(filePath, GREEN) << std::endl;

	// Handle body

	// Build response
	setStatus(200);
	_version = "HTTP/1.1";
	setContentLengthHeader(_body.size());
	_headers["Content-Type"] = "text/html";
}

//------------------------------------------------------------------------------

void Response::handleDeleteMethod()
{
	//**TODO
}
