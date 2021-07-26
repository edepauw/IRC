#include "User.hpp"


// Class

User::User(int fd): _fd(fd){ }

User::User(): _fd(-1){ }

User::~User(){ }


// Setter

void User::setFd( int fd ){ _fd = fd; }

void User::setPass( std::string pw ){ _password = pw; }

void User::setNickName( std::string nick ){ _nickName = nick; }

void User::setUserName( std::string user ){ _userName = user; }

// Getter

std::string User::getNickName( void ){ return (_nickName); }

std::string User::getUserName( void ){ return (_userName); }

std::string User::getPassword( void ){ return (_password); }

bool User::isOper( void ){ return (_oper); }

int User::getFd( void ){ return (_fd); }

std::string User::getCmd(void){ return (_cmd); }

// Method

void User::addCmd( std::string str ){ _cmd += str; }

void User::resetCmd( void ){ _cmd.clear(); }