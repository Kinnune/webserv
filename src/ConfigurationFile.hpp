#ifndef CONFIGURATIONFILE_HPP
#define CONFIGURATIONFILE_HPP
#include "Host.hpp"
#include <string>
#include <map>
#include <vector>

// enum class autoIndexState { NONE, ON, OFF };

class ConfigurationFile
{
	private:
		std::string _path;
		std::string _content;
		int _serverCount;
		std::vector<Host> _hosts;
		bool _configSyntaxError;
		bool _eof;
		void printMultipleValues(std::vector<std::string> &values);
		int parseHostConfig(std::ifstream &file, std::string &line);
		int parseHostDefaultValues(Host& host, std::ifstream& file, std::string& line);
		int parseMultipleValues(std::vector<std::string>& values, std::string& line, int type);
		int storeLocationValues(Location& loc, std::string& line);
		int setLocation(Location& loc, std::string& line);
		int parseLocations(Host& host, std::ifstream& file, std::string& line);
		int storeHostDefaultValue(Host& host, std::string& line);
		int err(std::string error);
		void nextInfo(std::ifstream &file, std::string& line);
		int serverFound(std::ifstream& file, std::string& line);
		int getKey(std::string& line, std::string& key);
		int getValue(std::string& line, std::string& value);
	public:

		// Constructors/Destructors
		ConfigurationFile();
		ConfigurationFile(std::string path);
		ConfigurationFile(const ConfigurationFile &other);
		ConfigurationFile &operator=(const ConfigurationFile &other);
		~ConfigurationFile();
		
		// Getters
		std::vector<Host> &getHosts();
		Host *getHost(std::string hostHeader);
		
		// Setters

		// Member functions
		void printConfigInfo();
		int parse();
};

#endif