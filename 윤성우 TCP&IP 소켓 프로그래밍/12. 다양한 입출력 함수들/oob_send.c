#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define FUNCTION_CALL_ERROR -1
#define BUFF_SIZE 1024

void init_socket_addr(struct sockaddr_in *server_addr, char *ip, int port);
void error_handling(char *message);

int main(int argc, char *argv[]) {
	struct sockaddr_in server_addr;

	int client_sock_fd;

	if (argc != 3) {
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}

    client_sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(client_sock_fd == FUNCTION_CALL_ERROR) {
        error_handling("socket() error");
    }
    init_socket_addr(&server_addr, argv[1], atoi(argv[2]));

	if(connect(client_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == FUNCTION_CALL_ERROR) {
		error_handling("connect() error!");
	}

	write(client_sock_fd, "123", strlen("123"));
	send(client_sock_fd, "4", strlen("4"), MSG_OOB);
	write(client_sock_fd, "567", strlen("567"));
	send(client_sock_fd, "890", strlen("890"), MSG_OOB);
	close(client_sock_fd);
	return 0;
}

void init_socket_addr(struct sockaddr_in *server_addr, char *ip, int port) {
	memset(server_addr, 0, sizeof(server_addr));
	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = inet_addr(ip);
	server_addr->sin_port = htons(port);
}

void error_handling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}