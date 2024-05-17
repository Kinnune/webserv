#ifndef LOCATION_HPP
#define LOCATION_HPP
#include <vector>
#include <string>


class Location
{
	private:
		int							redirectionCode;
		int							maxBody;
		std::string					location;
		std::string					root;
		std::string					alias;
		std::string					cgiPath;
		std::string					interpreter;
		std::string					redirection;
		bool						autoIndex;
		std::vector<std::string>	methods;
		std::vector<std::string>	index_pages;
		std::vector<std::string>	cgiExtensions;
	
	public:
		// Constructors/Destructors
		Location();
		~Location();
		Location(Location const &other);
		Location &operator=(Location const &other);

		// Getters
		int getRedirectionCode() const;
		int getMaxBody() const;
		bool getAutoIndex() const;
		std::string getLocation() const;
		std::string getRoot() const;
		std::string getAlias() const;
		std::string getCgiPath() const;
		std::string getInterpreter() const;
		std::string getRedirection() const;
		std::vector<std::string> getMethods() const;
		std::vector<std::string> getIndexPages() const;
		std::vector<std::string> getCgiExtensions() const;

		// Setters
		void setRedirectionCode(int redirectionCode);
		void setMaxBody(int maxBody);
		void setLocation(std::string location);
		void setRoot(std::string root);
		void setAlias(std::string alias);
		void setCgiPath(std::string cgiPath);
		void setInterpreter(std::string interpreter);
		void setRedirection(std::string redirection);
		void setAutoIndex(bool autoIndex);
		void setMethods(std::vector<std::string> methods);
		void setIndexPages(std::vector<std::string> index_pages);
		void setCgiExtensions(std::vector<std::string> cgiExtensions);
};

#endif