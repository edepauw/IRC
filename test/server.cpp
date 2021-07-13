#include "server.hpp"

Server::Server()
{

}
Server::~Server()
{

}
// User &Server::getUser(int fd);
// void Server::incrUser(void);
// void Server::decrUser(void);
User::User(int fd): _fd(fd), _nick("*"), _password("")
{

}
User::User()
{

}
User::~User()
{

}
// User::User(int fd): _fd(fd)
// {

// }
void Server::addUser(int &fd)
{
	User usr(fd);
	std::cout << fd << std::endl;
	user[fd] = usr;
}

void User::setPass(std::string pw)
{
	_password = pw;
}
void User::setNick(std::string nick)
{
	_nick = nick;
}
