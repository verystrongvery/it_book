#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

void init_socket_addr(struct sockaddr_in *server_addr, int port);

int main(int argc, char *argv[]){
    int socket_fd, read_count;
    struct sockaddr_in server_addr;
    char buff[BUFF_SIZE];
    FILE *fp;

    if(argc != 3){
        printf("Usage : %s <IP> <PORT>\n", argv[0]);
        exit(1);
    }

    fp = fopen("receive.dat", "wb");
    socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    init_socket_addr(&server_addr, atoi(argv[2]));

    connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    while((read_count = read(socket_fd, buff, BUFF_SIZE)) != 0){
        fwrite(buff, 1, read_count, fp);
    }

    puts("Received file data");
    write(socket_fd, "Thank you", 10);
    fclose(fp);
    close(socket_fd);
    return 0;
}

void init_socket_addr(struct sockaddr_in *server_addr, int port) {
	memset(server_addr, 0, sizeof(server_addr));
	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr->sin_port = htons(port);
}