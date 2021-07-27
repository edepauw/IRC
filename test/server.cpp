#include "server.hpp"

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

std::string Server::response(std::string num, std::string resp)
{
   std::string res(":");

   res += SERVER_NAME;
   res += " ";
   res += num;
   if (!_user[_data.it->first].getNickName().empty()) // PASS ?
   {
      res += " ";
      res += _user[_data.it->first].getNickName();
   }
   res += " ";
   res += resp;
   res += "\n";
   return res;
}

void Server::pass(std::vector<std::string> &args)
{
   std::string resp;
   if (args.size() == 1)
      resp = response("461", "PASS :Not enough parameters");
   else if (!_user[_data.it->first].getUserName().empty()) // UserName n'est set que si le client est co
      resp = response("462", ":You may not reregister");
   if (!resp.empty())
      send(_data.it->first , resp.c_str(), resp.length(), 0);
   else
      _user[_data.it->first].setPassword(args[1]);
}

void Server::nick(std::vector<std::string> &args)
{
   std::string resp;

   if (args.size() == 1)
   {
      resp = response("431", ":No nickname given");
   }
   else if (args[1].size() > 8) // A check les conditions
   {
      resp = response("432", args[1] + " :Erroneus nickname");
   }
   else
   {
      for (std::map<int, User>::iterator it = _user.begin(); it != _user.end(); ++it)
      {
         if (!it->second.getNickName().compare(args[1]))
            resp = response("433", it->second.getNickName() + " :Nickname is already in use");
      }
   }
   if (!resp.empty())
      send(_data.it->first , resp.c_str(), resp.length(), 0);
   else
      _user[_data.it->first].setNickName(args[1]);
}

void Server::user(std::vector<std::string> &args)
{
   std::string resp;
   if (args.size() < 5) // Gerer si trop ???? (impossible puisque realname doit etre avec :)
      resp = response("461", "USER :Not enough parameters");
   else if (!_user[_data.it->first].getUserName().empty()) // UserName n'est set que si le client est co
      resp = response("462", ":You may not reregister");
   else if (_user[_data.it->first].getPassword() != this->getPassword())
      resp = response("Error", ":Bad password"); // Voir ce qu'il faut renvoyer
   else if (_user[_data.it->first].getNickName().empty())
      resp = response("Error", ":Nick require");  // Voir ce qu'il faut renvoyer
   // Faut il check si userName est deja pris comme Nick ?
   else
   {
      _user[_data.it->first].setUserName(args[1]);
      _user[_data.it->first].setRealName(args[4]);
      resp = response("001", ":Welcome to the Internet Relay Network");
   }
   send(_data.it->first , resp.c_str(), resp.length(), 0);
}

void showVector(std::vector<std::string> args)
{
   std::vector<std::string>::iterator it;
   for (it = args.begin(); it != args.end(); it++)
   {
      std::cout << *it << std::endl;
   }
   return;
}

std::list<std::string> isChannel(std::string str)
{
   std::list<std::string> temp;
   int pos = 0;
   if(str.find(',') == std::string::npos && (str[0] == '#'  ||  str[0] == '&') && str.length() > 1)
   {
      temp.push_back(str);
      return (temp);
   }
   while((pos = str.find(',')) != std::string::npos)
   { 
      if (str[0] != '#'  &&  str[0] != '&')
         return temp;
      temp.push_back((str.substr(0, pos)));
      str.erase(0, pos + 1);
   }
   if (str[0] != '#'  &&  str[0] != '&')
         return temp;
   temp.push_back((str.substr(0, pos)));
   str.erase(0, pos);
   return (temp);
}

void Server::join(std::vector<std::string> &args)
{
   std::list<std::string> temp;
   std::list<std::string>::iterator tit;
   std::vector<std::string>::iterator it;
   showVector(args);
   if (args[1].length() > 0)
      temp = isChannel(args[1]);
   for (tit = temp.begin(); tit != temp.end(); tit++)
   {
      std::cout << "list :" << *tit << std::endl;
   }
   
}

void Server::parseMsg()
{
	void (Server::*ptr[])(std::vector<std::string> &args) =
   {
      &Server::pass,
      &Server::nick,
      &Server::user,
      &Server::join
   };

   std::string _name[] =
   {
      "PASS",
      "NICK",
      "USER",
      "JOIN"
   };

   _data.buffer[_data.ret_read] = 0;
   _user[_data.it->first].addCmd(_data.buffer);
   std::cout << "Commande du client: " << _user[_data.it->first].getCmd() << std::endl;
   
   if (_data.buffer[_data.ret_read - 1] == '\n') // Si la commande est terminée
   {
      std::vector<std::string> args;
      args = this->cutMsg(_user[_data.it->first].getCmd());
      _user[_data.it->first].resetCmd();
      for (int i = 0; i < 4; i++)
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