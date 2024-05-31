
#include "Host.hpp"
#include "Colors.hpp"
#include <iostream>
#include <unistd.h>		// access()
#include <sys/stat.h>	// stat()

//------------------------------------------------------------------------------
//	CONSTRUCTORS / DESTRUCTORS
//------------------------------------------------------------------------------

Host::Host()
{
	_id = 0;
	_portInt = 0;
	_maxBody = -1;
	_statusCode = 0;
	_serverName = "";
	_host = "";
	_portString = "";
	_root = "";
	_dirList = false;
	_autoIndex = autoIndexState::OFF;
}

Host::~Host()
{
}

Host::Host(Host const &other)
{
	*this = other;
}

Host &Host::operator=(Host const &other)
{
	_id = other._id;
	_portInt = other._portInt;
	_maxBody = other._maxBody;
	_serverName = other._serverName;
	_host = other._host;
	_portString = other._portString;
	_root = other._root;
	_autoIndex = other._autoIndex;
	_indexPages = other._indexPages;
	_methods = other._methods;
	_errorPages = other._errorPages;
	_locations = other._locations;
	_dirList = other._dirList;
	return *this;
}

//------------------------------------------------------------------------------
//	GETTERS
//------------------------------------------------------------------------------

int Host::getId() const { return _id; }
int Host::getPortInt() const { return _portInt; }
int Host::getMaxBody() const { return _maxBody; }
bool Host::getDirList() { return _dirList; }
std::string Host::getServerName() const { return _serverName; }
std::string Host::getHost() const { return _host; }
std::string Host::getPortString() const { return _portString; }
std::string Host::getRoot() const { return _root; }
autoIndexState Host::getAutoIndex() const { return _autoIndex; }
std::vector<std::string> &Host::getMethods() { return _methods; }
std::vector<std::string> &Host::getIndexPages() { return _indexPages; }
std::map<std::string, std::string> &Host::getErrorPages() { return _errorPages; }
std::vector<Location> &Host::getLocations() { return _locations; }

std::string Host::getInterpreter(std::string &path, const std::string &extension)
{
	for (std::vector<Location>::iterator loc = _locations.begin(); loc != _locations.end(); loc++)
	{
		if (locationExists(path, loc->getLocation()))
		{
			std::vector<std::string> extensions = loc->getCgiExtensions();
			if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
			{
				return loc->getInterpreter();
			}
			else
			{
				return "";
			}
		}
	}
	return "";
}

//------------------------------------------------------------------------------
//	SETTERS
//------------------------------------------------------------------------------

void Host::setId(int id) { _id = id; }
void Host::setPortInt(int portInt) { _portInt = portInt; }
void Host::setMaxBody(int maxBody) { _maxBody = maxBody; }
void Host::setServerName(std::string serverName) { _serverName = serverName; }
void Host::setHost(std::string host) { _host = host; }
void Host::setPortString(std::string portString) { _portString = portString; }
void Host::setRoot(std::string root) { _root = root; }
void Host::setAutoIndex(autoIndexState autoIndex) { _autoIndex = autoIndex; }
void Host::setMethods(std::vector<std::string> methods) { _methods = methods; }
void Host::setIndexPages(std::vector<std::string> index_pages) { _indexPages = index_pages; }
void Host::setErrorPages(std::map<std::string, std::string> errorPages) { _errorPages = errorPages; }
void Host::setLocations(std::vector<Location> locations) { _locations = locations; }

//------------------------------------------------------------------------------
//	ADD
//------------------------------------------------------------------------------

void Host::addLocation(Location &location) { _locations.push_back(location); }
void Host::addErrorPage(std::string &errorPage, std::string &path) { _errorPages.insert(std::pair<std::string, std::string>(errorPage, path)); }

//------------------------------------------------------------------------------
//	BOOL FUNCTIONS
//------------------------------------------------------------------------------

