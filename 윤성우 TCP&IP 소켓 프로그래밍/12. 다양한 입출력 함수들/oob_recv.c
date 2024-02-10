#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define BUFF_SIZE 1024
#define FUNCTION_CALL_ERROR -1
#define REQUEST_CLOSE 0

void init_socket_addr(struct sockaddr_in *server_addr, int port);
void init_sigaction(struct sigaction *act, int client_sock_fd);
void error_handling(char *message);
void urg_handler(int signo);

int server_sock_fd;
int client_sock_fd;

int main(int argc, char *argv[]) {
	struct sockaddr_in server_addr;
	struct sigaction sig_act;

	socklen_t server_addr_size;
	char buff[BUFF_SIZE];
	int str_len;

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

	init_sigaction(&sig_act, client_sock_fd);

	while((str_len = recv(client_sock_fd, buff, BUFF_SIZE, 0)) != REQUEST_CLOSE){
		if(str_len == FUNCTION_CALL_ERROR){
			continue;
		}
		buff[str_len] = 0;
		puts(buff);
	}

	close(client_sock_fd);
	close(server_sock_fd);
	return 0;
}

void urg_handler(int signo){
	int str_len;
	char buff[BUFF_SIZE];

	str_len = recv(client_sock_fd, buff, BUFF_SIZE - 1, MSG_OOB);
	buff[str_len] = 0;
	printf("Urgent message: %s\n", buff);
}

void init_socket_addr(struct sockaddr_in *server_addr, int port) {
	memset(server_addr, 0, sizeof(server_addr));
	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr->sin_port = htons(port);
}

void init_sigaction(struct sigaction *act, int client_sock_fd){
    act->sa_handler = urg_handler;
    sigemptyset(&act->sa_mask);
    act->sa_flags = 0;
    fcntl(client_sock_fd, F_SETOWN, getpid());
    sigaction(SIGURG, act, 0);
}

void error_handling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}


