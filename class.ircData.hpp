#ifndef CLASS_IRCDATA_HPP
# define CLASS_IRCDATA_HPP
#include <iostream>

class ircData
{
	public:
		ircData(int ac , char **av);
		~ircData();
		std::string av1;
		std::string host;
		std::string port_network;
		std::string pw_network;
		std::string port;
		std::string password;
};

#endif
