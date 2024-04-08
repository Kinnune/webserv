#ifndef CONFIGURATIONFILE_HPP
#define CONFIGURATIONFILE_HPP
#include <string>
#include <map>
#include <vector>

typedef struct locationConfig
{
	std::string location;
	std::string root;
	std::string index;
	std::string alias;
	std::vector<std::string> methods;
	std::vector<std::string> cgiExtensions;
	std::string cgiPath;
	bool autoIndex;
} locationConfig;

typedef struct hostConfig
{
	int			id;
	int 		portInt;
	std::string serverName;
	std::string host;
	std::vector<std::string> methods;
	std::string portString;
	std::string root;
	std::string index;
	bool autoindex;
	std::vector<std::string> errorPages;
	std::map<std::string, struct locationConfig> locations;
} hostConfig;

class ConfigurationFile
{
	private:
		std::string _path;
		std::string _content;
		std::map<std::string, std::string> _config;
		int _serverCount;
		int _locationCount;
		std::map<std::string, struct hostConfig> _hosts;
	public:
		ConfigurationFile();
		ConfigurationFile(std::string path);
		~ConfigurationFile();
		int parse();
};

#endif