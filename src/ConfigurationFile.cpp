
#include "ConfigurationFile.hpp"
#include "Colors.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>

//------------------------------------------------------------------------------
//		DEBUG
//------------------------------------------------------------------------------

void ConfigurationFile::printMultipleValues(std::vector<std::string> &values)
{
	for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); it++)
		std::cout << color(*it, GREEN) << " ";
	std::cout << std::endl;
}

void ConfigurationFile::printErrorpages(std::map<std::string, std::string> &errorPages)
{
	for (std::map<std::string, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); it++)
		std::cout << "ERRPAG:\t\t" << color(it->first, GREEN) << " -> " << color(it->second, GREEN) << std::endl;
}

//------------------------------------------------------------------------------

void ConfigurationFile::printConfigInfo()
{
	std::cout << color("----------------------------------------", CYAN) << std::endl;
	std::cout << color("CONFIG INFO", CYAN) << std::endl;
	for (std::vector<Host>::iterator host = _hosts.begin(); host != _hosts.end(); host++)
	{
		std::cout << CYAN << "----------------------------------------" << RESET << std::endl;
		std::cout << "ID:\t\t" << color(host->getId(), GREEN) << std::endl;
		std::cout << "NAME:\t\t" << color(host->getServerName(), GREEN) << std::endl;
		std::cout << "HOST:\t\t" << color(host->getHost(), GREEN) << std::endl;
		std::cout << "PORT-str:\t" << color(host->getPortString(), GREEN) << std::endl;
		std::cout << "PORT-int:\t" << color(host->getPortInt(), GREEN) << std::endl;
		std::cout << "ROOT:\t\t" << color(host->getRoot(), GREEN) << std::endl;
		std::vector<std::string> methods = host->getMethods();
		std::cout << "METHODS:\t"; printMultipleValues(methods);
		std::vector<std::string> indexPages = host->getIndexPages();
		std::cout << "INDEX:\t\t"; printMultipleValues(indexPages);
		std::cout << "AUTOINDEX:\t" << color((int)host->getAutoIndex(), GREEN) << std::endl;
		std::cout << "MAX_BODY:\t" << color(host->getMaxBody(), GREEN) << std::endl;
		std::map<std::string, std::string> errorPages = host->getErrorPages();
		printErrorpages(errorPages);
		std::cout << "LOC-n:\t\t" << color(host->getLocations().size(), GREEN) << std::endl;
		for (std::vector<Location>::iterator loc = host->getLocations().begin(); loc != host->getLocations().end(); loc++)
		{
			std::cout << YELLOW << "\t----------------------------------------" << RESET << std::endl;
			std::cout << "\tLOCATION:\t" << color(loc->getLocation(), GREEN) << std::endl;
			std::cout << "\tROOT:\t\t" << color(loc->getRoot(), GREEN) << std::endl;
			std::vector<std::string> indexPages = loc->getIndexPages();
			std::cout << "\tINDEX:\t\t"; printMultipleValues(indexPages);
			std::cout << "\tALIAS:\t\t" << color(loc->getAlias(), GREEN) << std::endl;
			std::vector<std::string> methods = loc->getMethods();
			std::cout << "\tMETHODS:\t"; printMultipleValues(methods);
			std::vector<std::string> cgiExtensions = loc->getCgiExtensions();
			std::cout << "\tCGI:\t\t"; printMultipleValues(cgiExtensions);
			std::cout << "\tCGI_PATH:\t" << color(loc->getCgiPath(), GREEN) << std::endl;
			std::cout << "\tINTERPRETER:\t" << color(loc->getInterpreter(), GREEN) << std::endl;
			std::cout << "\tAUTOINDEX:\t" << color((int)loc->getAutoIndex(), GREEN) << std::endl;
			std::cout << "\tMAX_BODY:\t" << color(loc->getMaxBody(), GREEN) << std::endl;
			std::cout << "\tRETURN:\t\t" << color(loc->getRedirectionCode(), GREEN) << " " << color(loc->getRedirection(), GREEN) << std::endl;
		}
		std::cout << std::endl;
	}
}

//------------------------------------------------------------------------------
//		CONSTRUCTORS/DESTRUCTORS
//------------------------------------------------------------------------------

ConfigurationFile::ConfigurationFile() {}

ConfigurationFile::ConfigurationFile(std::string path) : _path(path), _serverCount(0) {}

ConfigurationFile::ConfigurationFile(const ConfigurationFile &other)
{
	*this = other;
}

