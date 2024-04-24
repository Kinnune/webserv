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
	_statusCode = 0;
	_serverName = "";
	_host = "";
	_portString = "";
	_root = "";
	_autoIndex = autoIndexState::NONE;
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
	_serverName = other._serverName;
	_host = other._host;
	_portString = other._portString;
	_root = other._root;
	_autoIndex = other._autoIndex;
	_indexPages = other._indexPages;
	_methods = other._methods;
	_errorPages = other._errorPages;
	_locations = other._locations;
	return *this;
}


//------------------------------------------------------------------------------
//	GETTERS
//------------------------------------------------------------------------------

int Host::getId() const { return _id; }
int Host::getPortInt() const { return _portInt; }
std::string Host::getServerName() const { return _serverName; }
std::string Host::getHost() const { return _host; }
std::string Host::getPortString() const { return _portString; }
std::string Host::getRoot() const { return _root; }
autoIndexState Host::getAutoIndex() const { return _autoIndex; }
std::vector<std::string> &Host::getMethods() { return _methods; }
std::vector<std::string> &Host::getIndexPages() { return _indexPages; }
std::vector<std::string> &Host::getErrorPages() { return _errorPages; }
std::vector<Location> &Host::getLocations() { return _locations; }


//------------------------------------------------------------------------------
//	SETTERS
//------------------------------------------------------------------------------

void Host::setId(int id) { _id = id; }
void Host::setPortInt(int portInt) { _portInt = portInt; }
void Host::setServerName(std::string serverName) { _serverName = serverName; }
void Host::setHost(std::string host) { _host = host; }
void Host::setPortString(std::string portString) { _portString = portString; }
void Host::setRoot(std::string root) { _root = root; }
void Host::setAutoIndex(autoIndexState autoIndex) { _autoIndex = autoIndex; }
void Host::setMethods(std::vector<std::string> methods) { _methods = methods; }
void Host::setIndexPages(std::vector<std::string> index_pages) { _indexPages = index_pages; }
void Host::setErrorPages(std::vector<std::string> errorPages) { _errorPages = errorPages; }
void Host::setLocations(std::vector<Location> locations) { _locations = locations; }


//------------------------------------------------------------------------------
//	ADD
//------------------------------------------------------------------------------

void Host::addLocation(Location &location) { _locations.push_back(location); }


//------------------------------------------------------------------------------
//	METHODS
//------------------------------------------------------------------------------

bool Host::isAllowedCGI(std::string &extension)
{
	for (std::vector<Location>::iterator loc = _locations.begin(); loc != _locations.end(); loc++)
	{
		std::vector<std::string> extensions = loc->getCgiExtensions();
		if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
			return true;
	}
	return false;
}


//------------------------------------------------------------------------------
//	BOOL FUNCTIONS
//------------------------------------------------------------------------------

