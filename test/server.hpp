#ifndef SERVER_HPP
# define SERVER_HPP
# include <map>
# include <list>
# include <iostream>
# include <string>
# include <cstring>
# include <iostream>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <cerrno>
# include <fcntl.h>
# include <sys/ioctl.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/select.h>
# include <sys/time.h>
# include <netinet/in.h>
# include "User.hpp"

#define TRUE 1
#define FALSE 0

typedef struct s_data
{
    struct sockaddr_in              addr;
    int                             master_socket;
    int                             new_sd;
    int                             ret_select;
    int                             ret_read;
    char*                           buffer[1025];
    fd_set                          readfds;
    int                             opt;
    struct timeval                  timeout;
    int                             addrlen;
    int                             max_sd;
    std::map<int, User>::iterator   it;

    s_data(){opt = TRUE;};

}               t_data;

class Server
{
    public:
        Server();
        ~Server();

        void setPort(int port);
        void setPassword(std::string password);
        void addUser(int fd);

        int         getPort(void);
        std::string getPassword(void);


        void init(void);
        void loop(void);
        void close_con(void);

    private:
        std::map<int, User>                     _user;
        std::map<std::string, std::list<int> >  _chan;
        //int                                     _n_user;
        int                                     _port;
        std::string                             _password;
        t_data                                  _data;

        void clearBuffer(void);
        void new_connection(void);
        void receive(void);
        void select_fun(void);
};

#endif
