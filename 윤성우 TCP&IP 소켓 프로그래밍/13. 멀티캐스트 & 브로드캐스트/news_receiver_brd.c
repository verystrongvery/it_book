#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024
#define FUNCTION_CALL_ERROR -1

void init_socket_addr(struct sockaddr_in *server_addr, int port);
void error_handling(char *message);
void init_ip_mreq(struct ip_mreq *mreq, char *group_ip);

int main(int argc, char *argv[]) {
	struct sockaddr_in server_addr;

	char buff[BUFF_SIZE];
	int server_sock_fd, str_len;

	if(argc != 2){
		printf("Usage: %s <PORT>\n", argv[0]);
		exit(1);
	}

	server_sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
	init_socket_addr(&server_addr, atoi(argv[1]));

    if(bind(server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == FUNCTION_CALL_ERROR){
        error_handling("bind() error");
    }

	while((str_len = recvfrom(server_sock_fd, buff, BUFF_SIZE-1, 0, NULL, 0)) >= 0) {
		buff[str_len] = 0;
		fputs(buff, stdout);
	}

	close(server_sock_fd);
	return 0;
}

void init_socket_addr(struct sockaddr_in *server_addr, int port) {
	memset(server_addr, 0, sizeof(server_addr));
	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr->sin_port = htons(port);
}

void error_handling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void init_ip_mreq(struct ip_mreq *mreq, char *group_ip) {
	mreq->imr_multiaddr.s_addr = inet_addr(group_ip);
	mreq->imr_interface.s_addr = htonl(INADDR_ANY);
}