bool Host::isFile(const std::string &path)
{
	std::cout << "Checking if path is a file" << std::endl;
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

bool Host::locationExists(const std::string &location)
{
	if (std::strncmp(_resourcePath.c_str(), location.c_str(), location.length()) == 0)
	{
		if (_resourcePath.length() == location.length() || _resourcePath[location.length()] == '/')
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------

bool Host::allowedMethod(std::vector<std::string> methods, std::string method)
{
	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
	{
		if (*it == method)
		{
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------------
//	UPDATE RESOURCE PATH
//------------------------------------------------------------------------------

std::string Host::updatePath(std::string const &path)
{
	_resourcePath = path;
	updateResourcePath();
	return _resourcePath;
}

void Host::updateResourcePath()
{
	std::cout << "Updating resource path" << std::endl;
	std::cout << "Path: " << color(_resourcePath, CYAN) << std::endl;
	std::cout << "Port: " << color(_portInt, CYAN) << std::endl;
	std::cout << "Host: " << color(_host, CYAN) << std::endl;

	updateAutoIndex(_autoIndex);
	for (std::vector<Location>::iterator loc = _locations.begin(); loc != _locations.end(); loc++)
	{
		std::cout << "Looking for location: " << color(loc->getLocation(), CYAN) << std::endl;
		if (locationExists(loc->getLocation()))
		{
			std::cout << "Location found: " << color(loc->getLocation(), CYAN) << std::endl;
			updateAutoIndex(loc->getAutoIndex());
			handleLocation(*loc);
			return ;
		}
	}
	std::cout << color("No location found", RED) << std::endl;
	handleNoLocation();
	return ;

	std::cout << color("No host found", RED) << std::endl;
	// If we get this far, we have no host with the port we are looking for. This should probably be handled elsewhere.
}

//------------------------------------------------------------------------------

void Host::handleLocation(Location &loc)
{
	// handle redirection
	if (loc.getRedirection() != "")
	{
		std::cout << "REDIRECTION: " << loc.getRedirection() << std::endl;
		_statusCode = 301;
		_resourcePath = loc.getRedirection();
		return ;
	}

	// handle root/alias
	if (loc.getRoot() != "")
	{
		std::cout << "LOC-ROOT: " << color(loc.getRoot(), GREEN) << std::endl;
		if (isFile(loc.getRoot() + _resourcePath))
		{
			std::cout << "Resource path is a file" << std::endl;
			_resourcePath = loc.getRoot() + _resourcePath;
			return ;
		}
		std::cout << "Resource path is a directory" << std::endl;
		_resourcePath = loc.getRoot() + _resourcePath.substr(loc.getLocation().length());
	}
	else if (loc.getAlias() != "")
	{
		std::cout << "LOC-ALIAS: " << color(loc.getAlias(), GREEN) << std::endl;
		_resourcePath = loc.getAlias();
	}
	else if (_root != "")
	{
		std::cout << "HOST-ROOT: " << color(_root, GREEN) << std::endl;
		_resourcePath = _root + _resourcePath.substr(loc.getLocation().length());
	}

	// handle index and autoindex
	if (isDirectory(_resourcePath))
	{
		_resourcePath.append("/");
		if (loc.getIndexPages().size() > 0)
		{
			for (std::vector<std::string>::iterator it = loc.getIndexPages().begin(); it != loc.getIndexPages().end(); it++)
			{
				std::string indexPage = _resourcePath;
				indexPage.append(*it);
				if (isFile(indexPage))
				{
					_resourcePath.append(*it);
					return ;
				}
			}
			_statusCode = 403;
		}
		else if (_indexPages.size() > 0)
		{
			for (std::vector<std::string>::iterator it = _indexPages.begin(); it != _indexPages.end(); it++)
			{
				std::string indexPage = _resourcePath;
				indexPage.append(*it);
				if (isFile(indexPage))
				{
					_resourcePath.append(*it);
					return ;
				}
			}
			_statusCode = 403;
		}
		else if (_autoIndex != autoIndexState::ON)
		{
			std::cout << "No index directive found. Looking for index file" << std::endl;
			lookForIndexFile();
		}
		else if (_autoIndex == autoIndexState::ON)
		{
			std::cout << "AUTOINDEX: " << color("true", GREEN) << std::endl;
			_autoIndex = autoIndexState::ON;
		}
	}
}

//------------------------------------------------------------------------------

void Host::handleNoLocation()
{
	if (_root != "")
	{
		std::cout << "HOST-ROOT: " << color(_root, GREEN) << std::endl;
		_resourcePath = _root + _resourcePath;
	}
	if (isDirectory(_resourcePath))
	{
		if (_indexPages.size() > 0)
		{
			for (std::vector<std::string>::iterator it = _indexPages.begin(); it != _indexPages.end(); it++)
			{
				std::string indexPage = _resourcePath;
				indexPage.append(*it);
				if (isFile(indexPage))
				{
					_resourcePath.append(*it);
					return ;
				}
			}
			_statusCode = 403;
		}
		else if (_autoIndex != autoIndexState::ON)
		{
			std::cout << "No index directive found. Looking for index file" << std::endl;
			lookForIndexFile();
		}
		else if (_autoIndex == autoIndexState::ON)
		{
			std::cout << "AUTOINDEX: " << color("true", GREEN) << std::endl;
			_autoIndex = autoIndexState::ON;
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

void Host::lookForIndexFile()
{
	std::string path = _resourcePath;
	std::string indexFiles[] = {"index.html", "index.htm", "index.php"};
	for (int i = 0; i < 3; i++)
	{
		if (isFile(path + indexFiles[i]))
		{
			_resourcePath = path + indexFiles[i];
			return ;
		}
	}
	_statusCode = 403;
}

