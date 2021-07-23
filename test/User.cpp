#include "User.hpp"

User::User(int fd): _fd(fd), _nick("*"), _password("")
{

}

User::User(): _fd(-1), _nick("*"), _password("")
{

}

User::~User()
{
    
}

void User::setFd(int fd)
{
    _fd = fd;
}

void User::setPass(std::string pw)
{
	_password = pw;
}

void User::setNick(std::string nick)
{
	_nick = nick;
}

std::string User::getNick(void)
{
    return (_nick);
}

std::string User::getPassword(void)
{
    return (_password);
}

int User::getFd(void)
{
    return (_fd);
}

std::string User::getCmd(void)
{
    return (_cmd);
}

void User::addCmd(std::string str)
{
    _cmd += str;
}

void User::resetCmd(void)
{
    _cmd.clear();
}