ConfigurationFile &ConfigurationFile::operator=(const ConfigurationFile &other)
{
	if (this != &other)
	{
		_path = other._path;
		_content = other._content;
		_serverCount = other._serverCount;
		_hosts = other._hosts;
		_configSyntaxError = other._configSyntaxError;
		_eof = other._eof;
	}
	return (*this);
}

ConfigurationFile::~ConfigurationFile() {}


//------------------------------------------------------------------------------
//		GETTERS
//------------------------------------------------------------------------------

std::vector<Host> &ConfigurationFile::getHosts() { return (_hosts); }

Host *ConfigurationFile::getHost(std::string hostHeader)
{
	std::string name;
	std::string port;
	size_t pos;
	
	// Derive name and port from hostHeader
	pos = hostHeader.find(":");
	if (pos != std::string::npos)
	{
		name = hostHeader.substr(0, pos);
		port = hostHeader.substr(pos + 1);
	}

	// Find host with correct name and port
	for (std::vector<Host>::iterator host = _hosts.begin(); host != _hosts.end(); host++)
	{
		if (host->getServerName() == name && host->getPortString() == port)
			return (&(*host));
	}

	std::cerr << RED << "Host not found!" << RESET << std::endl;

	// Return nullptr if host not found
	return (nullptr);
}


//------------------------------------------------------------------------------
//		HELPERS
//------------------------------------------------------------------------------

int ConfigurationFile::err(std::string error)
{
	_configSyntaxError = true;
	std::cerr << RED << error << RESET << std::endl;
	return (FAILURE);
}

//------------------------------------------------------------------------------

void ConfigurationFile::nextInfo(std::ifstream &file, std::string& line)
{
	std::getline(file, line);
	if (file.eof())
	{
		_eof = true;
		return ;
	}
	while (line.empty() || line.at(0) == '\n')
		std::getline(file, line);
	size_t startPos = line.find_first_not_of(" \t");
	if (startPos == std::string::npos || line.at(startPos) == '#')
		nextInfo(file, line);
	else
		line = line.substr(startPos);
}

//------------------------------------------------------------------------------

int ConfigurationFile::getKey(std::string& line, std::string& key)
{
	size_t pos;

	pos = line.find_first_of(" \t");
	if (pos == std::string::npos || line.at(pos) == '\n')
		return (err("Syntax error: Expected key"));
	key = line.substr(0, pos);
	line = line.substr(pos + 1);
	return (SUCCESS);
}

//------------------------------------------------------------------------------

int ConfigurationFile::getValue(std::string& line, std::string& value)
{
	size_t pos;
	std::string tmp;

	pos = line.find_first_not_of(" \t");
	if (pos == std::string::npos)
		return (err("Syntax error: Expected value"));
	tmp = line.substr(pos);
	pos = tmp.find_first_of(";");
	if (pos == std::string::npos)
		return (err("Syntax error: Expected whitespace or ';'"));
	value = tmp.substr(0, pos);
	return (SUCCESS);
}

//------------------------------------------------------------------------------

int ConfigurationFile::getLastValue(std::string& line, std::string& value)
{
	size_t pos;

	pos = line.find_last_not_of(" \t");
	if (pos == std::string::npos)
		return (err("Syntax error: Expected value"));
	line = line.substr(0, pos + 1);
	pos = line.find_last_of(" \t");
	if (pos == std::string::npos)
		return (err("Syntax error: Expected value"));
	value = line.substr(pos + 1);
	line = line.substr(0, pos);
	pos = line.find_last_not_of(" \t");
	if (pos == std::string::npos)
		return (err("Syntax error: Expected value"));
	line = line.substr(0, pos + 1);
	return (SUCCESS);
}

//------------------------------------------------------------------------------

int ConfigurationFile::parseMultipleValues(std::vector<std::string>& values, std::string& line, int type)
{
	std::string value;
	size_t pos;

	while (!line.empty())
	{
		if (line.find_first_of(" \t") == std::string::npos)
		{
			if (line.find_first_not_of(" \t") == std::string::npos)
				break ;
			value = line;
			values.push_back(value);
			break ;
		}
		pos = line.find_first_not_of(" \t");
		line = line.substr(pos);
		pos = line.find_first_of(" \t;");
		if (pos == std::string::npos)
		{
			pos = line.size();
			value = line;
		}
		else
			value = line.substr(0, pos);
		if (type == METHODS)
		{
			if (value == "GET" || value == "POST" || value == "DELETE")
				values.push_back(value);
			else
				return (err("Invalid method: " + value));
		}
		else if (type == CGI)
		{
			if (value.at(0) == '.')
				values.push_back(value);
			else
				return (err("Invalid CGI extension: " + value));
		}
		else if (type == INDEX)
		{
			values.push_back(value);
		}
		else if (type == ERROR_PAGES)
		{
			values.push_back(value);
		}
		else
		{
			values.push_back(value);
		}
		line = line.substr(pos);
	}
	return (SUCCESS);
}

