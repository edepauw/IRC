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
# include "Channel.hpp"

# define TRUE 1
# define FALSE 0
# define PASS_OPE "1234"
# define SERVER_NAME "ft_irc.com"
# define MAX_ON_CHAN 3

class Channel;

typedef struct s_data
{
    struct sockaddr_in              addr;
    int                             master_socket;
    int                             new_sd;
    int                             ret_select;
    int                             ret_read;
    char                            buffer[512];
    std::string                     strbuff;
    fd_set                          m_set;
    fd_set                          w_set;
    int                             opt;
    struct timeval                  timeout;
    int                             addrlen;
    int                             max_sd;
    std::map<int, User>::iterator   it;
    std::map<std::string, Channel>    itc;

    s_data(){opt = TRUE;};

}               t_data;

class Server
{
    public:
        // Class
        Server();
        ~Server();

        // Setter
        void setPort(int port);
        void setPassword(std::string password);

        // Getter
        int         getPort(void);
        std::string getPassword(void);
		int			getFd_ByName(std::string name);

        // Method
        void addUser(int fd);
        void init(void);
        void loop(void);
        std::string response(std::string num, std::string resp);
        std::string sendMessage(std::string cmd, std::string dest, std::string msg);

        // Command
        std::vector<std::string> cutMsg(std::string cmd);
        void parseMsg(void);
        void pass(std::vector<std::string> &args);
        void nick(std::vector<std::string> &args);
        void createOrJoinWithPass(std::string chan_name, std::string password);
        void user(std::vector<std::string> &args);
        void join(std::vector<std::string> &args);
		void setBanFromServ(std::string channel, int fd);
		void printUserAndTopic(std::string chan_name);
		void oper(std::vector<std::string> &args);
		void privMsg(std::vector<std::string> &args);
        void quit(std::vector<std::string> &args);
        void kill(std::vector<std::string> &args);
        void notice(std::vector<std::string> &args);
        void kick(std::vector<std::string> &args);

        // Close
        void disconnect( void );
        void closeServer(void);

    private:
        std::map<int, User>                     _user;
        std::map<std::string, Channel>          _chan;
        int                                     _port;
        std::string                             _password;
        t_data                                  _data;

        void clearBuffer(void);
        void newConnection(void);
        void receive(void);
        void selectFun(void);
		// Debug
		void showVector(std::vector<std::string> args);
};

#endif
