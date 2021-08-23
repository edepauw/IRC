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
   if (s[0] == ':')
   {
      while (s[i] != ' ' && s[i] != '\n')
         ++i;
      s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
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
         s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
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
   else if (!_user[_data.it->first].getUserName().empty())
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
      resp = response("431", ":No nickname given");
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
   
  if (args.size() < 5)
      resp = response("461", "USER :Not enough parameters");
   else if (!_user[_data.it->first].getUserName().empty())
      resp = response("462", ":You may not reregister");
   else if (_user[_data.it->first].getPassword() != this->getPassword())
      resp = response("464", ":Password incorrect");
   else if (_user[_data.it->first].getNickName().empty())
      resp = response("420", ":Nick require");
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
   if (_chan.find(chan_name) == _chan.end())
   {
      _chan[chan_name];
      _chan[chan_name].addUser(fd);
      _chan[chan_name].addOper(fd);
      if (password != "")
         _chan[chan_name].setPass(password);
      _chan[chan_name].setName(chan_name);
      std::cout << "Create channel : " << chan_name << std::endl;
      if (password != "")
         std::cout << "With mdr :" << password;
	   printUserAndTopic(chan_name);
   }
   else
   {
    	if (_chan[chan_name].getPass() != password)
      {
         std::cout << "'" << _chan[chan_name].getPass() << "' TRUE: '" << password << std::endl;
        	std::string resp = response("475", chan_name + " :Cannot join channel (+k)");
        	send(_data.it->first , resp.c_str(), resp.length(), 0);
		}
    	else
    	{
		   int ret = _chan[chan_name].addUser(fd);
    	  	if (ret == 1)
    	  	   std::cout << "Already connected to channel : " << chan_name << std::endl;
			else if (ret == 2)
         {
    	  	   std::string resp = response("471", chan_name + " :Cannot join channel (+l)");
    	      send(_data.it->first , resp.c_str(), resp.length(), 0);
			}
    	  	else
    	  	   std::cout << "Join channel : " << chan_name << std::endl;
			
         _chan[chan_name].sendAll(sendMessage("JOIN", _chan[chan_name].getName(), ""));
         printUserAndTopic(chan_name);
    	}
   }
}

void	Server::printUserAndTopic(std::string chan_name){
	//Topic
	std::string resp = response("331", chan_name + " :No topic is set");
	send(_data.it->first , resp.c_str(), resp.length(), 0);
	//list of user
	std::list<int>::iterator it;
	std::string resp1 = chan_name + " :";
	for (it = _chan[chan_name].getUser().begin(); it != _chan[chan_name].getUser().end(); it++)
   {
		if (_chan[chan_name].isOpe(*it))
      {
			resp1 += " @" + _user[*it].getNickName();
      }
		else
			resp1 += " " + _user[*it].getNickName();
	}
	std::string resp2 = response("353", resp1);
   std::string resp3 = response("366", chan_name + " :End of /NAMES list");
	send(_data.it->first , sendMessage("JOIN", "", chan_name).c_str(), sendMessage("JOIN", "", chan_name).length(), 0);
   send(_data.it->first , resp2.c_str(), resp2.length(), 0);
   send(_data.it->first, resp3.c_str(), resp3.length(), 0);
   _chan[chan_name].sendAll(resp2);
   _chan[chan_name].sendAll(resp3);
}

void Server::join(std::vector<std::string> &args)
{
   std::vector<std::string> channel;
   std::vector<std::string> passw;
   std::vector<std::string>::iterator chan_it;
   std::vector<std::string>::iterator pass_it;
   std::vector<std::string>::iterator it;
   if (args.size() > 1)
      if (args[1].length() > 0)
         channel = isChannel(args[1]);
	if (channel.empty() == true)
   {
		std::string resp = response("403", " :No such channel");
      send(_data.it->first , resp.c_str(), resp.length(), 0);
	}
   if (args.size() > 2)
      if (args[2].length() > 0)
         passw = isPass(args[2]);
   chan_it = channel.begin();
   while(channel.size() > passw.size())
      passw.push_back("");
   pass_it = passw.begin();
   while (chan_it != channel.end())
   {
	   	if (chan_it->length() > 1)
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
      &Server::user
   };

   std::string _con[] =
   {
      "PASS",
      "NICK",
      "USER"
   };

   void (Server::*cmd[])(std::vector<std::string> &args) =
   {
      &Server::join,
	   &Server::oper,
	   &Server::privMsg,
      &Server::quit,
      &Server::notice,
      &Server::kill
   };

   std::string _cmd[] =
   {
      "JOIN",
	   "OPER",
	   "PRIVMSG",
      "QUIT",
      "NOTICE",
      "KILL"
   };

   _data.buffer[_data.ret_read] = 0;
   _user[_data.it->first].addCmd(_data.buffer);
   if (_user[_data.it->first].getCmd().length() != 0)
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
         if (args[0].length() == 1 || args[0].compare(1, args[0].length(), _user[_data.it->first].getNickName()) != 0)
            return;
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
      for (int i = 0; i < 6; i++)
      {
         if (_user[_data.it->first].getUserName().empty())
         {
            std::string resp(response("451", ":You have not registered batard"));
            send(_data.it->first , resp.c_str(), resp.length(), 0);
            return;
         }
         if (!args[0].compare(0, _cmd[i].length(), _cmd[i]))
         {
            (this->*cmd[i])(args);
            return;
         }
      }
      std::string resp(response("421", args[0] + " :Unknown command"));
      send(_data.it->first , resp.c_str(), resp.length(), 0);
   }
}

