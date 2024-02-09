#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define BUFF_SIZE 1024
#define FUNCTION_CALL_ERROR -1
#define DISCONNECTED -1

void init_sigaction(struct sigaction *act);
void read_child_proc(int sig);
void init_socket_addr(struct sockaddr_in *server_addr, int port);
void error_handling(char *message);
int handle_request(struct sockaddr_in *client_addr, int server_sockfd);

int main(int argc, char *argv[]){
    int server_sockfd;
    struct sockaddr_in server_addr, client_addr;
    struct sigaction act;

    if(argc != 2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    init_sigaction(&act);

    server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
    init_socket_addr(&server_addr, atoi(argv[1]));

    if(bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == FUNCTION_CALL_ERROR){
        error_handling("bind() error");
    }

    if(listen(server_sockfd, 5) == FUNCTION_CALL_ERROR){
        error_handling("listen() error");
    }

    while(handle_request(&client_addr, server_sockfd) != DISCONNECTED){
    }
    puts("Server is closed");
    close(server_sockfd);
    return 0;
}

void init_sigaction(struct sigaction *act){
    act->sa_handler = read_child_proc;
    sigemptyset(&act->sa_mask);
    act->sa_flags = 0;
    sigaction(SIGCHLD, act, 0);
}

void read_child_proc(int sig){
    pid_t pid;
    int status;

    pid = waitpid(-1, &status, WNOHANG);
    printf("remove proc id : %d\n", pid);
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

int handle_request(struct sockaddr_in *client_addr, int server_sockfd){
    socklen_t addr_size = sizeof(*client_addr);
    int client_sockfd, str_len;
    pid_t pid;
    char buff[BUFF_SIZE];

    client_sockfd = accept(server_sockfd, (struct sockaddr*)client_addr, &addr_size);
    if(client_sockfd == FUNCTION_CALL_ERROR){
        return 0;
    }
    puts("new client connected...");

    pid = fork();
    if (pid == FUNCTION_CALL_ERROR) {
        close(client_sockfd);
        return 0;
    }

    if (pid != 0) {
        close(client_sockfd);
        return 0;
    }

    while ((str_len = read(client_sockfd, buff, BUFF_SIZE)) != 0) {
        write(client_sockfd, buff, str_len);
    }
    close(client_sockfd);
    puts("client disconnected...");
    return DISCONNECTED;
}

