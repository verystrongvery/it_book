#include<stdio.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define FUNCTION_CALL_ERROR -1

void error_handling(char *message);
void init_socket_addr(struct sockaddr_in *server_addr, int port);

int main(int argc, char *argv[]) {
	int server_socket_fd;
	int client_socket_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char message[] = "Hello World!";
	socklen_t client_addr_size;

	if(argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	// PF_INET: IPv4 주소 체계 사용
	// SOCK_STREAM: TCP 프로토콜 사용
	// 0: 첫번째 두번째 매개변수를 기준으로 프로토콜 자동 선택
	// socket(): 소켓 생성
	server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(server_socket_fd == FUNCTION_CALL_ERROR) {
		error_handling("socket() error");
	}
	init_socket_addr(&server_addr, atoi(argv[1]));

	// bind(): IP 주소와 PORT 번호 할당
	if (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == FUNCTION_CALL_ERROR) {
		error_handling("bind() error");
	}

	// listen(): 연결 요청 대기 상태로 진입
	if (listen(server_socket_fd, 5) == FUNCTION_CALL_ERROR) {
		error_handling("listen() error");
	}

	client_addr_size = sizeof(client_addr);
	// accept(): 연결 요청 수락
	client_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_size);
	if (client_socket_fd == FUNCTION_CALL_ERROR) {
		error_handling("accept() error");
	}

	// write(): 데이터 전송
	write(client_socket_fd, message, sizeof(message));

	close(client_socket_fd);
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
	server_addr->sin_family = AF_INET; // IPv4 주소 체계 사용
	// htonl(): 호스트 바이트 순서를 네트워크 바이트 순서로 변환
	// INADDR_ANY: 서버의 IP 주소를 자동으로 찾아서 대입
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr->sin_port = htons(port); // htons(): 호스트 바이트 순서를 네트워크 바이트 순서로 변환
}