void Server::oper(std::vector<std::string> &args){
	int fd = getFd_ByName(args[1]);
   std::string resp;
   if (fd == - 1)
      resp = response("401", args[1] + " :No such nick");
	else if (_user[fd].isOper() == false)
   {
      if (args.size() == 1)
         resp = response("461", "OPER :Not enough parameters");
		else if (args[2] == PASS_OPE)
      {	
			_user[fd].setOper(true);
			resp = response("381", ":You are now an IRC operator");
		}
		else
			resp = response("464", ":Password incorrect");
	}
   send(_data.it->first , resp.c_str(), resp.length(), 0);
}

void Server::setBanFromServ(std::string channel, int fd){ _chan[channel].setBan(fd); }

std::vector<std::string> cutPrivMsg(std::string str)
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


std::string getMessage(std::vector<std::string> &args)
{
   std::vector<std::string>::iterator args_it;
   std::string msg;
   for (args_it = args.begin(); args_it != args.end(); args_it++)
   {
      msg += *args_it;
      msg += " ";
   }
   if (msg.find(":"))
      msg = msg.substr(msg.find(":") + 1, msg.length() - msg.find(":") + 1);
   else
      msg = msg.substr(msg.find_last_of(" ") + 1, msg.length() - msg.find_last_of(" ") + 1);
   return msg;
}

void Server::privMsg(std::vector<std::string> &args)
{
   std::vector<std::string> user;
   std::vector<std::string> all;
   std::vector<std::string> chan;
   std::vector<std::string>::iterator it;
   std::string msg;
   std::string cur;
   //x = cutby ',' args[1]
   all = cutPrivMsg(args[1]);
   if (args.size() < 2) // ERRO
      return;
   msg = args[2];
   for (it = all.begin(); it != all.end(); it++)
   {
      cur = *it;
      if ((cur[0] == '#' || cur[0] == '&' )&& cur.length() > 1)
         chan.push_back(cur);
      else if (cur.length() > 1)
         user.push_back(cur);
   }
   for (it = chan.begin(); it != chan.end(); it++)
   {
      if (_chan.find(*it) != _chan.end())
         _chan.find(*it)->second.sendAll(sendMessage("PRIVMSG", _chan.find(*it)->first, msg));
   }
   for (it = user.begin(); it != user.end(); it++)
   {
      if (_user.find(getFd_ByName(*it)) != _user.end())
         send(getFd_ByName(*it) , sendMessage("PRIVMSG", *it, msg).c_str(), sendMessage("PRIVMSG", *it, msg).length(), 0);
   }
}

void Server::quit(std::vector<std::string> &args)
{
   std::string resp;
   std::string message;
   if (args.size() > 1)
      message += " " + args[1];
   for (std::map<std::string, Channel>::iterator it =  _chan.begin(); it != _chan.end(); it++)
   {
      std::list<int>::iterator itt = std::find(it->second.getUser().begin(), it->second.getUser().end(), _data.it->first);
      if (itt != it->second.getUser().end())
      {
         for (std::list<int>::iterator ite = it->second.getUser().begin(); ite != it->second.getUser().end(); ite++)
         {
            if (ite != itt)
               send(*ite , sendMessage("QUIT", it->first, message).c_str(), sendMessage("QUIT", it->first, message).length(), 0);
         }
         it->second.removeUser(*itt);
      }
   }
   close(_data.it->first);
   FD_CLR(_data.it->first, &_data.m_set);
}

void Server::kill(std::vector<std::string> &args)
{
   int fd = getFd_ByName(args[1]);
   std::string resp;
   if (_user[_data.it->first].isOper() == false)
      //sendMessage("PRIVMSG", *it, msg).c_str(), sendMessage("PRIVMSG", *it, msg).length(), 0);
      resp = response("481", ":Permission Denied- You're not an IRC operator");
   else if (args.size() < 3)
      resp = response("461", "KILL :Not enough parameters");
   else if (fd == - 1)
      resp = response("401", args[1] + " :No such nick");
   if (!resp.empty())
      send(_data.it->first, resp.c_str(), resp.length(), 0);
   else
   {
      send(fd , sendMessage("KILL", args[1], args[2]).c_str(), sendMessage("KILL", args[1], args[2]).length(), 0);
      for (std::map<std::string, Channel>::iterator it =  _chan.begin(); it != _chan.end(); it++)
         it->second.removeUser(fd);
      close(fd);
      FD_CLR(fd, &_data.m_set);
   }
}

void Server::notice(std::vector<std::string> &args)
{
   if (args.size() > 2)
      if (getFd_ByName(args[1]) > 0)
         send(getFd_ByName(args[1]) , sendMessage("NOTICE", args[1], args[2]).c_str(), sendMessage("PRIVMSG", args[1], args[2]).length(), 0);
}

/*void Server::kick(std::vector<std::string> &args)
{
   std::string resp;
   int fdName = getFd_ByName(args[1]);
   std::string chan = args[2];

   if (args.size() < 3)
      resp = response("461", "KICK :Not enough parameters");
   else if (fd == - 1)
      resp = response("401", args[1] + " :No such channel");
   
              403 ERR_NOSUCHCHANNEL
"<nom de canal> :No such channel"
                            
          442 ERR_NOTONCHANNEL
"<canal> :You're not on that channel"

482 ERR_CHANOPRIVSNEEDED
"<canal> :You're not channel operator"

}*/