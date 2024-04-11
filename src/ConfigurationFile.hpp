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
	std::string redirection;
	int redirectionCode;
	int maxBody;
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
	std::vector<struct locationConfig> locations;
} hostConfig;

class ConfigurationFile
{
	private:
		std::string _path;
		std::string _content;
		int _serverCount;
		std::vector<struct hostConfig> _hosts;
		bool _configSyntaxError;
		bool _eof;
		void printMultipleValues(std::vector<std::string> &values);
		int getHostConfig(std::ifstream &file, std::string &line);
		int getHostDefaultValues(hostConfig& host, std::ifstream& file, std::string& line);
		int getMultipleValues(std::vector<std::string>& values, std::string& line, int type);
		void setDefaultLocationValues(locationConfig& loc);
		int storeLocationValues(locationConfig& loc, std::string& line);
		int setLocation(locationConfig& loc, std::string& line);
		int getLocations(hostConfig& host, std::ifstream& file, std::string& line);
		void setDefaultHostValues(hostConfig& host);
		int storeHostDefaultValue(hostConfig& host, std::string& line);
		int err(std::string error);
		void nextInfo(std::ifstream &file, std::string& line);
		int serverFound(std::ifstream& file, std::string& line);
		int getKey(std::string& line, std::string& key);
		int getValue(std::string& line, std::string& value);
	public:
		ConfigurationFile();
		ConfigurationFile(std::string path);
		ConfigurationFile(const ConfigurationFile &other);
		ConfigurationFile &operator=(const ConfigurationFile &other);
		~ConfigurationFile();
		std::vector<struct hostConfig> &getHosts();
		void printConfigInfo();
		int parse();
};

#endif