#ifndef SERVER_HPP
# define SERVER_HPP

class User;

class Server
{
    public:
        Server();
        ~Server();
        User &getUser(int fd);
        void incrUser(void);
        void decrUser(void);
    private:
        int _n_user;
        int _port;
        int _password;
        User *user;
};

#endif
