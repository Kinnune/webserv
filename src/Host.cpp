#include "Host.hpp"


//------------------------------------------------------------------------------
//	CONSTRUCTORS / DESTRUCTORS
//------------------------------------------------------------------------------

Host::Host()
{
	this->id = 0;
	this->portInt = 0;
	this->serverName = "";
	this->host = "";
	this->portString = "";
	this->root = "";
	this->autoIndex = autoIndexState::NONE;
}

Host::~Host()
{
}

Host::Host(Host const &other)
{
	*this = other;
}

Host &Host::operator=(Host const &other)
{
	this->id = other.id;
	this->portInt = other.portInt;
	this->serverName = other.serverName;
	this->host = other.host;
	this->portString = other.portString;
	this->root = other.root;
	this->autoIndex = other.autoIndex;
	this->index_pages = other.index_pages;
	this->methods = other.methods;
	this->errorPages = other.errorPages;
	this->locations = other.locations;
	return *this;
}


//------------------------------------------------------------------------------
//	GETTERS
//------------------------------------------------------------------------------

int Host::getId() const { return this->id; }
int Host::getPortInt() const { return this->portInt; }
std::string Host::getServerName() const { return this->serverName; }
std::string Host::getHost() const { return this->host; }
std::string Host::getPortString() const { return this->portString; }
std::string Host::getRoot() const { return this->root; }
autoIndexState Host::getAutoIndex() const { return this->autoIndex; }
std::vector<std::string> Host::getMethods() const { return this->methods; }
std::vector<std::string> Host::getIndexPages() const { return this->index_pages; }
std::vector<std::string> Host::getErrorPages() const { return this->errorPages; }
std::vector<Location> Host::getLocations() const { return this->locations; }


//------------------------------------------------------------------------------
//	SETTERS
//------------------------------------------------------------------------------

void Host::setId(int id) { this->id = id; }
void Host::setPortInt(int portInt) { this->portInt = portInt; }
void Host::setServerName(std::string serverName) { this->serverName = serverName; }
void Host::setHost(std::string host) { this->host = host; }
void Host::setPortString(std::string portString) { this->portString = portString; }
void Host::setRoot(std::string root) { this->root = root; }
void Host::setAutoIndex(autoIndexState autoIndex) { this->autoIndex = autoIndex; }
void Host::setMethods(std::vector<std::string> methods) { this->methods = methods; }
void Host::setIndexPages(std::vector<std::string> index_pages) { this->index_pages = index_pages; }
void Host::setErrorPages(std::vector<std::string> errorPages) { this->errorPages = errorPages; }
void Host::setLocations(std::vector<Location> locations) { this->locations = locations; }


//------------------------------------------------------------------------------
//	ADD
//------------------------------------------------------------------------------

void Host::addLocation(Location location) { this->locations.push_back(location); }


//------------------------------------------------------------------------------
//	METHODS
//------------------------------------------------------------------------------

bool Host::isAllowedCGI(std::string extension)
{
	for (std::vector<Location>::iterator loc = this->locations.begin(); loc != this->locations.end(); loc++)
	{
		std::vector<std::string> extensions = loc->getCgiExtensions();
		if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
			return true;
	}
	return false;
}