#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define FUNCTION_CALL_ERROR -1
#define BUFF_SIZE 1024

void init_socket_addr(struct sockaddr_in *server_addr, char *ip, int port);
void error_handling(char *message);

int main(int argc, char *argv[]) {
	struct sockaddr_in server_addr;

	FILE *fp;
	char buff[BUFF_SIZE];
	int client_sock_fd, sock_broadcast_opt;

	if (argc != 3) {
		printf("Usage: %s <BROADCAST IP> <PORT>\n", argv[0]);
		exit(1);
	}

    client_sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(client_sock_fd == FUNCTION_CALL_ERROR) {
        error_handling("socket() error");
    }
    init_socket_addr(&server_addr, argv[1], atoi(argv[2]));

	setsockopt(client_sock_fd, SOL_SOCKET, SO_BROADCAST, (void*)&sock_broadcast_opt, sizeof(sock_broadcast_opt));

    if((fp = fopen("news_sender_brd.c", "r")) == NULL) {
		error_handling("fopen() error");
	}

    while(fgets(buff, BUFF_SIZE, fp) != NULL) {
		sendto(client_sock_fd, buff, strlen(buff), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
		sleep(2);
	}

	fclose(fp);
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