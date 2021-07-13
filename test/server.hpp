#ifndef SERVER_HPP
# define SERVER_HPP
#include <map>
#include <iostream>
#include <string>
class User
{
    public:
        User(int fd);
        User(User &src);
        User();
        void setPass(std::string pw);
        void setNick(std::string nick);
        ~User();
        int _fd;
        std::string _nick;
        std::string _password;
};

class Server
{
    public:
        Server();
        ~Server();
        void addUser(int &fd);
        std::map<int, User> user;
        // void incrUser(void);
        // void decrUser(void);
    private:
        int _n_user;
        int _port;
        int _password;
};

#endif
