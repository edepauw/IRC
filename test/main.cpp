#include "Server.hpp"

bool only_digit(char *str)
{
    for(int i = 0; i < std::strlen(str); ++i)
    {
        if (std::isdigit(str[i]) == 0)
            return false;
    }
    return true;
}

Server get_input(int argc, char **argv)
{
    Server serv();
    if (argc != 3 || only_digit(argv[1]) == false)
        throw std::string("./ircserv <port> <password>");
    serv.setPort(); 65535
    serv.setPassword(std::string(argv[2]));
}

int main(int argc, char **argv)
{
    try
    {
        get_input(argc, argv);
    }
    catch (std::string const& err)
    {
        std::cerr << err << std::endl;
    }
    Server.start();
    return (0);
}