//------------------------------------------------------------------------------

int ConfigurationFile::parseErrorPages(Host& host, std::string& line)
{
	std::string value;

	if (!getLastValue(line, value))
		return (FAILURE);

	std::vector<std::string> errorPages;
	if (!parseMultipleValues(errorPages, line, ERROR_PAGES))
		return (FAILURE);
	
	for (std::vector<std::string>::iterator it = errorPages.begin(); it != errorPages.end(); it++)
	{
		host.addErrorPage(*it, value);
	}
	
	return (SUCCESS);
}


//------------------------------------------------------------------------------
//		LOCATION
//------------------------------------------------------------------------------

int ConfigurationFile::storeLocationValues(Location& loc, std::string& line)
{
	std::string key;
	std::string value;

	if (!getKey(line, key))
		return (FAILURE);
	if (!getValue(line, value))
		return (FAILURE);

	if (key == "ROOT")
		loc.setRoot(value);
	else if (key == "INDEX")
	{
		std::vector<std::string> indexPages;
		if (!parseMultipleValues(indexPages, value, INDEX))
			return (FAILURE);
		loc.setIndexPages(indexPages);
	}
	else if (key == "ALIAS")
		loc.setAlias(value);
	else if (key == "METHODS")
	{
		std::vector<std::string> methods;
		if (!parseMultipleValues(methods, value, METHODS))
			return (FAILURE);
		loc.setMethods(methods);

	}
	else if (key == "CGI_EXT")
	{
		std::vector<std::string> cgiExtensions;
		if (!parseMultipleValues(cgiExtensions, value, CGI))
			return (FAILURE);
		loc.setCgiExtensions(cgiExtensions);
	}
	else if (key == "CGI_PATH")
		loc.setCgiPath(value);
	else if (key == "INTERPRETER")
		loc.setInterpreter(value);
	else if (key == "AUTOINDEX")
	{
		if (value == "on")
			loc.setAutoIndex(true);
		else if (value == "off")
			loc.setAutoIndex(false);
		else
			return (err("Invalid value for autoindex: " + value));
	}
	else if (key == "MAX_BODY")
		loc.setMaxBody(std::stoi(value));
	else if (key == "RETURN")
	{
		size_t pos;
		pos = value.find_first_of(" \t");
		loc.setRedirectionCode(stoi(value.substr(0, pos)));
		pos = value.find_first_not_of(" \t", pos);
		loc.setRedirection(value.substr(pos));
	}
	else if (key.at(0) == '}')
		return (SUCCESS);
	else
		return (err("Unknown key: " + key));
	return (SUCCESS);
}

//------------------------------------------------------------------------------

int ConfigurationFile::setLocation(Location& loc, std::string& line)
{
	size_t pos = line.find_first_of(" \t");
	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos + 1);
	pos = value.find_first_of(" \t");
	if (pos != std::string::npos)
		value = value.substr(0, pos);
	if (key == "location")
		loc.setLocation(value);
	else
		return (err("Unknown key: " + key));
	return (SUCCESS);
}

//------------------------------------------------------------------------------

int ConfigurationFile::parseLocations(Host& host, std::ifstream& file, std::string& line)
{
	while (!file.eof())
	{
		if (line.find("location") != std::string::npos)
		{
			Location loc;

			if (!setLocation(loc, line))
				return (FAILURE);

			nextInfo(file, line);
			nextInfo(file, line);
			while (line.at(0) != '}')
			{
				if (!storeLocationValues(loc, line))
					return (FAILURE);
				nextInfo(file, line);
			}
			host.addLocation(loc);

			nextInfo(file, line);
		}
		if (line.at(0) == '}')
		{
			break ;
		}
	}
	return (SUCCESS);
}


//------------------------------------------------------------------------------
//		HOST
//------------------------------------------------------------------------------

