#include "Server.hpp"

// Class

Server::Server(){ }

Server::~Server(){ }

// Setter

void Server::setPort( int port ){ _port = port; }

void Server::setPassword( std::string password ){ _password = password; }

// Getter

int Server::getPort(void){ return (_port); }

std::string	Server::getPassword( void ){ return (_password); }

// Method

void Server::addUser(int fd)
{
	User usr(fd);
	_user[fd] = usr;
}

void	Server::init(void)
{  
   if ((this->_data.master_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      perror("socket() failed");
      exit(EXIT_FAILURE);
   }
   if (setsockopt(this->_data.master_socket, SOL_SOCKET,  SO_REUSEADDR, (char *)&_data.opt, sizeof(_data.opt)) < 0)
   {
      perror("setsockopt() failed");
      close(_data.master_socket);
      exit(EXIT_FAILURE);
   }
   fcntl(this->_data.master_socket, F_SETFL, O_NONBLOCK);
   memset(&_data.addr, 0, sizeof(_data.addr));
   _data.addr.sin_family      = AF_INET;
   _data.addr.sin_addr.s_addr = htonl(INADDR_ANY);
   _data.addr.sin_port        = htons(_port);
   _data.addrlen = sizeof(_data.addr);
   if (bind(_data.master_socket, reinterpret_cast<struct sockaddr*>(&_data.addr), _data.addrlen) < 0)
   {
      perror("bind() failed");
      close(_data.master_socket);
      exit(EXIT_FAILURE);
   }
   if ((listen(_data.master_socket, 3) < 0))
   {
      perror("listen() failed");
      close(_data.master_socket);
      exit(EXIT_FAILURE);
   }
   FD_ZERO(&_data.m_set);
   FD_SET(_data.master_socket, &_data.m_set);
   _user[_data.master_socket];
   _data.max_sd = _data.master_socket;
}

void Server::loop(void)
{
   do
   {
      this->selectFun();
      for (_data.it = _user.begin(); _data.it != _user.end(); _data.it++)
      {
         if (FD_ISSET(_data.it->first, &_data.w_set)) // masterset[i] = I/O , qui est set via select() plus haut a 1 si le fd a eu de l'activite
         {
            //desc_ready -= 1;
            if (_data.it->first == _data.master_socket) // si i == listen_sd, activiter sur le fd du server, soit demande de connection
            {
               this->newConnection();
            }
            else // si i != listen_sd, il ne s'agis pas du server mais d'un client_fd en activiter
            {
               this->receive();
               break;
            }
        }
      }
   } while (TRUE);
}

void Server::selectFun(void)
{
   memcpy(&_data.w_set, &_data.m_set, sizeof(_data.m_set));
   std::cout << "Waiting on select()..." << std::endl;
   if ((_data.ret_select = select(_data.max_sd + 1, &_data.w_set, NULL, NULL, NULL)) < 0)
   {
      perror("  select() failed");
      exit(EXIT_FAILURE);
   }
}

void Server::newConnection(void)
{
   std::cout << "  Listening socket is readable" << std::endl;
   if ((_data.new_sd = accept(_data.master_socket, reinterpret_cast<struct sockaddr *>(&_data.addr), reinterpret_cast<socklen_t *>(&_data.addrlen))) < 0)
   {
      if (errno != EWOULDBLOCK)
         perror("  accept() failed");
      exit(EXIT_FAILURE);
   }
   std::cout << "  New incoming connection [ " << _data.new_sd << " ]" << std::endl;
   FD_SET(_data.new_sd, &_data.m_set);
   addUser(_data.new_sd);
   if (_data.new_sd > _data.max_sd)
      _data.max_sd = _data.new_sd;
}

void Server::receive(void)
{
   std::cout << "  Descriptor "<< _data.it->first << " is readable" << std::endl;
   if((_data.ret_read = recv(_data.it->first, _data.buffer, sizeof(_data.buffer), 0)) < 0)
   {
      perror("recv() failed");
      exit(EXIT_FAILURE);
   }
   if (_data.ret_read == 0)
   {
      disconnect();
      _user.erase(_data.it->first);
      close(_data.it->first);
      FD_CLR(_data.it->first, &_data.m_set);
      if (_data.it->first == _data.max_sd)
      {
         while (FD_ISSET(_data.max_sd, &_data.m_set) == FALSE)
            _data.max_sd -= 1;
      }
   }
   this->parseMsg();
   //serv.clearBuffer();
}

std::string Server::response(std::string num, std::string resp)
{
   std::string res(":");
   res += SERVER_NAME;
   res += " ";
   res += num;
   res += " ";
   if (!_user[_data.it->first].getNickName().empty()) // PASS ?
   {
      res += _user[_data.it->first].getNickName();
      res += " ";
   }
   res += resp;
   res += "\r\n";
   return res;
}

std::string Server::sendMessage(std::string cmd, std::string dest, std::string msg)
{
   std::string mes(":");  // <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
   mes += _user[_data.it->first].getNickName();
   mes += "!";
   mes += _user[_data.it->first].getUserName();
   mes += "@IPhost ";
   mes += cmd;
   mes += " ";
   if (dest.empty() == false)
   {
      mes += " ";
      mes += dest;
   }
   if (msg != "")
   {
      mes += " :";
      mes += msg;
   }
   mes += "\r\n";
   return (mes);
}

void Server::showVector(std::vector<std::string> args)
{
   std::vector<std::string>::iterator it;
   for (it = args.begin(); it != args.end(); it++)
   {
      std::cout << *it << std::endl;
   }
   return;
}

int	Server::getFd_ByName(std::string name)
{
	std::map<int, User>::iterator it;
	for (it = _user.begin(); it != _user.end(); it++)
   {
		if (it->second.getNickName() == name)
			return it->first;
	}
	return (-1);
}
