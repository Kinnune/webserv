
#include "ConfigurationFile.hpp"
#include "Colors.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
#define METHODS 0
#define CGI 1
#define ERROR_PAGES 2
#define INDEX 3
#define FAILURE 0
#define SUCCESS 1


//------------------------------------------------------------------------------
//		DEBUG
//------------------------------------------------------------------------------

void ConfigurationFile::printMultipleValues(std::vector<std::string> &values)
{
	for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); it++)
		std::cout << color(*it, GREEN) << " ";
	std::cout << std::endl;
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
		std::cout << "METHODS:\t"; printMultipleValues(host->getMethods());
		std::cout << "INDEX:\t\t"; printMultipleValues(host->getIndexPages());
		std::cout << "AUTOINDEX:\t" << color((int)host->getAutoIndex(), GREEN) << std::endl;
		std::cout << "ERRPAG:\t\t"; printMultipleValues(host->getErrorPages());
		std::cout << "LOC-n:\t\t" << color(host->getLocations().size(), GREEN) << std::endl;
		for (std::vector<Location>::iterator loc = host->getLocations().begin(); loc != host->getLocations().end(); loc++)
		{
			std::cout << YELLOW << "\t----------------------------------------" << RESET << std::endl;
			std::cout << "\tLOCATION:\t" << color(loc->getLocation, GREEN) << std::endl;
			std::cout << "\tROOT:\t\t" << color(loc->getRoot(), GREEN) << std::endl;
			std::cout << "\tINDEX:\t\t"; printMultipleValues(loc->getIndexPages());
			std::cout << "\tALIAS:\t\t" << color(loc->getAlias(), GREEN) << std::endl;
			std::cout << "\tMETHODS:\t"; printMultipleValues(loc->getMethods());
			std::cout << "\tCGI:\t\t"; printMultipleValues(loc->getCgiExtensions());
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
ConfigurationFile::ConfigurationFile(const ConfigurationFile &other) { *this = other; }
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
			if (value.find_first_not_of("0123456789") == std::string::npos)
				values.push_back(value);
			else
				return (err("Invalid error page: " + value));
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
//		LOCATION
//------------------------------------------------------------------------------

// void ConfigurationFile::setDefaultLocationValues(Location& loc)
// {
// 	loc.root = "";
// 	loc.index_pages.clear();
// 	loc.alias = "";
// 	loc.methods.clear();
// 	loc.cgiExtensions.clear();
// 	loc.cgiPath = "";
// 	loc.maxBody = -1;
// 	loc.autoIndex = autoIndexState::NONE;
// }

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
		if (!parseMultipleValues(loc.getIndexPages(), value, INDEX))
			return (FAILURE);
	}
	else if (key == "ALIAS")
		loc.setAlias(value);
	else if (key == "METHODS")
	{
		if (!parseMultipleValues(loc.getMethods(), value, METHODS))
			return (FAILURE);
	}
	else if (key == "CGI_EXT")
	{
		if (!parseMultipleValues(loc.getCgiExtensions(), value, CGI))
			return (FAILURE);
	}
	else if (key == "CGI_PATH")
		loc.setCgiPath(value);
	else if (key == "INTERPRETER")
		loc.setInterpreter(value);
	else if (key == "AUTOINDEX")
	{
		if (value == "on")
			loc.setAutoIndex(autoIndexState::ON);
		else if (value == "off")
			loc.setAutoIndex(autoIndexState::OFF);
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
			
			// Here we set default values for the location struct. Empty strings, -1, etc...
			// setDefaultLocationValues(loc);

			nextInfo(file, line);	// error check for starting curly brace?
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

// void ConfigurationFile::setDefaultHostValues(hostConfig& host)
// {
// 	host.serverName = "";
// 	host.host = "";
// 	host.portString = "";
// 	host.portInt = -1;
// 	host.methods.clear();
// 	host.root = "";
// 	host.index_pages.clear();
// 	host.autoIndex = autoIndexState::NONE;
// 	host.errorPages.clear();
// 	host.locations.clear();
// }

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
		if (!parseMultipleValues(host.getMethods(), value, METHODS))
			return (FAILURE);
	}
	else if (key == "ROOT")
		host.setRoot(value);
	else if (key == "INDEX")
	{
		// host.index = value;
		if (!parseMultipleValues(host.getIndexPages(), value, INDEX))
			return (FAILURE);
	}
	else if (key == "AUTOINDEX")
	{
		if (value == "on")
			host.setAutoIndex(autoIndexState::ON);
		else if (value == "off")
			host.setAutoIndex(autoIndexState::OFF);
		else
			std::cerr << RED << "Invalid value for autoindex: " << value << RESET << std::endl;
	}
	else if (key == "ERROR_PAGES")
	{
		if (!parseMultipleValues(host.getErrorPages(), value, ERROR_PAGES))
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
			std::cout << RED << "No locations! Is this an error?" << RESET << std::endl;
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

	// Store host in _hosts
	_hosts.push_back(host);

	parseLocations(_hosts.back(), file, line);

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
