#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 10
#define MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char *argv[]){
    char buf[BUFFER_SIZE];
    int fd, cpFd; // 원본, 복사본
    int len;

    // 제대로 된 입력 아닐경우
    if(argc != 3){
        fprintf(stderr, "Please input correct oper\n");
        exit(1);
    }

    // 원본 파일 open
    if((fd = open(argv[1], O_RDONLY)) < 0){
        fprintf(stderr, "open error for %s\n", argv[1]);
        exit(1);        
    }

    //  복사본 파일 open
    if((cpFd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, MODE)) < 0){
        fprintf(stderr, "open error for %s\n", argv[1]);
        exit(1);        
    }

    // 원본 파일 다 읽을 때까지
	while((len = read(fd, buf, BUFFER_SIZE)) > 0) // 원본파일 데이터 buf에 저장
		write(cpFd, buf, len); // len만큼 복사본 파일에 write

    exit(0);
}