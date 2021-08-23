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
	bool		isBanned(int fd);
	void		setBan(int fd);
	bool		isFd(int fd);
	bool		isOpe(int fd);
	void        sendAll(std::string str);

	std::list<int> & getUser();
	std::list<int> & getOper();

    private:
    std::list<int>		_userFd;
    std::list<int>		_operFd;
	std::list<int>		_banFd; 
    std::string        	_name;
    std::string        	_passw;
    bool            	_boolPw;

};
std::ostream &operator<<(std::ostream &os, Channel &src);

#endif