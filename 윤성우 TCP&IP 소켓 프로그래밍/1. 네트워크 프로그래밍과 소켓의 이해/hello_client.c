#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define FUNCTION_CALL_ERROR -1

void error_handling(char *message);
void init_socket_addr(struct sockaddr_in *server_addr, char *ip, int port);

int main(int argc, char *argv[]) {
	int client_socket;
	struct sockaddr_in server_addr;
	char message[32];

	if(argc != 3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	// PF_INET : IPv4 주소 체계 사용
	// SOCK_STREAM : TCP 프로토콜 사용
	// 0: 첫번째 두번째 매개변수를 기준으로 프로토콜 자동 선택
	// socket(): 소켓 생성
	client_socket = socket(PF_INET, SOCK_STREAM, 0);
	if(client_socket == -1) {
		error_handling("socket() error");
	}
	init_socket_addr(&server_addr, argv[1], atoi(argv[2]));

	// connect(): 서버에 연결 요청
	if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == FUNCTION_CALL_ERROR) {
		error_handling("connect() error");
	}

	// read(): 서버로부터 데이터 수신
	if(read(client_socket, message, sizeof(message)) == FUNCTION_CALL_ERROR) {
		error_handling("read() error");
	}

	printf("Message from server: %s\n", message);
	close(client_socket);
	return 0;
}

void error_handling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void init_socket_addr(struct sockaddr_in *server_addr, char *ip, int port) {
	memset(server_addr, 0, sizeof(server_addr));
	server_addr->sin_family = AF_INET; // IPv4 주소 체계 사용
	server_addr->sin_addr.s_addr = inet_addr(ip); // inet_addr(): 문자열 IP 주소를 정수형 형태인 네트워크 바이트 순서로 변환
	server_addr->sin_port = htons(port); // htons(): 호스트 바이트 순서를 네트워크 바이트 순서로 변환
}
