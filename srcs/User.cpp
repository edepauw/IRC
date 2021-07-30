#include "User.hpp"


// Class

User::User(): _fd(-1){ }

User::User(int fd): _fd(fd){ }

User::~User()
{
	std::cout << "User " << _fd << ": has been deleted" << std::endl;
}


// Setter

void User::setFd( int fd ){ _fd = fd; }

void User::setPassword( std::string pw ){ _password = pw; }

void User::setNickName( std::string nick ){ _nickName = nick; }

void User::setUserName( std::string user ){ _userName = user; }

void User::setRealName( std::string user ){ _realName = user; }

void User::setOper( bool n ){
	if (n == true)
		_oper = true;
	else
		_oper = false;
}


// Getter

std::string User::getNickName( void ){ return (_nickName); }

std::string User::getUserName( void ){ return (_userName); }

std::string User::getPassword( void ){ return (_password); }

std::string User::getRealName( void ){ return (_realName); }

bool User::isOper( void ){ return (_oper); }

int User::getFd( void ){ return (_fd); }

std::string User::getCmd(void){ return (_cmd); }

// Method

void User::addCmd( std::string str ){ _cmd += str; }

void User::resetCmd( void ){ _cmd.clear(); }