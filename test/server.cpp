#include "Server.hpp"

Server::Server()
{

}

Server::~Server()
{

}

void Server::addUser(int fd)
{
	User usr(fd);
	_user[fd] = usr;
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
      this->select_fun();
      for (_data.it = _user.begin(); _data.it != _user.end(); _data.it++)
      {
         if (FD_ISSET(_data.it->first, &_data.w_set)) // masterset[i] = I/O , qui est set via select() plus haut a 1 si le fd a eu de l'activite
         {
            //desc_ready -= 1;
            if (_data.it->first == _data.master_socket) // si i == listen_sd, activiter sur le fd du server, soit demande de connection
            {
               this->new_connection();
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

void Server::select_fun(void)
{
   memcpy(&_data.w_set, &_data.m_set, sizeof(_data.m_set));
   std::cout << "Waiting on select()..." << std::endl;
   if ((_data.ret_select = select(_data.max_sd + 1, &_data.w_set, NULL, NULL, NULL)) < 0)
   {
      perror("  select() failed");
      exit(EXIT_FAILURE);
   }
}

void Server::new_connection(void)
{
   std::cout << "  Listening socket is readable" << std::endl;
   if ((_data.new_sd = accept(_data.master_socket, reinterpret_cast<struct sockaddr *>(&_data.addr), reinterpret_cast<socklen_t *>(&_data.addrlen))) < 0)
   {
      if (errno != EWOULDBLOCK)
         perror("  accept() failed");
      exit(EXIT_FAILURE);
   }
   std::cout << "  New incoming connection - " << _data.new_sd << std::endl;
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

std::vector<std::string> Server::cutMsg(std::string cmd)
{
   std::vector<std::string> args;
   std::vector<std::string>::iterator it;
   std::string s(cmd);
   s = s.substr(0, s.find("\n"));
   std::cout << s << std::endl;
   size_t pos = 0;
   int start = 0;
   int i = 0;
   int stop = 0;
   bool doublefind = 0;
   if (s[0] == ':' && s[1] == ' ') // Pour le prefix
      return args;
   while (s[i] && i < s.length())
   {
      while (s[i] == ' ' && !doublefind)
         i++;
      start = i;
      while ((s[i] != ' ' || doublefind != false) && s[i] != ':' && i < s.length())
         i++;
      stop = i;
      i++;
      if (s.substr(start, stop - start).length() != 0)
         args.push_back(s.substr(start, stop - start));
      if (s[i] == ':')
         doublefind = true;
   }
   // for(it = args.begin(); it != args.end(); it++)
   //    std::cout << *it << std::endl;
   return (args);
}

void Server::pass(std::vector<std::string> &args)
{
   //serv.user[fd]._password = buff;
   std::cout << "message PASS received from" << std::endl;

   /*461 ERR_NEEDMOREPARAMS
"<commande> :Not enough parameters"

462 ERR_ALREADYREGISTRED
":You may not reregister"*/
}

void Server::nick(std::vector<std::string> &args)
{
   //serv.user[fd]._nick = buff;
   std::cout << "Nickname :" << std::endl;
   /*431 ERR_NONICKNAMEGIVEN
":No nickname given"
Renvoyé quand un paramètre pseudonyme attendu pour une commande n'est pas fourni.

432 ERR_ERRONEUSNICKNAME
"<pseudo> :Erroneus nickname"
Renvoyé après la réception d'un message NICK qui contient des caractères qui ne font pas partie du jeu autorisé. Voir les sections 1 et 2.2 pour les détails des pseudonymes valides.

433 ERR_NICKNAMEINUSE
"<nick> :Nickname is already in use"
Renvoyé quand le traitement d'un message NICK résulte en une tentative de changer de pseudonyme en un déjà existant.

436 ERR_NICKCOLLISION
"<nick> :Nickname collision KILL"*/
}

void Server::user(std::vector<std::string> &args)
{
   std::cout << "message USER received from : fd :" << _data.it->first <<std::endl;
   std::string resp = ":irc 001 alidy :Welcome to our FT_IRC project !\n";
   send(_data.it->first , resp.c_str(), resp.length(), 0);
}

void Server::parseMsg()
{
	void (Server::*ptr[])(std::vector<std::string> &args) =
   {
      &Server::pass,
      &Server::nick,
      &Server::user
   };

   std::string _name[] =
   {
      "PASS",
      "NICK",
      "USER"
   };

   _data.buffer[_data.ret_read] = 0;
   _user[_data.it->first].addCmd(_data.buffer);
   std::cout << "Commande du client: " << _user[_data.it->first].getCmd() << std::endl;
   
   if (_data.buffer[_data.ret_read - 1] == '\n') // Si la commande est terminée
   {
      std::vector<std::string> args;
      args = this->cutMsg(_user[_data.it->first].getCmd());
      _user[_data.it->first].resetCmd();
      for (int i = 0; i < 3; i++)
      {
         if (!args.empty() && !args[0].compare(0, _name[i].length(), _name[i]))
         {
            (this->*ptr[i])(args);
            return;
         }
      }
      std::cout << "Commande non trouvée !" << std::endl;
   }
}

void Server::close_con(void)
{
   for (_data.it = _user.begin(); _data.it != _user.end(); _data.it++)
      if (FD_ISSET(_data.it->first, &_data.m_set))
         close(_data.it->first);
}