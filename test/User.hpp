#ifndef USER_HPP
# define USER_HPP
# include <string>

class User
{
    public:
        User();
        User(int fd);
        ~User();
    
        void setPass(std::string pw);
        void setNick(std::string nick);
        void setFd(int fd);  

        std::string getNick(void);
        std::string getPassword(void);
        std::string getCmd(void);
        int getFd(void);

        void addCmd(std::string str);
        void resetCmd(void);

    private:
        int         _fd;
        std::string _nick;
        std::string _password;
        bool        _oper;
        std::string _cmd;
};

#endif