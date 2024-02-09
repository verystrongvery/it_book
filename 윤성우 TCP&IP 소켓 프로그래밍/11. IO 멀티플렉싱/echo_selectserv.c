#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define FUNCTION_CALL_ERROR -1
#define BUFF_SIZE 1024
#define TIME_OUT 0
#define EXIT -1
#define REQUEST_CLOSE 0

void init_socket_addr(struct sockaddr_in *server_addr, int port);
void error_handling(char *message);
void init_fd_set(fd_set *fs, int sock_fd);
void init_timeval(struct timeval *timeout);
void handle_connection(fd_set *fs, int server_sock_fd, struct sockaddr_in client_addr, int *fd_max);
void echo_message(int client_sock_fd, fd_set *fs);
int handle_request(fd_set *fs, int *fd_max, int server_sock_fd);

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;

    fd_set fs;
    int server_sock_fd, fd_max;

    if(argc != 2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    server_sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    init_socket_addr(&server_addr, atoi(argv[1]));

    if(bind(server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        error_handling("bind() error");
    }

    if(listen(server_sock_fd, 5) == FUNCTION_CALL_ERROR){
        error_handling("listen() error");
    }

    init_fd_set(&fs, server_sock_fd);
    fd_max = server_sock_fd;

    while(handle_request(&fs, &fd_max, server_sock_fd) != EXIT) {
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

void init_fd_set(fd_set *fs, int sock_fd) {
    FD_ZERO(fs);
    FD_SET(sock_fd, fs);
}

int handle_request(fd_set *fs, int *fd_max, int server_sock_fd) {
    struct timeval timeout;
    struct sockaddr_in client_addr;

    fd_set fs_copy;
    int select_result;

    init_timeval(&timeout);
    fs_copy = *fs;

    select_result = select(*fd_max + 1, &fs_copy, NULL, NULL, &timeout);
    if (select_result == FUNCTION_CALL_ERROR) {
    	return EXIT;
    }

    if (select_result == TIME_OUT) {
       puts("Time-out!");
       return 0;
    }

    for (int i = 0; i < *fd_max + 1; i++) {
        if (!FD_ISSET(i, &fs_copy)) {
            continue;
        }
        if (i == server_sock_fd) {
            handle_connection(fs, server_sock_fd, client_addr, fd_max);
            continue;
        }
        echo_message(i, fs);
    }
    return 0;
}

void init_timeval(struct timeval *timeout) {
    timeout->tv_sec = 5;
    timeout->tv_usec = 0;
}

void handle_connection(fd_set *fs, int server_sock_fd, struct sockaddr_in client_addr, int *fd_max) {
    socklen_t client_addr_len;
    int client_sock_fd;

    client_addr_len = sizeof(client_addr);
    client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&client_addr, &client_addr_len);

    FD_SET(client_sock_fd, fs);
    if (client_sock_fd > *fd_max) {
        *fd_max = client_sock_fd;
    }
    printf("connected client: %d \n", client_sock_fd);
}

void echo_message(int client_sock_fd, fd_set *fs) {
    int str_len;
    char buff[BUFF_SIZE];

    str_len = read(client_sock_fd, buff, BUFF_SIZE);
    if (str_len != REQUEST_CLOSE) {
        write(client_sock_fd, buff, str_len);
        return ;
    }

    FD_CLR(client_sock_fd, fs);
    close(client_sock_fd);
    printf("closed client: %d \n", client_sock_fd);
}