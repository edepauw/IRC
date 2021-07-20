#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include "server.hpp"

#define SERVER_PORT  12345

#define TRUE             1
#define FALSE            0

void pass(int fd, char *buff, Server &serv)
{
   serv.user[fd]._password = buff;

    std::cout << "PASS = " << serv.user[fd]._password << " for " << fd <<std::endl;
}
void nick(int fd, char *buff, Server &serv)
{
   serv.user[fd]._nick = buff;
    std::cout << "message NICK received from" << fd <<std::endl;
}
void user(int fd, char *buff, Server &serv)
{
    std::cout << "message USER received from" << fd <<std::endl;
}

void parseMsg(int fd ,char *buffer, Server &serv)
{
	void (*ptr[])( int fd, char* buff, Server &serv) =
    {
        pass,
        nick,
        user,
    };
    std::string        _name[] =
    {
        "PASS ",
        "NICK ",
        "USER "
    };
    for (int i = 0; i < 3; i++)
    {
        if (!strncmp(buffer, _name[i].c_str(), 5))
        {
            (*ptr[i])(fd, buffer + 5, serv);
			return;
        }
    }
    std::cout << "Connais pas ce message" << std::endl;
    return ;
}

int new_connection(int &listen_sd, fd_set &master_set, Server &serv)
{
   int new_sd;
   printf("  Listening socket is readable\n");
   new_sd = accept(listen_sd, NULL, NULL);
   if (new_sd < 0)
   {
      if (errno != EWOULDBLOCK)
         perror("  accept() failed");
      return -1;
   }
   printf("  New incoming connection - %d\n", new_sd);
   return(new_sd);
}



int main (int argc, char *argv[])
{
   Server serv;
   int    len, rc, on = 1;
   int    listen_sd, max_sd, new_sd;
   int    desc_ready, end_server = FALSE;
   fd_set               master_set, working_set;
   int    close_conn;
   char   buffer[80] = {0};
   listen_sd = init_server(atoi(argv[1]));
   
   do
   {
      memcpy(&working_set, &master_set, sizeof(master_set));
      printf("Waiting on select()...\n");
      if ((rc = select(max_sd + 1, &working_set, NULL, NULL, NULL)) <= 0)
      {
         perror("  select() failed");
         break;
      }
      desc_ready = rc;
      for (int i = 0; i <= max_sd  &&  desc_ready > 0; ++i)
      {
         if (FD_ISSET(i, &working_set)) // workingset[i] = I/O , qui est set via select() plus haut a 1 si le fd a eu de l'activite
         {
            desc_ready -= 1;
            if (i == listen_sd) // si i == listen_sd, activiter sur le fd du server, soit demande de connection
            {
               new_sd = new_connection(listen_sd, master_set, serv);
               FD_SET(new_sd, &master_set);
               serv.addUser(new_sd);
               if (new_sd > max_sd)
                  max_sd = new_sd;
            }
            else // si i != listen_sd, il ne s'agis pas du server mais d'un client_fd en activiter
            {
               printf("  Descriptor %d is readable\n", i);
               close_conn = FALSE;
               rc = recv(i, buffer, sizeof(buffer), 0);
               if (rc < 0)
               {
                  perror("  recv() failed");
                  break;
               }
               if (rc == 0)
               {
                  printf("  Connection closed\n");
                  serv.user.erase(i);
                  close_conn = TRUE;
                  break;
               }
               printf("  %d bytes received\n", rc);
               parseMsg(i ,buffer, serv);
               for(int i = 0; i < 80 ; i++)
                  buffer[i] = '\0';
               if (close_conn)
               {
                  close(i);
                  FD_CLR(i, &master_set);
                  if (i == max_sd)
                  {
                     while (FD_ISSET(max_sd, &master_set) == FALSE)
                        max_sd -= 1;
                  }
               }
            }
         }
      }

   } while (end_server == FALSE);

   /*************************************************************/
   /* Cleanup all of the sockets that are open                  */
   /*************************************************************/
   for (int i = 0; i <= max_sd; ++i)
   {
      if (FD_ISSET(i, &master_set))
         close(i);
   }
}
