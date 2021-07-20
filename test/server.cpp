#include "Server.hpp"

Server::Server(): _n_user(0)
{

}

Server::~Server()
{

}

void Server::addUser(int fd)
{
	User usr(fd);
	_user[fd] = usr;
	_n_user++;
}

void Server::setPort(int port)
{
	_port = port;
}

void Server::setPassword(std::string password)
{
	_password = password;
}

int	Server::getPort(void)
{
	return (_port);
}

std::string	Server::getPassword(void)
{
	return (_password);
}


void	Server::init(void)
{  
   if ((this->_data.master_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
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
   if(bind(_data.master_socket, reinterpret_cast<struct sockaddr*>(&_data.addr), _data.addrlen) < 0)
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
   FD_ZERO(&_data.readfds);
   FD_SET(_data.master_socket, &_data.readfds);
   _user[_data.master_socket];
   _data.max_sd = _data.master_socket;
}

void Server::loop(void)
{
    do
   {
      this->select_fun();
      for (_data.it = _user.begin(); _data.it != _user.end(); _data.it++)
      {
         if (FD_ISSET(_data.it->first, &_data.readfds)) // masterset[i] = I/O , qui est set via select() plus haut a 1 si le fd a eu de l'activite
         {
            //desc_ready -= 1;
            if (_data.it->first == _data.master_socket) // si i == listen_sd, activiter sur le fd du server, soit demande de connection
            {
               this->new_connection();
            }
            else // si i != listen_sd, il ne s'agis pas du server mais d'un client_fd en activiter
            {
               this->receive();
            }
        }
      }

    } while (TRUE);
}

void Server::select_fun(void)
{
   std::cout << "Waiting on select()..." << std::endl;
      if ((_data.ret_select = select(_data.max_sd + 1, &_data.readfds, NULL, NULL, NULL)) < 0)
      {
         perror("  select() failed");
         exit(EXIT_FAILURE);
      }
}

void Server::new_connection(void)
{
   printf("  Listening socket is readable\n");
   if ((_data.new_sd = accept(_data.master_socket, reinterpret_cast<struct sockaddr *>(&_data.addr), reinterpret_cast<socklen_t *>(&_data.addrlen))) < 0)
   {
      if (errno != EWOULDBLOCK)
         perror("  accept() failed");
      exit(EXIT_FAILURE);
   }
   printf("  New incoming connection - %d\n", _data.new_sd);
   FD_SET(_data.new_sd, &_data.readfds);
   addUser(_data.new_sd);
   if (_data.new_sd > _data.max_sd)
      _data.max_sd = _data.new_sd;
}

void Server::receive(void)
{
   printf("  Descriptor %d is readable\n", _data.it->first);
    if((_data.ret_read = recv(_data.it->first, _data.buffer, sizeof(_data.buffer), 0)) < 0)
   {
      perror("recv() failed");
      exit(EXIT_FAILURE);
   }
   if (_data.ret_read == 0)
   {
       _user.erase(_data.it->first);
       close(_data.it->first);
       FD_CLR(_data.it->first, &_data.readfds);
       if (_data.it->first == _data.max_sd)
       {
          while (FD_ISSET(_data.max_sd, &_data.readfds) == FALSE)
             _data.max_sd -= 1;
       }
   }
    //serv.parseMsg(//i ,buffer, serv);
    //serv.clearBuffer();
}

void Server::close_con(void)
{
   for (_data.it = _user.begin(); _data.it != _user.end(); _data.it++)
      if (FD_ISSET(_data.it->first, &_data.readfds))
         close(_data.it->first);
}