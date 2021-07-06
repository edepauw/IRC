#include "class.ircData.hpp"
#include <algorithm>


ircData::ircData(int ac , char **av): av1(av[1]), port(av[2]), password(av[3])
{
	int start = 0;
	int find = 0;
	if(std::count(av1.begin(), av1.end(), ':') != 2)
		throw std::exception();
	for (int i = 0; i < 3; i++)
	{
		find = av1.find(":", start);
		if(i == 0)
			host = av1.substr(start, find);
		if(i == 1)
			port_network = av1.substr(start, find - start);
		if(i == 2)
			pw_network = av1.substr(start, find - start);
		start = find + 1;
	}
	if(host.length() == 0 || port_network.length() == 0 || pw_network.length() == 0)
		throw std::exception();
}
ircData::~ircData()
{

}
