#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024
#define FUNCTION_CALL_ERROR -1
#define DISCONNECTED -1
#define CHILD_PROCESS 0

void init_sigaction(struct sigaction *sig_act);
void read_child_proc(int sig);
void init_socket_addr(struct sockaddr_in *server_addr, int port);
void error_handling(char *message);
void store_str(int pipe_fd);
int handle_request(int server_sock_fd, int pipe_input_fd);
void read_request(int client_sock_fd, int pipe_input_fd);

int main(int argc, char *argv[]) {

    struct sockaddr_in server_addr;
    struct sigaction sig_act;

    pid_t pid;
    int server_sock_fd, sig_state;
    int pipe_fd[2];

    if(argc != 2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    init_sigaction(&sig_act);

    server_sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    init_socket_addr(&server_addr, atoi(argv[1]));

    if(bind(server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == FUNCTION_CALL_ERROR){
        error_handling("bind() error");
    }

    if(listen(server_sock_fd, 5) == FUNCTION_CALL_ERROR){
        error_handling("listen() error");
    }

    pipe(pipe_fd);
    pid = fork();
    if (pid == CHILD_PROCESS) {
        store_str(pipe_fd[0]);
        return 0;
    }

    while(handle_request(server_sock_fd, pipe_fd[1]) != DISCONNECTED) {
    }

    close(server_sock_fd);
    return 0;
}

void init_sigaction(struct sigaction *sig_act){
    sig_act->sa_handler = read_child_proc;
    sigemptyset(&sig_act->sa_mask);
    sig_act->sa_flags = 0;
    sigaction(SIGCHLD, sig_act, 0);
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

void store_str(int pipe_output_fd) {
    FILE *fp;
    char buff[BUFF_SIZE];
    int str_len;

    fp = fopen("msg.txt", "wt");
    for(int i = 0; i < 10; i++) {
        str_len = read(pipe_output_fd, buff, BUFF_SIZE);
        fwrite((void*)buff, 1, str_len, fp);
    }
    fclose(fp);
}

int handle_request(int server_sock_fd, int pipe_input_fd) {
    struct sockaddr_in client_addr;

    pid_t pid;
    int client_addr_size, client_sock_fd;

    client_addr_size = sizeof(client_addr);
    client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&client_addr, &client_addr_size);
    if(client_sock_fd == FUNCTION_CALL_ERROR) {
        return 0;
    }
    puts("new client connected...\n");

    pid = fork();
    if (pid == CHILD_PROCESS) {
        close(server_sock_fd);
        read_request(client_sock_fd, pipe_input_fd);
        return DISCONNECTED;
    }
    close(client_sock_fd);
}

void read_request(int client_sock_fd, int pipe_input_fd) {
    int str_len;
    char buff[BUFF_SIZE];

    while((str_len = read(client_sock_fd, buff, BUFF_SIZE)) != 0) {
        write(client_sock_fd, buff, str_len);
        write(pipe_input_fd, buff, str_len);
    }
    close(client_sock_fd);
    puts("client disconnected...");
}
