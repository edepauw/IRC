#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
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

#define DATA_BUFFER 512
#define MAX_CONNECTIONS 1024
using namespace std;
int create_tcp_server_socket(){
    struct sockaddr_in saddr;
    int fd, ret_val;

   //Step1: create a TCP socket
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }
    printf("Created a socket with fd: %d\n", fd);

    // Initialize the socket address structure
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(7003);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Step2: bind the socket to port 7000 on the local host */
    ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val != 0) {
        fprintf(stderr, "bind failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }

    // Step3: listen for incoming connections */
    ret_val = listen(fd, 10);
    if (ret_val != 0) {
        fprintf(stderr, "listen failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }
    fcntl(fd, F_SETFL, O_NONBLOCK);
    return fd;
}


int main () {
	fd_set read_fd_set, write_fd_set;
	struct sockaddr_in new_addr;
	int server_fd, new_fd, ret_val, i;
	socklen_t addrlen;
	char buf[DATA_BUFFER];
	int all_connections[MAX_CONNECTIONS];
	std::string fullBuffer;
	/* Get the socket server fd */
	server_fd = create_tcp_server_socket();
	if (server_fd == -1) {
		fprintf(stderr, "Failed to create a server\n");
		return -1;
	}
	for (i=0;i < MAX_CONNECTIONS;i++) {
		all_connections[i] = -1;
	}
	all_connections[0] = server_fd;
	int nb_fds = 1;
	while (1) {
		FD_ZERO(&read_fd_set);
		/* Set the fd_set before passing it to the select call */
		int max_fd = 0;
		for (i=0;i < nb_fds ;i++) {
			if (all_connections[i] >= 0) {
				FD_SET(all_connections[i], &read_fd_set);
				if(all_connections[i] > max_fd) max_fd = all_connections[i];
			}
		}
		write_fd_set = read_fd_set;
		/* Invoke select() and then wait! */
		printf("-----------------------------------------------------------");
		printf("\nUsing select() to listen for incoming events\n");
		std::cout << max_fd << std::endl;
		ret_val = select(max_fd + 1, &read_fd_set, NULL, NULL, NULL);

		/* select() woke up. Identify the fd that has events */
		while (ret_val > 0 ) {
			printf("Select returned with %d\n", ret_val);
			/* Check if the fd with event is the server fd */
			if (FD_ISSET(server_fd, &read_fd_set)) {
				/* accept the new connection */
				printf("Returned fd is %d (server's fd)\n", server_fd);
				new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
				if (new_fd >= 0) {
					printf("Accepted a new connection with fd: %d\n", new_fd);
					all_connections[nb_fds] = new_fd;
					nb_fds++;
				} else {
					fprintf(stderr, "accept failed [%s]\n", strerror(errno));
				}
				ret_val--;
		}
		else{
			/* Check if the fd with event is a non-server fd */
			for (i=1;i < MAX_CONNECTIONS;i++) {
				if ((all_connections[i] > 0) &&
					(FD_ISSET(all_connections[i], &read_fd_set))) {
					/* read incoming data */
					printf("Returned fd is %d [index, i: %d]\n", all_connections[i], i);
					fullBuffer.clear();
						//if (fullBuffer.find("\r\n\r\n") != std::string::npos)
						//	break;
						ret_val = recv(all_connections[i], buf, DATA_BUFFER, 0);//changer ret_val
						if (ret_val == 0) {
							printf("Closing connection for fd:%d\n", all_connections[i]);
							close(all_connections[i]);
							all_connections[i] = -1; /* Connection is now closed */
						}
						if (ret_val == -1) {
							printf("recv() failed for fd: %d [%s], but buf = %s, ret_val = %d\n",
								all_connections[i], strerror(errno), buf, ret_val);
							break;
						}
						if (ret_val > 0) {
							fullBuffer += buf;
						}
						cout << "ret_val = " << ret_val << endl;
					std::cout << buf << std::endl;
				}
				ret_val--;
				if (!ret_val) continue;
			}
			ret_val--;
			} /* for-loop */
		} /* (ret_val >= 0) */
	} /* while(1) */

	/* Last step: Close all the sockets */
	for (i=0;i < MAX_CONNECTIONS;i++) {
		if (all_connections[i] > 0) {
			close(all_connections[i]);
		}
	}
	return 0;
 }

