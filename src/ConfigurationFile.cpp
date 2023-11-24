
#include "ConfigurationFile.hpp"
#include "Colors.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>

ConfigurationFile::ConfigurationFile() {}

ConfigurationFile::ConfigurationFile(std::string path) : _path(path), _serverCount(0) {}

ConfigurationFile::~ConfigurationFile() {}

//------------------------------------------------------------------------------

bool isComment(std::string& line)
{
	if (line.empty() || line.at(0) == '\n')
		return (true);
	size_t startPos = line.find_first_not_of(" \t");
	if (startPos == std::string::npos)
		return (true);
	if (line.at(startPos) == '#')
		return (true);
	line = line.substr(startPos);
	return (false);
}

//------------------------------------------------------------------------------

void getMultipleValues(std::vector<std::string>& values, std::string& line)
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
		values.push_back(value);
		line = line.substr(pos);
	}
	std::cout << PURPLE << "End of multi-line" << RESET << std::endl;
}

//---- HOST --------------------------------------------------------------------

void storeHostDefaultValue(hostConfig& host, std::string& line)
{
	std::cout << "Host line: " << line << std::endl;
	size_t pos = line.find_first_of(" \t");
	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos + 1);
	if (key == "NAME")
		host.serverName = value;
	else if (key == "PORT")
	{
		host.portString = value;
		host.portInt = std::stoi(value);
	}
	else if (key == "HOST")
		host.host = value;
	else if (key == "METHODS")
		getMultipleValues(host.methods, value);
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
		getMultipleValues(host.errorPages, value);
	else
		std::cerr << RED << "Unknown key: " << key << RESET << std::endl;
}

void getHostDefaultValues(hostConfig& host, std::ifstream& file, std::string& line)
{
	std::cout << "line = \"" << line << "\"" << std::endl;

	while (!file.eof() && line.empty() == false && line.find("location") && line.at(0) != '}')
	{
		if (isComment(line))
			continue ;
		storeHostDefaultValue(host, line);
		std::getline(file, line);
		std::cout << "line = \"" << line << "\"" << std::endl;
	}
}

//---- LOCATION ----------------------------------------------------------------

void storeLocationValue(locationConfig& loc, std::string& line)
{
	std::cout << "Location line: " << line << std::endl;
	size_t pos = line.find_first_of(" \t");
	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos + 1);
	if (key == "location")
		loc.location = value;
	else if (key == "ROOT")
		loc.root = value;
	else if (key == "INDEX")
		loc.index = value;
	else if (key == "ALIAS")
		loc.alias = value;
	else if (key == "METHODS")
		getMultipleValues(loc.methods, value);
	else if (key == "CGI_EXT")
		getMultipleValues(loc.cgiExtensions, value);
	else if (key == "CGI_PATH")
		loc.cgiPath = value;
	else if (key == "AUTOINDEX")
	{
		if (value == "on")
			loc.autoIndex = true;
		else if (value == "off")
			loc.autoIndex = false;
		else
			std::cerr << RED << "Invalid value for autoindex: " << value << RESET << std::endl;
	}
	else if (key.at(0) == '}')
		return ;
	else
		std::cerr << RED << "Unknown key: " << key << RESET << std::endl;
}

void getLocation(hostConfig& host, std::ifstream& file, std::string& line)
{
}

//---- PARSE -------------------------------------------------------------------

int ConfigurationFile::parse()
{
	std::ifstream file(_path.c_str());

	if (!file.is_open())
	{
		std::cerr << RED << "Error opening config file" << RESET << std::endl;
		return (1);
	}

	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);

		if (isComment(line) || line.empty())
			continue ;
		if (line.find("server") != std::string::npos)
		{
			std::cout << GREEN << "Server found" << RESET << std::endl;
			
			hostConfig host;
			host.id = _serverCount;

			// Skip comments and empty lines
			std::getline(file, line);
			while (isComment(line))
				std::getline(file, line);

			getHostDefaultValues(host, file, line);

			// Store host in _hosts
			_hosts.insert(std::pair<std::string, struct hostConfig>(host.serverName, host));

			// Check if default values were set
			std::cout << YELLOW << "default values set" << RESET << std::endl;

			while (isComment(line))
				std::getline(file, line);


			while (line.at(0) != '}')
				getLocations(host, file, line);

			// Store all the location configurations in host
			if (line.find("location") != std::string::npos)
			{
				locationConfig loc;
				
				while (!file.eof() && line.at(0) != '}')
				{
					if (isComment(line))
						continue ;
					storeLocationValue(loc, line);
					std::getline(file, line);
				}
				_hosts[host.serverName].locations.insert(std::pair<std::string, struct locationConfig>(loc.location, loc));
				_locationCount++;
			}
			else if (line.at(0) == '}') // End of server
			{
				std::cout << PURPLE << "End of server" << RESET << std::endl;
			}

			std::cout << GREEN << "Server " << _serverCount << RESET << std::endl;
			_serverCount++;
		}
	}

	file.close();

	std::cout << "Server count: " << _serverCount << std::endl;
	std::cout << "Location count: " << _locationCount << std::endl;

	// print host info
	for (std::map<std::string, struct hostConfig>::iterator it = _hosts.begin(); it != _hosts.end(); it++)
	{
		std::cout << "Host: " << it->first << std::endl;
		std::cout << "Host id: " << it->second.id << std::endl;
		std::cout << "Host serverName: " << it->second.serverName << std::endl;
		std::cout << "Host host: " << it->second.host << std::endl;
		std::cout << "Host portString: " << it->second.portString << std::endl;
		std::cout << "Host portInt: " << it->second.portInt << std::endl;
		std::cout << "Host root: " << it->second.root << std::endl;
		std::cout << "Host index: " << it->second.index << std::endl;
		std::cout << "Host autoindex: " << it->second.autoindex << std::endl;
		std::cout << "Host location count: " << it->second.locations.size() << std::endl;
		std::cout << std::endl;
	}



	return (0);
}