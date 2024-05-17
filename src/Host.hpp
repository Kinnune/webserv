#ifndef HOST_HPP
#define HOST_HPP
#include <string>
#include <vector>
#include <map>
#include "Location.hpp"

// enum class autoIndexState { NONE, ON, OFF };

class Host
{
	private:
		int							_id;
		int 						_portInt;
		int							_statusCode;
		bool						_autoIndex;
		bool						_dirList;
		std::string 				_serverName;
		std::string 				_host;
		std::string					_portString;
		std::string					_root;
		std::vector<std::string>	_methods;
		std::vector<std::string>	_indexPages;
		std::map<std::string, std::string>	_errorPages;
		std::vector<Location>		_locations;

		// Private methods
		bool isDirectory(std::string const &path);
		bool locationExists(std::string const &path, std::string const &location);
		void handleLocation(Location &loc, std::string &path, int &statusCode);
		void handleNoLocation(std::string &path, int &statusCode);
		void updateAutoIndex(bool state);
		void lookForIndexFile(std::string &path);

	
	public:
		// Constructors/Destructors
		Host();
		~Host();
		Host(Host const &other);
		Host &operator=(Host const &other);

		// Getters
		int getId() const;
		int getPortInt() const;
		bool getAutoIndex() const;
		bool getDirList(void);
		std::string getServerName() const;
		std::string getHost() const;
		std::string getPortString() const;
		std::string getRoot() const;
		std::string getInterpreter(std::string &path, const std::string &extension);
		std::vector<std::string> &getMethods();
		std::vector<std::string> &getIndexPages();
		std::map<std::string, std::string> &getErrorPages();
		std::vector<Location> &getLocations();

		// Setters
		void setId(int id);
		void setPortInt(int portInt);
		void setServerName(std::string serverName);
		void setHost(std::string host);
		void setPortString(std::string portString);
		void setRoot(std::string root);
		void setAutoIndex(bool autoIndex);
		void setMethods(std::vector<std::string> methods);
		void setIndexPages(std::vector<std::string> index_pages);
		void setErrorPages(std::map<std::string, std::string> errorPages);
		void setLocations(std::vector<Location> locations);

		// Add
		void addLocation(Location &location);
		void addErrorPage(std::string &errorPage, std::string &path);

		// Methods
		bool isFile(std::string const &path);
		bool isAllowedCGI(std::string &path, std::string &extension);
		bool isAllowedMethod(std::string &path, std::string method);
		bool isRedirection(std::string &path);
		bool isAutoindexOn();
		std::string updateResourcePath(std::string &path, int &statusCode);
};

#endif