#include "Client.hpp"

Client::Client() {}

Client::~Client() {}

void Client::setFd(int fd) {
	this->_fd = fd;
}

void Client::setPort(int port) {
	this->_port = port;
}

void Client::setServer(Server &server) {
	this->_server = server;
}

int Client::getFd() const {
	return this->_fd;
}

int Client::getPort() const {
	return this->_port;
}

Server &Client::getServer() const {
	return this->_server;
}