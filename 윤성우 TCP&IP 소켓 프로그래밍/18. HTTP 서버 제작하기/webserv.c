#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFF_SIZE 1024
#define SMALL_BUFF_SIZE 100

int initServerSocket(struct sockaddr_in* serverAddress, char* port);
void* requestHandler(void *clientSocketDescriptor);
void readRequest(FILE* clientInputStream, char* fileName);
void sendResponse(FILE* clientOutputStream, char* fileName);

int main(int argc, char *argv[]) {
	if(argc!=2) {
		printf("How To Use: %s [Port]\n", argv[0]);
		exit(1);
    }
	// sockaddr_in: ip, port 같은 주소 정보를 가지는 구조체
	// sockaddr_in.sin_family: 사용하는 주소 체계
	// sockaddr_in.sin_addr.s_addr: IP 주소
	// sockaddr_in.sin_port: 포트 번호
	struct sockaddr_in serverAddress, clientAddress;
 	int serverSocketDescriptor = initServerSocket(&serverAddress, argv[1]); // 서버 소켓 자원 할당 및 초기화
 	int clientSocketDescriptor;
	while(1) {
		int clientAddressSize = sizeof(clientAddress);
		clientSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr*)&clientAddress, &clientAddressSize);  // 서버에서 클라이언트의 연결 요청을 수락 및 클라이언트 소켓의 자원 할당
		pthread_t threadId;
		pthread_create(&threadId, NULL, requestHandler, &clientSocketDescriptor); // 정적 컨텐츠(HTML)를 클라이언트에게 전달하는 역할을 하는 쓰레드 생성
        pthread_detach(threadId); // pthread_create로 생성된 쓰레드 해제
	}
    close(serverSocketDescriptor); // 서버 소켓 자원 할당 해제
    close(clientSocketDescriptor); // 클라이언트 소켓 자원 할당 해제
    return 0;
}

int initServerSocket(struct sockaddr_in* serverAddress, char* port) {
	// PF_INET: IPv4 프로토콜 사용
	// SOCK_STREAM: TCP 소켓 사용
	int serverSocketDescriptor = socket(PF_INET, SOCK_STREAM, 0);
   	memset(serverAddress, 0, sizeof(serverAddress)); // 서버 주소에 대한 구조체 초기화
	serverAddress->sin_family = AF_INET; // AF_INET: IPv4 프로토콜 사용
	// htonl: 데이터를 네트워크 바이트 순서로 변환(32비트 정수)
	// INADDR_ANY: 서버의 IP 주소
	serverAddress->sin_addr.s_addr = htonl(INADDR_ANY);
	// htons: 데이터를 네트워크 바이트 순서로 변환(16비트 정수)
	// atoi: 문자열을 정수로 변환
	serverAddress->sin_port = htons(atoi(port));
	if(bind(serverSocketDescriptor, (struct sockaddr*)serverAddress, sizeof(*serverAddress)) == -1) { // 서버 소켓에 IP 주소와, 포트 번호 할당
		printf("bind() Fail");
		exit(1);
	}
	if(listen(serverSocketDescriptor, 20) == -1) { // 서버 소켓을 연결 대기 상태로 설정
		printf("listen() Fail");
		exit(1);
	}
	return serverSocketDescriptor;
}

void* requestHandler(void *clientSocketDescriptor) {
	int clientSocket = *((int *)clientSocketDescriptor);
	char fileName[SMALL_BUFF_SIZE];
	FILE* clientInputStream = fdopen(clientSocket, "r"); // 클라이언트 소켓에서 클라이언트의 요청을 받을 스트림 열기
	FILE* clientOutputStream = fdopen(dup(clientSocket), "w"); // 클라이언트 소켓에서 클라이언트의 응답을 보낼 스트림 열기
	readRequest(clientInputStream, fileName); // 요청 에서 파일이름 읽기
	sendResponse(clientOutputStream, fileName); // 파일을 읽고 클라이언트에게 보내기
}

void readRequest(FILE* clientInputStream, char* fileName) {
	char requestMessage[SMALL_BUFF_SIZE];
	fgets(requestMessage, SMALL_BUFF_SIZE, clientInputStream); // 클라이언트의 요청 메시지 읽기
	// 요청 메시지의 Start Line에서 파일 이름 읽기
	// Start Line 구조: [HTTP Method] [Request target] [HTTP Version]
	char httpMethod[SMALL_BUFF_SIZE];
	strcpy(httpMethod, strtok(requestMessage, " /"));
	strcpy(fileName, strtok(NULL, " /"));
	fclose(clientInputStream); // 클라이언트의 요청에 대한 스트림 닫기
}

void sendResponse(FILE* clientOutputStream, char* fileName) {
	FILE* file = fopen(fileName, "r"); // 클라이언트에게 보여줄 파일 읽기
	// 응답 메시지의 Status Line 작성
	// Status Line 구조: [HTTP Version] [Status Code] [Status Text]
	fputs("HTTP/1.0 200 OK\r\n\r\n", clientOutputStream);
	char buff[BUFF_SIZE];
	while(fgets(buff, BUFF_SIZE, file) != NULL){
		fputs(buff, clientOutputStream); // 파일을 클라이언트에게 전송
		fflush(clientOutputStream); // 출력 버퍼 비우기
   }
}