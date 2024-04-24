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

		std::string					_resourcePath;
		std::string					_requestedMethod;

		// Private methods
		bool isFile(const std::string &path);
		bool isDirectory(const std::string &path);
		bool locationExists(const std::string &location);
		bool allowedMethod(std::vector<std::string> methods, std::string method);
		void updateResourcePath();
		void handleLocation(Location &location);
		void handleNoLocation();
		void updateAutoIndex(autoIndexState state);
		void lookForIndexFile();

	
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
		autoIndexState getAutoIndex() const;
		std::vector<std::string> &getMethods();
		std::vector<std::string> &getIndexPages();
		std::vector<std::string> &getErrorPages();
		std::vector<Location> &getLocations();

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
		bool isAllowedCGI(std::string &extension);
		std::string updatePath(std::string const &path);
};

#endif