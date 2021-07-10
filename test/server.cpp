#include "server.hpp"

Server::Server();
Server::~Server();
User &Server::getUser(int fd);
void Server::incrUser(void);
void Server::decrUser(void);
