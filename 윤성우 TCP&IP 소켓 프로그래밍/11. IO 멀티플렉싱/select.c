#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define FUNCTION_CALL_ERROR -1
#define BUFF_SIZE 1024
#define TIME_OUT 0
#define EXIT -1

void init_fd_set(fd_set *fs);
void init_timeval(struct timeval *timeout);
void error_handling(char *message);
void detect_select();
int handle_select(fd_set *fs);

int main(int argc, char *argv[]) {
    fd_set fs;

    init_fd_set(&fs);

    while(handle_select(&fs) != EXIT) {
    }
    return 0;
}

void init_fd_set(fd_set *fs) {
    FD_ZERO(fs);
    FD_SET(0, fs);
}

int handle_select(fd_set *fs) {
    struct timeval timeout;

    fd_set temp;
    int select_result;

    init_timeval(&timeout);
    temp = *fs;

    select_result = select(1, &temp, NULL, NULL, &timeout);
    if (select_result == FUNCTION_CALL_ERROR) {
        error_handling("select() error");
    }

    if (select_result == TIME_OUT) {
        puts("Time-out!");
        return 0;
    }

    if (FD_ISSET(0, &temp)) {
        detect_select();
        return 0;
    }

    return EXIT;
}

void init_timeval(struct timeval *timeout) {
    timeout->tv_sec = 5;
    timeout->tv_usec = 0;
}

void error_handling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void detect_select() {
    char buff[BUFF_SIZE];
    int str_len;

    str_len = read(0, buff, BUFF_SIZE);
    buff[str_len] = 0;
    printf("message from console: %s", buff);
}