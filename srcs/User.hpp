#ifndef USER_HPP
# define USER_HPP
# include <string>
# include <iostream>
class User
{
    public:
        User();
        User(int fd);
        ~User();
    
        void setNickName(std::string nick);
        void setUserName(std::string user);
        void setRealName(std::string real);
        void setPassword(std::string pw);
        void setFd(int fd);  

        std::string getNickName(void);
        std::string getUserName(void);
        std::string getRealName(void);
        std::string getPassword(void);
        std::string getCmd(void);
        bool isOper(void);
		void setOper(bool n);
        int getFd(void);

        void addCmd(std::string str);
        void resetCmd(void);

    private:
        int         _fd;
        std::string _nickName;
        std::string _userName;
        std::string _realName;
        std::string _password;
        bool        _oper;
        std::string _cmd;
};

#endif