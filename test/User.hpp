#ifndef USER_HPP
# define USER_HPP
# include <string>

class User
{
    public:
        User(int fd);
        ~User();
        void        setPass(std::string pw);
        void        setNick(std::string nick);
        std::string getNick(void);
        std::string getPassword(void);
    private:
        User();
        int         _fd;
        std::string _nick;
        std::string _password;
};

#endif