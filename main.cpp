#include "class.ircData.hpp"

int main(int ac, char **av)
{
	try{
		if (ac != 4)
			throw std::exception();
		ircData(ac, av);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