int ConfigurationFile::storeHostDefaultValue(Host& host, std::string& line)
{
	std::string key;
	std::string value;

	if (!getKey(line, key))
		return (FAILURE);
	if (!getValue(line, value))
		return (FAILURE);
	if (key == "NAME")
	{
		host.setServerName(value);
	}
	else if (key == "PORT")
	{
		host.setPortString(value);
		host.setPortInt(std::stoi(value));
	}
	else if (key == "HOST")
		host.setHost(value);
	else if (key == "METHODS")
	{
		std::vector<std::string> methods;
		if (!parseMultipleValues(methods, value, METHODS))
			return (FAILURE);
		host.setMethods(methods);
	}
	else if (key == "ROOT")
		host.setRoot(value);
	else if (key == "INDEX")
	{
		std::vector<std::string> indexPages;
		if (!parseMultipleValues(indexPages, value, INDEX))
			return (FAILURE);
		host.setIndexPages(indexPages);
	}
	else if (key == "AUTOINDEX")
	{
		if (value == "on")
			host.setAutoIndex(true);
		else if (value == "off")
			host.setAutoIndex(false);
		else
			return (err("Invalid value for autoindex: " + value));
	}
	else if (key == "MAX_BODY")
		host.setMaxBody(std::stoi(value));
	else if (key == "ERROR_PAGES")
	{
		if (!parseErrorPages(host, value))
			return (FAILURE);
	}
	else
		return (err("Unknown key: " + key));
	return (SUCCESS);
}

//------------------------------------------------------------------------------

int ConfigurationFile::parseHostDefaultValues(Host& host, std::ifstream& file, std::string& line)
{
	while (!file.eof() && !line.empty() && line.at(0) != '}')
	{
		nextInfo(file, line);
		if (line.find("location") != std::string::npos)
		{
			break ;
		}
		if (line.at(0) == '}')
		{
			std::cerr << RED << "No locations! Is this an error?" << RESET << std::endl;
			break ;
		}
		if (!storeHostDefaultValue(host, line))
		{
			std::cerr << RED << "StoreHostDefaultValue failed!" << RESET << std::endl;
			return (FAILURE);
		}
	}
	return (SUCCESS);
}


//------------------------------------------------------------------------------
//		SERVER
//------------------------------------------------------------------------------

int ConfigurationFile::parseHostConfig(std::ifstream& file, std::string& line)
{
	Host host;
	host.setId(_serverCount);

	// Here we set default values for the host struct. Empty strings, -1, etc...
	// setDefaultHostValues(host);

	if (!parseHostDefaultValues(host, file, line))
		return (FAILURE);

	// Check if default values were set!	example: if (server.serverName.empty())


	parseLocations(host, file, line);

	// Store host in _hosts
	_hosts.push_back(host);

	// DEBUG START
	// std::cout << color("Locations parsed!", PURPLE) << std::endl;
	// for (std::vector<Location>::iterator loc = _hosts.back().getLocations().begin(); loc != _hosts.back().getLocations().end(); loc++)
	// {
	// 	std::cout << "Location: " << color(loc->getLocation(), GREEN) << " has " << color(loc->getMethods().size(), GREEN) << " methods." << std::endl;
	// }
	// DEBUG END


	if (line.at(0) == '}') // End of server
	{
		_serverCount++;
		return (SUCCESS);
	}

	return (FAILURE);
}


//------------------------------------------------------------------------------
//		PARSE
//------------------------------------------------------------------------------

int ConfigurationFile::serverFound(std::ifstream& file, std::string& line)
{
	if (line.find("server") != std::string::npos)
	{
		nextInfo(file, line);
		if (!line.empty() && line.at(0) == '{')
			return (SUCCESS);
		else
			return (err("Syntax error: Expected '{' after server"));
	}
	return (FAILURE);
}

//------------------------------------------------------------------------------

int ConfigurationFile::parse()
{
	std::ifstream file(_path.c_str());
	std::string line;

	_configSyntaxError = false;
	_eof = false;

	if (!file.is_open())
		return (err("Could not open file: " + _path));

	while (!file.eof())
	{
		nextInfo(file, line);
		if (_eof && _serverCount == 0)
			return (err("No server config found."));
		else if (_eof)
			break ;
		if (serverFound(file, line))
		{
			if (_eof || !parseHostConfig(file, line))
				return (file.close(), err("Error in server config"));
		}
		else
			return (file.close(), err("Syntax error: Token found outside server config."));
	}
	file.close();

	printConfigInfo();
	
	return (SUCCESS);
}
