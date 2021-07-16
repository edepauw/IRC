#include "Server.hpp"

Server::Server(): _n_user(0), _port(SERVER_PORT)
{

}

Server::~Server()
{

}

void Server::addUser(int fd)
{
	User usr(fd);
	_user.insert({fd, usr});
	_n_user++;
}

void Server::setPort(int port)
{
	_port = port;
}

void Server::setPassword(std::string password)
{
	_password = password;
}
