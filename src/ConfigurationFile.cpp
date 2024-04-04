
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
//		CONSTRUCTORS/DESTRUCTORS
//------------------------------------------------------------------------------

ConfigurationFile::ConfigurationFile() {}

ConfigurationFile::ConfigurationFile(std::string path) : _path(path), _serverCount(0) {}

ConfigurationFile::ConfigurationFile(const ConfigurationFile &other)
{
	*this = other;
}

ConfigurationFile::~ConfigurationFile() {}


//------------------------------------------------------------------------------
//		ERROR
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

int ConfigurationFile::getMultipleValues(std::vector<std::string>& values, std::string& line, int type)
{
	std::string value;
	size_t pos;

	while (!line.empty())
	{
		std::cout << GREEN << "multi-line = " << line << RESET << std::endl;
		if (line.find_first_of(" \t") == std::string::npos)
			break ;
		pos = line.find_first_not_of(" \t");
		line = line.substr(pos);
		pos = line.find_first_of(" \t;");
		value = line.substr(0, pos);
		std::cout << "multi-value = " << value << std::endl;
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
	std::cout << PURPLE << "End of multi-line" << RESET << std::endl;
	return (SUCCESS);
}


//------------------------------------------------------------------------------
//		LOCATION
//------------------------------------------------------------------------------

int ConfigurationFile::storeLocationValues(locationConfig& loc, std::string& line)
{
	std::cout << "Location line: " << line << std::endl;
	size_t pos = line.find_first_of(" \t");
	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos + 1);

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

int ConfigurationFile::setLocation(locationConfig& loc, std::string& line)
{
	std::cout << CYAN << "Setting location" << RESET << std::endl;
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
	std::cout << "Location set: " << loc.location << std::endl;
	return (SUCCESS);
}

int ConfigurationFile::getLocations(hostConfig& host, std::ifstream& file, std::string& line)
{
	std::cout << CYAN << "Getting locations" << RESET << std::endl;
	
	while (!file.eof())
	{
		if (line.find("location") != std::string::npos)
		{
			std::cout << PURPLE << "Start of location" << RESET << std::endl;
			if (!setLocation(host.locations[host.serverName], line))
				return (FAILURE);
			nextInfo(file, line);	// error check for starting curly brace?
			nextInfo(file, line);
			while (line.at(0) != '}')
			{
				if (!storeLocationValues(host.locations[host.serverName], line))
					return (FAILURE);
				nextInfo(file, line);
			}
			std::cout << PURPLE << "End of location" << RESET << std::endl;
			_locationCount++;
			std::cout << "Line: " << line << std::endl;			
			nextInfo(file, line);
			std::cout << "Line: " << line << std::endl;			
		}
		if (line.at(0) == '}')
		{
			std::cout << "End of locations" << std::endl;
			break ;
		}
	}
	std::cout << PURPLE << "End of locations" << RESET << std::endl;
	return (SUCCESS);
}


//------------------------------------------------------------------------------
//		HOST
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

int ConfigurationFile::getValue(std::string& line, std::string& value)
{
	size_t pos;
	std::string tmp;

	pos = line.find_first_not_of(" \t");
	if (pos == std::string::npos)
		return (err("Syntax error: Expected value"));
	tmp = line.substr(pos);
	pos = tmp.find_first_of(" \t;");
	if (pos == std::string::npos)
		return (err("Syntax error: Expected whitespace or ';'"));
	value = tmp.substr(0, pos);
	return (SUCCESS);
}

int ConfigurationFile::storeHostDefaultValue(hostConfig& host, std::string& line)
{
	std::string key;
	std::string value;

	std::cout << "Host line: " << line << std::endl;
	if (!getKey(line, key))
		return (FAILURE);
	if (!getValue(line, value))
		return (FAILURE);
	std::cout << "key = " << key << std::endl;
	std::cout << "value = " << value << std::endl;
	if (key == "NAME")
	{
		host.serverName = value;
		std::cout << "Server name set: " << host.serverName << std::endl;
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

int ConfigurationFile::getHostDefaultValues(hostConfig& host, std::ifstream& file, std::string& line)
{
	while (!file.eof() && !line.empty() && line.at(0) != '}')
	{
		std::cout << "GHDV: " << line << std::endl;
		nextInfo(file, line);
		std::cout << "GHDV: " << line << std::endl;
		if (line.find("location") != std::string::npos)
		{
			std::cout << "Location found" << std::endl;
			break ;
		}
		if (line.at(0) == '}')
		{
			std::cout << RED << "No locations! Is this an error?" << RESET << std::endl;
			break ;
		}
		if (!storeHostDefaultValue(host, line))
		{
			std::cout << "StoreHostDefaultValue failed!" << std::endl;
			return (FAILURE);
		}
	}
	std::cout << "End of default values" << std::endl;
	return (SUCCESS);
}


//------------------------------------------------------------------------------
//		SERVER
//------------------------------------------------------------------------------

int ConfigurationFile::getHostConfig(std::ifstream& file, std::string& line)
{
	std::cout << CYAN << "Server found" << RESET << std::endl;
			
	hostConfig host;
	host.id = _serverCount;

	getHostDefaultValues(host, file, line);

	// Check if default values were set!	example: if (server.serverName.empty())

	std::cout << YELLOW << "default values set" << RESET << std::endl;

	// Store host in _hosts
	_hosts.insert(std::pair<std::string, struct hostConfig>(host.serverName, host));

	getLocations(host, file, line);

	if (line.at(0) == '}') // End of server
	{
		_serverCount++;
		std::cout << PURPLE << "End of server: " << _serverCount << RESET << std::endl;
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
		std::cout << CYAN_BOLD << "Line: [" << line << "]" << RESET << std::endl;
		if (serverFound(file, line))
		{
			if (_eof || !getHostConfig(file, line))
				return (file.close(), err("Error in server config"));
		}
		else
			return (file.close(), err("Syntax error: Token found outside server config."));
	}
	file.close();

	std::cout << "Server count: " << _serverCount << std::endl;
	std::cout << "Location count: " << _locationCount << std::endl;
	return (SUCCESS);
}



	// print host info
	// for (std::map<std::string, struct hostConfig>::iterator it = _hosts.begin(); it != _hosts.end(); it++)
	// {
	// 	std::cout << "Host: " << it->first << std::endl;
	// 	std::cout << "Host id: " << it->second.id << std::endl;
	// 	std::cout << "Host serverName: " << it->second.serverName << std::endl;
	// 	std::cout << "Host host: " << it->second.host << std::endl;
	// 	std::cout << "Host portString: " << it->second.portString << std::endl;
	// 	std::cout << "Host portInt: " << it->second.portInt << std::endl;
	// 	std::cout << "Host root: " << it->second.root << std::endl;
	// 	std::cout << "Host index: " << it->second.index << std::endl;
	// 	std::cout << "Host autoindex: " << it->second.autoindex << std::endl;
	// 	std::cout << "Host location count: " << it->second.locations.size() << std::endl;
	// 	std::cout << std::endl;
	// }