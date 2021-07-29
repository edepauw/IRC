#include "Server.hpp"

std::vector<std::string> Server::cutMsg(std::string cmd)
{
   std::vector<std::string> args;
   std::vector<std::string>::iterator it;
   std::string s(cmd);

   size_t pos = 0;
   int start = 0;
   int i = 0;
   int stop = 0;
   bool doublefind = 0;
   if (s[0] == ':') // Pour le prefix
   {
      while (s[i] != ' ' && s[i] != '\n')
         ++i;
      s.erase(std::remove(s.begin(), s.end(), '\r'), s.end()); //erase \r, to work with netcat or irc client
      args.push_back(s.substr(0, i));
   }
   while (s[i] != '\n')
   {
      while (s[i] == ' ' && !doublefind && s[i] != '\n')
         ++i;
      start = i;
      while ((s[i] != ' ' || doublefind == true) && s[i] != ':' && s[i] != '\n')
         ++i;
      stop = i;
      if (s.substr(start, stop - start).length() != 0)
      {
         s.erase(std::remove(s.begin(), s.end(), '\r'), s.end()); //erase \r, to work with netcat or irc client
         args.push_back(s.substr(start, stop - start));
      }
      if (s[i] == ':')
      {
         doublefind = true;
         i++;
      }
   }
   /*std::cout << "Args: " << std::endl;
   for(it = args.begin(); it != args.end(); it++)
      std::cout << "'" << *it  << "'"  << std::endl;*/
   return (args);
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
   /*else if (args[1].size() > 8) // A check les conditions
   {
      resp = response("432", args[1] + " :Erroneus nickname");
   }*/
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
      resp = response("464", ":Password incorrect");
   else if (_user[_data.it->first].getNickName().empty())
      resp = response("Error", ":Nick require");  // Voir ce qu'il faut renvoyer
   // Faut il check si userName est deja pris comme Nick ?
   else
   {
      _user[_data.it->first].setUserName(args[1]);
      _user[_data.it->first].setRealName(args[4]);
      std::cout << _user[_data.it->first].getRealName() << std::endl;
      resp = response("001", ":Welcome to the Internet Relay Network");
   }
   send(_data.it->first , resp.c_str(), resp.length(), 0);
}

std::vector<std::string> isPass(std::string str)
{
   std::vector<std::string> temp;
   int pos = 0;
   if(str.find(',') == std::string::npos && str.length() > 1)
   {
      temp.push_back(str);
      return (temp);
   }
   while((pos = str.find(',')) != std::string::npos)
   { 
      temp.push_back((str.substr(0, pos)));
      str.erase(0, pos + 1);
   }
   temp.push_back((str.substr(0, pos)));
   str.erase(0, pos);
   return (temp);
}

std::vector<std::string> isChannel(std::string str)
{
   std::vector<std::string> temp;
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
void Server::createOrJoinWithPass(std::string chan_name, std::string password)
{
   int fd = _data.it->first;
   if( _chan.find(chan_name) == _chan.end())
   {
      _chan[chan_name];
      _chan[chan_name].addUser(fd);
      _chan[chan_name].addOper(fd);
      if (password != "")
         _chan[chan_name].setPass(password);
      _chan[chan_name].setName(chan_name);
      std::cout << "Create channel : " << chan_name << std::endl;
   }
   else
   {

      if (_chan[chan_name].getPass() != password)
         std::cout << "Bad password" << std::endl;
      else
      {
         if (_chan[chan_name].addUser(fd))
            std::cout << "Already connected to channel : " << chan_name << std::endl;
         else
            std::cout << "Join channel : " << chan_name << std::endl;
      }
   }
}

void Server::join(std::vector<std::string> &args)
{
   std::vector<std::string> channel;
   std::vector<std::string> passw;
   std::vector<std::string>::iterator chan_it;
   std::vector<std::string>::iterator pass_it;
   std::vector<std::string>::iterator it;
   // showVector(args);
   if (args.size() > 1)
      if (args[1].length() > 0)
         channel = isChannel(args[1]);
   if (args.size() > 2)
      if (args[2].length() > 0)
         passw = isPass(args[2]);
   chan_it = channel.begin();
   while(channel.size() > passw.size())
      passw.push_back("");
   pass_it = passw.begin();
   while (chan_it != channel.end())
   {
      createOrJoinWithPass(*chan_it, *pass_it);
      chan_it++;
      pass_it++;
   }
}

void Server::parseMsg()
{
	void (Server::*con[])(std::vector<std::string> &args) =
   {
      &Server::pass,
      &Server::nick,
      &Server::user,
   };

   std::string _con[] =
   {
      "PASS",
      "NICK",
      "USER"
   };

   void (Server::*cmd[])(std::vector<std::string> &args) =
   {
      &Server::join
   };

   std::string _cmd[] =
   {
      "JOIN"
   };

   _data.buffer[_data.ret_read] = 0;
   _user[_data.it->first].addCmd(_data.buffer);
   std::cout << "Commande du client: " << "'" << _user[_data.it->first].getCmd() << "'" << std::endl;
   if (_data.buffer[_data.ret_read - 1] == '\n') // Si la commande est terminée
   {
      std::vector<std::string> args;
      args = this->cutMsg(_user[_data.it->first].getCmd());
      _user[_data.it->first].resetCmd();
      if (args.empty())
         return;
      if (args[0][0] == ':') 
      {
         if (args[0].length() == 1 || args[0].compare(1, args[0].length(), _user[_data.it->first].getNickName()) != 0) // A VOIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIR
         { 
            
            std::cout << "BAD NICK" << std::endl;
            return;
         }
         else
            args.erase(args.begin());
      }
      for (int i = 0; i < 3; i++)
      {
         if (!args[0].compare(0, _con[i].length(), _con[i]))
         {
            (this->*con[i])(args);
            return;
         }
      }
      for (int i = 0; i < 1; i++)
      {
         if (_user[_data.it->first].getUserName().empty())
         {
            std::string resp(response("451", ":You have not registered"));
            send(_data.it->first , resp.c_str(), resp.length(), 0);
            return;
         }
         if (!args[0].compare(0, _cmd[i].length(), _cmd[i]))
         {
            (this->*cmd[i])(args);
            return;
         }
      }
      std::cout << "Commande non trouvée !" << std::endl;
   }
}