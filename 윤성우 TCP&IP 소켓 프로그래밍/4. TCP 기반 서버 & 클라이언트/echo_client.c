#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define FUNCTION_CALL_ERROR -1
#define BUFF_SIZE 1024
#define QUIT -1

void error_handling(char *message);
void init_socket_addr(struct sockaddr_in *server_addr, char *ip, int port);
int request(int client_socket_fd);
int read_request(int client_socket_fd, char* message, int recv_len);

int main(int argc, char* argv[]) {
    int client_socket_fd;
    struct sockaddr_in server_addr;

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
    else {
        puts("Connected...........");
    }

    while(request(client_socket_fd) != QUIT) {
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

int request(int client_socket_fd) {
    char message[BUFF_SIZE];
    int str_len;
    int recv_len = 0;

    fputs("Input message(Q to quit): ", stdout);
    fgets(message, BUFF_SIZE, stdin);
    if(!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
        return QUIT;
    }
    str_len = write(client_socket_fd, message, strlen(message));
    while(recv_len < str_len) {
        recv_len = read_request(client_socket_fd, message, recv_len);
    }
    message[str_len] = 0;
    printf("Message from server: %s", message);
    return 0;
}

int read_request(int client_socket_fd, char* message, int recv_len) {
    int recv_cnt = read(client_socket_fd, &message[recv_len], BUFF_SIZE - 1);
    if (recv_cnt == FUNCTION_CALL_ERROR) {
        error_handling("read() error");
    }
    return recv_len + recv_cnt;
}