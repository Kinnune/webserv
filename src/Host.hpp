#ifndef HOST_HPP
#define HOST_HPP
#include "Location.hpp"
#include <vector>
#include <string>

// enum class autoIndexState { NONE, ON, OFF };

class Host
{
	private:
		int							id;
		int 						portInt;
		std::string 				serverName;
		std::string 				host;
		std::string					portString;
		std::string					root;
		autoIndexState				autoIndex;
		std::vector<std::string>	methods;
		std::vector<std::string>	index_pages;
		std::vector<std::string>	errorPages;
		std::vector<Location>		locations;
	
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
		std::vector<std::string> getMethods() const;
		std::vector<std::string> getIndexPages() const;
		std::vector<std::string> getErrorPages() const;
		std::vector<Location> getLocations() const;

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
		void addLocation(Location location);

		// Methods
		bool isAllowedCGI(std::string extension);
};

#endif