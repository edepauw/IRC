#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Server.hpp"

class Channel{

    public:
    //constructor and destructor
        Channel(void);
        ~Channel(void);
    //Getter and Setter
    int    removeUser(int fd);
    int    addUser(int fd);
    int    addOper(int fd);
    std::string getName();
    std::string getPass();

    void        setPass(std::string str);
    void        setName(std::string str);
    void        setBool(bool b);
    int         size();

    private:
    std::list<int>    _userFd;
    std::list<int>    _operFd;
    std::string        _name;
    std::string        _passw;
    bool            _boolPw;

};

#endif