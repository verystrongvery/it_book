#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define FUNCTION_CALL_ERROR -1
#define BUFF_SIZE 1024
#define EXIT -1
#define REQUEST_CLOSE 0
#define EPOLL_SIZE 50

void init_socket_addr(struct sockaddr_in *server_addr, int port);
void error_handling(char *message);
void handle_connection(int server_sock_fd, int epoll_fd);
void echo_message(int client_sock_fd, int epoll_fd);
int handle_request(int server_sock_fd, struct epoll_event *ep_events, int epoll_fd);

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;
    struct epoll_event *ep_events, event;

    int server_sock_fd, epoll_fd;

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

	epoll_fd = epoll_create(EPOLL_SIZE);
	ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

	event.events = EPOLLIN;
	event.data.fd = server_sock_fd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock_fd, &event);

	while(handle_request(server_sock_fd, ep_events, epoll_fd) != EXIT) {
	}
	close(server_sock_fd);
    close(epoll_fd);
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

int handle_request(int server_sock_fd, struct epoll_event *ep_events, int epoll_fd) {
    int event_count;

	event_count = epoll_wait(epoll_fd, ep_events, EPOLL_SIZE, -1);
	if (event_count == FUNCTION_CALL_ERROR) {
		error_handling("epoll_wait() error");
		return EXIT;
	}

	for (int i = 0; i < event_count; i++) {
		if (ep_events[i].data.fd == server_sock_fd) {
			handle_connection(server_sock_fd, epoll_fd);
			continue;
		}
		echo_message(ep_events[i].data.fd, epoll_fd);
	}
	return 0;
}

void handle_connection(int server_sock_fd, int epoll_fd) {
	struct sockaddr_in client_addr;
	struct epoll_event event;

    socklen_t client_addr_len;
    int client_sock_fd;

    client_addr_len = sizeof(client_addr);
    client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    event.events = EPOLLIN;
    event.data.fd = client_sock_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock_fd, &event);
    printf("connected client: %d \n", client_sock_fd);
}

void echo_message(int client_sock_fd, int epoll_fd) {
	char buff[BUFF_SIZE];
	int str_len;

	str_len = read(client_sock_fd, buff, BUFF_SIZE);
	if (str_len != REQUEST_CLOSE) {
		write(client_sock_fd, buff, str_len);
		return ;
	}

	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sock_fd, NULL);
	close(client_sock_fd);
	printf("closed client: %d \n", client_sock_fd);
}