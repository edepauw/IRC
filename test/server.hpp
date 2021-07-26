#ifndef SERVER_HPP
# define SERVER_HPP
# include <map>
# include <vector>
# include <list>
# include <string>
# include <cstring>
# include <iostream>
# include <iomanip>
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

# define TRUE 1
# define FALSE 0
# define PASS_OPE 1234
# define SERVER_NAME "ft_irc.com"

typedef struct s_data
{
    struct sockaddr_in              addr;
    int                             master_socket;
    int                             new_sd;
    int                             ret_select;
    int                             ret_read;
    char                            buffer[512];
    fd_set                          m_set;
    fd_set                          w_set;
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

        // Server
        void init(void);
        void loop(void);

        // Command
        std::vector<std::string> cutMsg(std::string cmd);
        void parseMsg(void);
        void pass(std::vector<std::string> &args);
        void nick(std::vector<std::string> &args);
        void user(std::vector<std::string> &args);

        // Close
        void close_con(void);

    private:
        std::map<int, User>                     _user;
        std::map<std::string, std::list<int> >  _chan;
        int                                     _port;
        std::string                             _password;
        t_data                                  _data;

        void clearBuffer(void);
        void new_connection(void);
        void receive(void);
        void select_fun(void);
};

#endif
