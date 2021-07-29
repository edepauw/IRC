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
int Channel::addUser(int fd){
    std::list<int>::iterator it;
    for (it = _userFd.begin(); it != _userFd.end(); it++){
        if (*it == fd)
            return 1;
    }
     _userFd.push_back(fd);
     return 0;
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