bool Host::isFile(const std::string &path)
{
	struct stat path_stat;						// create a stat struct
	if (stat(path.c_str(), &path_stat) != 0)	// get the stats of the path, and store them in the struct
		return false;							// if the path does not exist, return false
	return S_ISREG(path_stat.st_mode);			// check if the path is a regular file
}

//------------------------------------------------------------------------------

bool Host::isDirectory(const std::string &path)
{
	struct stat path_stat;
	stat(path.c_str(), &path_stat);
	return S_ISDIR(path_stat.st_mode);
}

//------------------------------------------------------------------------------

bool Host::locationExists(std::string const &path, std::string const &location)
{
	if (std::strncmp(path.c_str(), location.c_str(), location.length()) == 0)
	{
		if (path.length() == location.length() || path[location.length()] == '/')
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------

bool Host::isAllowedMethod(std::string &path, std::string method)
{
	for (std::vector<Location>::iterator loc = _locations.begin(); loc != _locations.end(); loc++)
	{
		if (locationExists(path, loc->getLocation()))
		{
			std::vector<std::string> methods = loc->getMethods();
			return std::find(methods.begin(), methods.end(), method) != methods.end();
		}
	}
	return std::find(_methods.begin(), _methods.end(), method) != _methods.end();
}

//------------------------------------------------------------------------------

bool Host::isAllowedCGI(std::string &path, std::string &extension)
{
	for (std::vector<Location>::iterator loc = _locations.begin(); loc != _locations.end(); loc++)
	{
		if (locationExists(path, loc->getLocation()))
		{
			std::vector<std::string> extensions = loc->getCgiExtensions();
			return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
		}
	}
	return false;
}

//------------------------------------------------------------------------------

bool Host::isRedirection(std::string &path)
{
	for (std::vector<Location>::iterator loc = _locations.begin(); loc != _locations.end(); loc++)
	{
		if (locationExists(path, loc->getLocation()))
		{
			return loc->getRedirection() != "";
		}
	}
	return false;
}

//------------------------------------------------------------------------------

bool Host::isAutoindexOn()
{
	return _autoIndex == autoIndexState::ON;
}

//------------------------------------------------------------------------------

bool stringEndsWithString(std::string const &fullString, std::string const &ending)
{
	if (fullString.length() >= ending.length())
	{
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------
//	UPDATE RESOURCE PATH
//------------------------------------------------------------------------------

std::string Host::updateResourcePath(std::string path, int &statusCode)
{
	for (std::vector<Location>::iterator loc = _locations.begin(); loc != _locations.end(); loc++)
	{
		if (locationExists(path, loc->getLocation()))
		{
			updateAutoIndex(loc->getAutoIndex());
			handleLocation(*loc, path, statusCode);
			return path;
		}
	}
	handleNoLocation(path, statusCode);
	return path;
}

//------------------------------------------------------------------------------

void Host::handleLocation(Location &loc, std::string &path, int &statusCode)
{
	// handle redirection
	if (loc.getRedirection() != "")
	{
		statusCode = 301;
		path = loc.getRedirection();
		return ;
	}
	
	// handle root/alias
	if (loc.getRoot() != "")
	{
		if (isFile(loc.getRoot() + path))
		{
			path = loc.getRoot() + path;
			return ;
		}
		path = loc.getRoot() + path.substr(loc.getLocation().length());
	}
	else if (loc.getAlias() != "")
	{
		path = loc.getAlias();
	}
	else if (_root != "")
	{
		path = _root + path.substr(loc.getLocation().length());
	}

	// handle index and autoindex
	if (isDirectory(path))
	{
		path.append("/");
		if (loc.getIndexPages().size() > 0)	// if index directive is found
		{
			// append location index to path if it exists
			std::vector<std::string> indexPages = loc.getIndexPages();
			for (std::vector<std::string>::iterator it = indexPages.begin(); it != indexPages.end(); it++)
			{
				std::string indexPage = path;
				indexPage.append(*it);
				if (isFile(indexPage))
				{
					path.append(*it);
					break ;
				}
			}
			if (isFile(path))
			{
				statusCode = 200;
			}
			else
			{
				if (loc.getAutoIndex() == autoIndexState::ON || _autoIndex == autoIndexState::ON)
				{
					statusCode = 200;
					_dirList = true;
				}
				else
				{
					statusCode = 403;
				}
			}
		}
		else if (_indexPages.size() > 0) // if root index directive is found
		{
			// append root index to path if it exists
			for (std::vector<std::string>::iterator it = _indexPages.begin(); it != _indexPages.end(); it++)
			{
				std::string indexPage = path;
				indexPage.append(*it);
				if (isFile(indexPage))
				{
					path.append(*it);
					break ;
				}
			}

			if (isFile(path))
			{
				statusCode = 200;
			}
			else
			{
				if (loc.getAutoIndex() == autoIndexState::ON || _autoIndex == autoIndexState::ON)
				{
					statusCode = 200;
					_dirList = true;
				}
				else
				{
					statusCode = 403;
				}
			}
		}
		else
		{
			if (loc.getAutoIndex() == autoIndexState::ON || _autoIndex == autoIndexState::ON)
			{
				statusCode = 200;
				_dirList = true;
			}
			else
			{
				statusCode = 404;
			}
		}
	}
	else if (isFile(path))
	{
		statusCode = 200;
	}
	else
	{
		if (loc.getAutoIndex() == autoIndexState::ON || _autoIndex == autoIndexState::ON)
		{
			if (stringEndsWithString(path, "index.html") || stringEndsWithString(path, "index.htm") || stringEndsWithString(path, "index.php"))
			{
				statusCode = 200;
				_dirList = true;
			}
			else
			{
				statusCode = 404;
			}
		}
		else
		{
			statusCode = 404;
		}

	}
}

//------------------------------------------------------------------------------

void Host::handleNoLocation(std::string &path, int &statusCode)
{
	if (_root != "")
	{
		// std::cout << "HOST-ROOT: " << color(_root, GREEN) << std::endl;
		path = _root + path;
	}

	// handle index and autoindex
	if (isDirectory(path))
	{
		path.append("/");
		if (_indexPages.size() > 0) // if root index directive is found
		{
			// append root index to path if it exists
			for (std::vector<std::string>::iterator it = _indexPages.begin(); it != _indexPages.end(); it++)
			{
				std::string indexPage = path;
				indexPage.append(*it);
				if (isFile(indexPage))
				{
					path.append(*it);
					break ;
				}
			}
			if (isFile(path))
			{
				statusCode = 200;
			}
			else
			{
				if (_autoIndex == autoIndexState::ON)
				{
					statusCode = 200;
					_dirList = true;
				}
				else
				{
					statusCode = 403;
				}
			}
		}
		else
		{
			if (_autoIndex == autoIndexState::ON)
			{
				statusCode = 200;
				_dirList = true;
			}
			else
			{
				statusCode = 404;
			}
		}
	}
	else if (isFile(path))
	{
		statusCode = 200;
	}
	else
	{
		if (_autoIndex == autoIndexState::ON)
		{
			if (stringEndsWithString(path, "index.html") || stringEndsWithString(path, "index.htm") || stringEndsWithString(path, "index.php"))
			{
				statusCode = 200;
				_dirList = true;
			}
			else
			{
				statusCode = 404;
			}
		}
		else
		{
			statusCode = 404;
		}

	}
}

//------------------------------------------------------------------------------

void Host::updateAutoIndex(autoIndexState state)
{
	if (state != autoIndexState::NONE)
	{
		_autoIndex = state;
	}
}

//------------------------------------------------------------------------------

void Host::lookForIndexFile(std::string &path)
{
	std::string indexFiles[] = {"index.html", "index.htm", "index.php"};
	for (int i = 0; i < 3; i++)
	{
		if (isFile(path + indexFiles[i]))
		{
			path = path + indexFiles[i];
			return ;
		}
	}
	_statusCode = 403;
}
