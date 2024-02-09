#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define FUNCTION_CALL_ERROR -1
#define BUFF_SIZE 1024

void error_handling(char *message);
void init_socket_addr(struct sockaddr_in *server_addr, char *ip, int port);
void read_routine(int client_socket_fd, char *buff);
void write_routine(int client_socket_fd, char *buff);

int main(int argc, char *argv[]) {
    int client_socket_fd;
    pid_t pid;
    struct sockaddr_in server_addr;
    char buff[BUFF_SIZE];

    if(argc != 3) {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);
        exit(1);
    }

    client_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(client_socket_fd == FUNCTION_CALL_ERROR) {
        error_handling("socket() error");
    }
    init_socket_addr(&server_addr, argv[1], atoi(argv[2]));

    if(connect(client_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == FUNCTION_CALL_ERROR) {
        error_handling("connect() error");
    }

    pid = fork();
    if(pid == 0) {
        write_routine(client_socket_fd, buff);
    }
    else {
        read_routine(client_socket_fd, buff);
    }

    close(client_socket_fd);
    return 0;
}

void error_handling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void init_socket_addr(struct sockaddr_in *server_addr, char *ip, int port) {
	memset(server_addr, 0, sizeof(server_addr));
	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = inet_addr(ip);
	server_addr->sin_port = htons(port);
}

void read_routine(int client_socket_fd, char *buff) {
    while(1) {
        int str_len = read(client_socket_fd, buff, BUFF_SIZE);
        if(str_len == 0) {
            return;
        }
        buff[str_len] = 0;
        printf("Message from server: %s", buff);
    }
}

void write_routine(int client_socket_fd, char *buff) {
    while(1) {
        fgets(buff, BUFF_SIZE, stdin);
        if(!strcmp(buff, "q\n") || !strcmp(buff, "Q\n")) {
            shutdown(client_socket_fd, SHUT_WR);
            return;
        }
        write(client_socket_fd, buff, strlen(buff));
    }
}