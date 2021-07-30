#include "Channel.hpp"

Channel::Channel(void): _passw("")
{
}

Channel::~Channel(void){
    std::cout << "channel : " << _name << " deleted" << std::endl;
}

int Channel::removeUser(int fd){
    std::list<int>::iterator it;
    for (it = _userFd.begin(); it != _userFd.end(); it++){
        if (*it == fd){
            _userFd.erase(it);
            return 0;
        }
    }
    for (it = _operFd.begin(); it != _operFd.end(); it++){
        if (*it == fd){
            _operFd.erase(it);
            return 0;
        }
    }
    return 1;
}

void        Channel::setPass(std::string pw)
{
    _passw = pw;
}

bool	Channel::isFd(int fd){
	std::list<int>::iterator it;
	for (it = _userFd.begin(); it != _userFd.end(); it++){
		if (*it == fd)
			return true;
	}
	return false;
}

bool	Channel::isOpe(int fd){
	std::list<int>::iterator it;
	for (it = _operFd.begin(); it != _operFd.end(); it++){
		if (*it == fd)
			return true;
	}
	return false;
}

void        Channel::setName(std::string name)
{
    _name = name;
}

int Channel::size()
{
    std::list<int>::iterator it;
    int ret = 0;
    for (it = _userFd.begin(); it != _userFd.end(); it++){
        ret++;
    }
    return (ret);
}

std::list<int> & Channel::getUser()
{
    return _userFd;
}

std::list<int> & Channel::getOper()
{
    return _operFd;
}

std::ostream &operator<<(std::ostream &os, Channel &src)
{
    std::list<int>::iterator it;
    os << "_userFd : " << std::endl;
    for (it = src.getUser().begin(); it != src.getUser().end(); it++)
        os << *it << std::endl;
    //os << "_operFd : " << std::endl;
    //for (it = src.getOper().begin(); it != src.getOper().end(); it++)
    //    os << *it << std::endl;
	return os;
}

int Channel::addUser(int fd){
    std::list<int>::iterator it;
	int ret = 0;
	if (_userFd.size() < MAX_ON_CHAN)
    {
		for (it = _userFd.begin(); it != _userFd.end(); it++){
        if (*it == fd)
            ret = 1;
			break;
   		}
		_userFd.insert(_userFd.begin(), fd);
     	//_userFd.push_back(fd);
     	ret = 0;
		std::cout << *this << std::endl;
		return ret;
	}
	std::cout << *this << std::endl;
	return 2;
}

int Channel::addOper(int fd){
    std::list<int>::iterator it;
    for (it = _userFd.begin(); it != _operFd.end(); it++){
        if (*it == fd)
            return 1;
    }
     _operFd.push_back(fd);
     return 0;
}

std::string Channel::getName(){
    return _name;
}

std::string Channel::getPass(){
    return _passw;
}

bool		Channel::isBanned(int fd){
		std::list<int>::iterator it;
		for (it = _banFd.begin(); it != _banFd.end(); it++)
			if (*it == fd)
				return true;
		return false;
}

void		Channel::setBan(int fd){
	if (isBanned(fd) == false)
		_banFd.push_back(fd);
	else
		std::cout << fd << " : is already ban" << std::endl;
}

void        Channel::sendAll(std::string str)
{
    int fd;
    std::list<int>::iterator it;
    for (it = _userFd.begin(); it != _userFd.end(); it++)
    {
        fd = *it;
        send(fd , str.c_str(), str.length(), 0);
    }
}