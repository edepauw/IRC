#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>

int main(int ac, char **av)
{
	char buffer[256];
	int i = 0;
	int socketServer = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addrServer;
	addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(atoi(av[1]));
	fd_set read_fd_set;
	FD_ZERO(&read_fd_set);
	FD_SET((unsigned int)socketServer, &read_fd_set);
struct timeval timeout;
timeout.tv_sec = 2;
timeout.tv_usec = 0;
	bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
	std::cout << "bind: " << socketServer << std::endl;

	listen(socketServer, 10);
	std::cout << "listening...." << std::endl;

	struct sockaddr_in addrClient;
	socklen_t cSize = sizeof(addrClient);
	int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &cSize);
	std::cout << "accept" << std::endl;
	while (1){
		select(socketServer+1, &read_fd_set, NULL, NULL, &timeout);
		i = recv(socketClient, &buffer, sizeof(buffer), 0);
		std::cout << i << "buffer: " << buffer << std::endl;
		if(!strncmp(buffer, "EXIT", 4))
			break;
	}
	close(socketClient);
	close(socketServer);
	return 0;
}
