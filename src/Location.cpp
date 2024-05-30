
#include "Location.hpp"

//------------------------------------------------------------------------------
//	CONSTRUCTORS / DESTRUCTORS
//------------------------------------------------------------------------------

Location::Location()
{
	this->redirectionCode = 0;
	this->maxBody = -1;
	this->location = "";
	this->root = "";
	this->alias = "";
	this->cgiPath = "";
	this->interpreter = "";
	this->redirection = "";
	this->autoIndex = autoIndexState::OFF;
}

Location::~Location()
{
}

Location::Location(Location const &other)
{
	*this = other;
}

Location &Location::operator=(Location const &other)
{
	this->redirectionCode = other.redirectionCode;
	this->maxBody = other.maxBody;
	this->location = other.location;
	this->root = other.root;
	this->alias = other.alias;
	this->cgiPath = other.cgiPath;
	this->interpreter = other.interpreter;
	this->redirection = other.redirection;
	this->autoIndex = other.autoIndex;
	this->index_pages = other.index_pages;
	this->methods = other.methods;
	this->cgiExtensions = other.cgiExtensions;
	return *this;
}

//------------------------------------------------------------------------------
//	GETTERS
//------------------------------------------------------------------------------

int Location::getRedirectionCode() const { return this->redirectionCode; }
int Location::getMaxBody() const { return this->maxBody; }
std::string Location::getLocation() const { return this->location; }
std::string Location::getRoot() const { return this->root; }
std::string Location::getAlias() const { return this->alias; }
std::string Location::getCgiPath() const { return this->cgiPath; }
std::string Location::getInterpreter() const { return this->interpreter; }
std::string Location::getRedirection() const { return this->redirection; }
autoIndexState Location::getAutoIndex() const { return this->autoIndex; }
std::vector<std::string> Location::getIndexPages() const { return this->index_pages; }
std::vector<std::string> Location::getMethods() const { return this->methods; }
std::vector<std::string> Location::getCgiExtensions() const { return this->cgiExtensions; }

//------------------------------------------------------------------------------
//	SETTERS
//------------------------------------------------------------------------------

void Location::setRedirectionCode(int redirectionCode) { this->redirectionCode = redirectionCode; }
void Location::setMaxBody(int maxBody) { this->maxBody = maxBody; }
void Location::setLocation(std::string location) { this->location = location; }
void Location::setRoot(std::string root) { this->root = root; }
void Location::setAlias(std::string alias) { this->alias = alias; }
void Location::setCgiPath(std::string cgiPath) { this->cgiPath = cgiPath; }
void Location::setInterpreter(std::string interpreter) { this->interpreter = interpreter; }
void Location::setRedirection(std::string redirection) { this->redirection = redirection; }
void Location::setAutoIndex(autoIndexState autoIndex) { this->autoIndex = autoIndex; }
void Location::setIndexPages(std::vector<std::string> index_pages) { this->index_pages = index_pages; }
void Location::setMethods(std::vector<std::string> methods) { this->methods = methods; }
void Location::setCgiExtensions(std::vector<std::string> cgiExtensions) { this->cgiExtensions = cgiExtensions; }
