#include "Server.hpp"

int port_valid(char *str)
{
	int i = 0;
	int res = 0;

	while (str[i] >= '0' && str[i] <= '9' && res <= 65535)
	{
		res = res * 10 + str[i] - 48;
		i++;
	}
    if (str[i] != 0)
        return -1;
	return res;
}

int get_input(Server &serv, int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "./ircserv <port> <password>" << std::endl;
        return (0);
    }
    serv.setPort(port_valid(argv[1]));
    if (serv.getPort() == -1)
    {
        std::cerr << "The port must be between 0 and 65535" << std::endl;
        return (0);
    }
    serv.setPassword(std::string(argv[2]));
    std::cout << "| Server   Name     : " << SERVER_NAME << std::endl;
    std::cout << "| Server   Password : " << serv.getPassword() << std::endl;
    std::cout << "| Operator Password : " << PASS_OPE  << std::endl << std::endl;
    return (1);
}

int main(int argc, char **argv)
{
    Server serv;
    if (!get_input(serv, argc, argv))
        return (0);
    serv.init();
    serv.loop();
    serv.closeServer();
    return (0);
}
