#include "Server.hpp"

void Server::disconnect()
{
   std::map<std::string, Channel>::iterator it;
   std::map<std::string, Channel>::iterator next_it;
   for (it = _chan.begin(), next_it = it; it != _chan.end(); it = next_it)
   {
      ++next_it;
      it->second.removeUser(_data.it->first);
      if (it->second.size() == 0)
      {
         _chan.erase(it);
      }
   }
}

void Server::closeServer(void)
{
   for (_data.it = _user.begin(); _data.it != _user.end(); _data.it++)
      if (FD_ISSET(_data.it->first, &_data.m_set))
         close(_data.it->first);
}