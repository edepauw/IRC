#ifndef SERVER_HPP
# define SERVER_HPP
# include <map>
# include <iostream>
# include <string>
# include <stdio.h>
# include <unistd.h>
# include <cstring>
# include <iostream>
# include <stdlib.h>
# include <sys/ioctl.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <errno.h>
# include <fcntl.h>
# define SERVER_PORT  12345

class Server
{
    public:
        Server();
        ~Server();
        void setPort(int port);
        void setPassword(std::string password);
        void addUser(int fd);
        void start();
    private:
        std::map<int, User> _user;
        int _n_user;
        int _port;
        std::string _password;
};

#endif
