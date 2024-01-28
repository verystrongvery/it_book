#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define FUNCTION_CALL_ERROR -1
#define BUFF_SIZE 1024

void error_handling(char *message);
void init_socket_addr(struct sockaddr_in *server_addr, int port);
void handle_request(int server_socket_fd);

int main(int argc, char* argv[]) {
	int server_socket_fd;
	struct sockaddr_in server_addr;

	if(argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(server_socket_fd == FUNCTION_CALL_ERROR) {
        error_handling("socket() error");
    }
    init_socket_addr(&server_addr, atoi(argv[1]));

    if (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == FUNCTION_CALL_ERROR) {
        error_handling("bind() error");
    }

    if (listen(server_socket_fd, 5) == FUNCTION_CALL_ERROR) {
        error_handling("listen() error");
    }

    for(int i = 0; i < 5; i++) {
        handle_request(server_socket_fd);
    }
    close(server_socket_fd);
    return 0;
}

void error_handling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void init_socket_addr(struct sockaddr_in *server_addr, int port) {
	memset(server_addr, 0, sizeof(server_addr));
	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr->sin_port = htons(port);
}

void handle_request(int server_socket_fd) {
    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);
    int client_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_size);
    int str_len = 0;
    char message[BUFF_SIZE];

    if (client_socket_fd == FUNCTION_CALL_ERROR) {
        error_handling("accept() error");
    }
    else {
        printf("Connected client %d \n", client_socket_fd);
    }

    while((str_len = read(client_socket_fd, message, BUFF_SIZE)) != 0) {
        write(client_socket_fd, message, str_len);
    }
    close(client_socket_fd);
}