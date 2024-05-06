#ifndef HOST_HPP
#define HOST_HPP
#include <string>
#include <vector>
#include "Location.hpp"
#include "utilityHeader.hpp"

// enum class autoIndexState { NONE, ON, OFF };

class Host
{
	private:
		int							_id;
		int 						_portInt;
		int							_statusCode;
		std::string 				_serverName;
		std::string 				_host;
		std::string					_portString;
		std::string					_root;
		autoIndexState				_autoIndex;
		std::vector<std::string>	_methods;
		std::vector<std::string>	_indexPages;
		std::vector<std::string>	_errorPages;
		std::vector<Location>		_locations;
		bool						_dirList;

		// Private methods
		bool isFile(std::string const &path);
		bool isDirectory(std::string const &path);
		bool locationExists(std::string const &path, std::string const &location);
		void handleLocation(std::string &path, Location &location);
		void handleNoLocation(std::string &path);
		void updateAutoIndex(autoIndexState state);
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
		std::string getServerName() const;
		std::string getHost() const;
		std::string getPortString() const;
		std::string getRoot() const;
		std::string getInterpreter(std::string &path, const std::string &extension);
		autoIndexState getAutoIndex() const;
		std::vector<std::string> &getMethods();
		std::vector<std::string> &getIndexPages();
		std::vector<std::string> &getErrorPages();
		std::vector<Location> &getLocations();
		bool getDirList(void);

		// Setters
		void setId(int id);
		void setPortInt(int portInt);
		void setServerName(std::string serverName);
		void setHost(std::string host);
		void setPortString(std::string portString);
		void setRoot(std::string root);
		void setAutoIndex(autoIndexState autoIndex);
		void setMethods(std::vector<std::string> methods);
		void setIndexPages(std::vector<std::string> index_pages);
		void setErrorPages(std::vector<std::string> errorPages);
		void setLocations(std::vector<Location> locations);

		// Add
		void addLocation(Location &location);

		// Methods
		bool isAllowedCGI(std::string &path, std::string &extension);
		bool isAllowedMethod(std::string &path, std::string method);
		bool isRedirection(std::string &path);
		bool isAutoindexOn();
		std::string updateResourcePath(std::string path);
};

#endif