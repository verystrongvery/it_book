#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFF_SIZE 1024
#define FUNCTION_CALL_ERROR -1

void init_socket_addr(struct sockaddr_in *server_addr, int port);
void error_handling(char *message);

int main(int argc, char *argv[]) {
	struct sockaddr_in server_addr;
	struct sigaction sig_act;

	socklen_t server_addr_size;
	char buff[BUFF_SIZE];
	int server_sock_fd, client_sock_fd, str_len;

	if(argc != 2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	server_sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	init_socket_addr(&server_addr, atoi(argv[1]));

    if(bind(server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == FUNCTION_CALL_ERROR){
        error_handling("bind() error");
    }

    if(listen(server_sock_fd, 5) == FUNCTION_CALL_ERROR){
        error_handling("listen() error");
    }

	server_addr_size = sizeof(server_addr);
	client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&server_addr, &server_addr_size);


	while((str_len = recv(client_sock_fd, buff, BUFF_SIZE, MSG_PEEK|MSG_DONTWAIT)) < 0){
	}

	buff[str_len] = 0;
	printf("Buffering %d bytes: %s\n", str_len, buff);

	str_len = recv(client_sock_fd, buff, BUFF_SIZE, 0);
	buff[str_len] = 0;
	printf("Read again: %s\n", buff);
	close(client_sock_fd);
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

