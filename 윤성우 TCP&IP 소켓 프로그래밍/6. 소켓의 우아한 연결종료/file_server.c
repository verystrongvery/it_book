#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define FUNCTION_CALL_ERROR -1
#define END_OF_LINE -1
#define BUFF_SIZE 1024

void init_socket_addr(struct sockaddr_in *server_addr, int port);
void error_handling(char *message);
int handle_request(int client_socket_fd, FILE *fp);
void connection_close(FILE *fp, int client_socket_fd, int server_socket_fd);

int main(int argc, char *argv[]){
    int server_socket_fd, client_socket_fd;
    FILE *fp;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;
    char buff[BUFF_SIZE];

	if(argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	fp = fopen("file_server.c", "rb");
	server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	init_socket_addr(&server_addr, atoi(argv[1]));

    if (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == FUNCTION_CALL_ERROR) {
        error_handling("bind() error");
    }

    if (listen(server_socket_fd, 5) == FUNCTION_CALL_ERROR) {
        error_handling("listen() error");
    }

    client_addr_size = sizeof(client_addr);
    client_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_size);

    while(handle_request(client_socket_fd, fp) != END_OF_LINE) {
    }

    shutdown(client_socket_fd, SHUT_WR);
    read(client_socket_fd, buff, BUFF_SIZE);
    printf("Message from client: %s \n", buff);
    connection_close(fp, client_socket_fd, server_socket_fd);
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

int handle_request(int client_socket_fd, FILE *fp) {
    int read_count;
    char buff[BUFF_SIZE];

    read_count = fread((void*)buff, 1, BUFF_SIZE, fp);
    write(client_socket_fd, buff, read_count);
    if (read_count < BUFF_SIZE) {
        return END_OF_LINE;
    }
    return 0;
}

void connection_close(FILE *fp, int client_socket_fd, int server_socket_fd) {
    fclose(fp);
    close(client_socket_fd);
    close(server_socket_fd);
}
