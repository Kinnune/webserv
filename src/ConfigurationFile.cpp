
#include "ConfigurationFile.hpp"
#include "Colors.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
#define METHODS 0
#define CGI 1
#define ERROR_PAGES 2
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
	for (std::vector<struct hostConfig>::iterator it = _hosts.begin(); it != _hosts.end(); it++)
	{
		std::cout << CYAN << "----------------------------------------" << RESET << std::endl;
		std::cout << "ID:\t\t" << color(it->id, GREEN) << std::endl;
		std::cout << "NAME:\t\t" << color(it->serverName, GREEN) << std::endl;
		std::cout << "HOST:\t\t" << color(it->host, GREEN) << std::endl;
		std::cout << "PORT-str:\t" << color(it->portString, GREEN) << std::endl;
		std::cout << "PORT-int:\t" << color(it->portInt, GREEN) << std::endl;
		std::cout << "ROOT:\t\t" << color(it->root, GREEN) << std::endl;
		std::cout << "METHODS:\t"; printMultipleValues(it->methods);
		std::cout << "INDEX:\t\t" << color(it->index, GREEN) << std::endl;
		std::cout << "AUTOINDEX:\t" << color(it->autoindex, GREEN) << std::endl;
		std::cout << "ERRPAG:\t"; printMultipleValues(it->errorPages);
		std::cout << "LOC-n:\t\t" << color(it->locations.size(), GREEN) << std::endl;
		for (std::vector<struct locationConfig>::iterator it2 = it->locations.begin(); it2 != it->locations.end(); it2++)
		{
			std::cout << YELLOW << "\t----------------------------------------" << RESET << std::endl;
			std::cout << "\tLOCATION:\t" << color(it2->location, GREEN) << std::endl;
			std::cout << "\tROOT:\t\t" << color(it2->root, GREEN) << std::endl;
			std::cout << "\tINDEX:\t\t" << color(it2->index, GREEN) << std::endl;
			std::cout << "\tALIAS:\t\t" << color(it2->alias, GREEN) << std::endl;
			std::cout << "\tMETHODS:\t"; printMultipleValues(it->methods);
			std::cout << "\tCGI:\t\t"; printMultipleValues(it2->cgiExtensions);
			std::cout << "\tCGI-PATH:\t" << color(it2->cgiPath, GREEN) << std::endl;
			std::cout << "\tMAX_BODY:\t" << color(it2->maxBody, GREEN) << std::endl;
			std::cout << "\tAUTOINDEX:\t" << color(it2->autoIndex, GREEN) << std::endl;
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
ConfigurationFile::~ConfigurationFile() {}


//------------------------------------------------------------------------------
//		GETTERS
//------------------------------------------------------------------------------

std::vector<struct hostConfig> &ConfigurationFile::getHosts() { return (_hosts); }


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

int ConfigurationFile::getMultipleValues(std::vector<std::string>& values, std::string& line, int type)
{
	std::string value;
	size_t pos;

	while (!line.empty())
	{
		if (line.find_first_of(" \t") == std::string::npos)
			break ;
		pos = line.find_first_not_of(" \t");
		line = line.substr(pos);
		pos = line.find_first_of(" \t;");
		if (pos == std::string::npos)
			pos = line.size();
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
		else
			values.push_back(value);
		line = line.substr(pos);
	}
	return (SUCCESS);
}


//------------------------------------------------------------------------------
//		LOCATION
//------------------------------------------------------------------------------

void ConfigurationFile::setDefaultLocationValues(locationConfig& loc)
{
	loc.root = "";
	loc.index = "";
	loc.alias = "";
	loc.methods.clear();
	loc.cgiExtensions.clear();
	loc.cgiPath = "";
	loc.maxBody = -1;
	loc.autoIndex = false;
}

//------------------------------------------------------------------------------

int ConfigurationFile::storeLocationValues(locationConfig& loc, std::string& line)
{
	std::string key;
	std::string value;

	if (!getKey(line, key))
		return (FAILURE);
	if (!getValue(line, value))
		return (FAILURE);

	if (key == "ROOT")
		loc.root = value;
	else if (key == "INDEX")
		loc.index = value;
	else if (key == "ALIAS")
		loc.alias = value;
	else if (key == "METHODS")
	{
		if (!getMultipleValues(loc.methods, value, METHODS))
			return (FAILURE);
	}
	else if (key == "CGI_EXT")
	{
		if (!getMultipleValues(loc.cgiExtensions, value, CGI))
			return (FAILURE);
	}
	else if (key == "CGI_PATH")
		loc.cgiPath = value;
	else if (key == "AUTOINDEX")
	{
		if (value == "on")
			loc.autoIndex = true;
		else if (value == "off")
			loc.autoIndex = false;
		else
			return (err("Invalid value for autoindex: " + value));
	}
	else if (key == "MAX_BODY")
		loc.maxBody = std::stoi(value);
	else if (key == "RETURN")
	{
		// Don't know if we need this
	}
	else if (key.at(0) == '}')
		return (SUCCESS);
	else
		return (err("Unknown key: " + key));
	return (SUCCESS);
}

//------------------------------------------------------------------------------

int ConfigurationFile::setLocation(locationConfig& loc, std::string& line)
{
	size_t pos = line.find_first_of(" \t");
	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos + 1);
	pos = value.find_first_of(" \t");
	if (pos != std::string::npos)
		value = value.substr(0, pos);
	if (key == "location")
		loc.location = value;
	else
		return (err("Unknown key: " + key));
	return (SUCCESS);
}

//------------------------------------------------------------------------------

int ConfigurationFile::getLocations(hostConfig& host, std::ifstream& file, std::string& line)
{
	while (!file.eof())
	{
		if (line.find("location") != std::string::npos)
		{
			locationConfig loc;

			if (!setLocation(loc, line))
				return (FAILURE);
			
			// Here we set default values for the location struct. Empty strings, -1, etc...
			setDefaultLocationValues(loc);

			nextInfo(file, line);	// error check for starting curly brace?
			nextInfo(file, line);
			while (line.at(0) != '}')
			{
				if (!storeLocationValues(loc, line))
					return (FAILURE);
				nextInfo(file, line);
			}
			host.locations.push_back(loc);		
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

void ConfigurationFile::setDefaultHostValues(hostConfig& host)
{
	host.serverName = "";
	host.host = "";
	host.portString = "";
	host.portInt = -1;
	host.methods.clear();
	host.root = "";
	host.index = "";
	host.autoindex = false;
	host.errorPages.clear();
	host.locations.clear();
}

//------------------------------------------------------------------------------

int ConfigurationFile::storeHostDefaultValue(hostConfig& host, std::string& line)
{
	std::string key;
	std::string value;

	if (!getKey(line, key))
		return (FAILURE);
	if (!getValue(line, value))
		return (FAILURE);
	if (key == "NAME")
	{
		host.serverName = value;
	}
	else if (key == "PORT")
	{
		host.portString = value;
		host.portInt = std::stoi(value);
	}
	else if (key == "HOST")
		host.host = value;
	else if (key == "METHODS")
	{
		if (!getMultipleValues(host.methods, value, METHODS))
			return (FAILURE);
	}
	else if (key == "ROOT")
		host.root = value;
	else if (key == "INDEX")
		host.index = value;
	else if (key == "AUTOINDEX")
	{
		if (value == "on")
			host.autoindex = true;
		else if (value == "off")
			host.autoindex = false;
		else
			std::cerr << RED << "Invalid value for autoindex: " << value << RESET << std::endl;
	}
	else if (key == "ERROR_PAGES")
	{
		if (!getMultipleValues(host.errorPages, value, ERROR_PAGES))
			return (FAILURE);
	}
	else
		return (err("Unknown key: " + key));
	return (SUCCESS);
}

//------------------------------------------------------------------------------

int ConfigurationFile::getHostDefaultValues(hostConfig& host, std::ifstream& file, std::string& line)
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

int ConfigurationFile::getHostConfig(std::ifstream& file, std::string& line)
{
	hostConfig host;
	host.id = _serverCount;

	// Here we set default values for the host struct. Empty strings, -1, etc...
	setDefaultHostValues(host);

	if (!getHostDefaultValues(host, file, line))
		return (FAILURE);

	// Check if default values were set!	example: if (server.serverName.empty())

	// Store host in _hosts
	_hosts.push_back(host);

	getLocations(_hosts.back(), file, line);

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
			if (_eof || !getHostConfig(file, line))
				return (file.close(), err("Error in server config"));
		}
		else
			return (file.close(), err("Syntax error: Token found outside server config."));
	}
	file.close();

	printConfigInfo();
	
	return (SUCCESS);
}
