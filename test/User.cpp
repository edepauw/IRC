#include "User.hpp"

User::User(int fd): _fd(fd), _nick("*"), _password("")
{

}

User::~User()
{

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