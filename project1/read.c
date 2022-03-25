#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){

	char buf[BUFFER_SIZE];
	int fd;
	int len;
	
    // 제대로 된 입력 아닐경우
    if(argc != 4){
        fprintf(stderr, "Please input correct oper\n");
        exit(1);
    }

	if((fd = open(argv[3], O_RDONLY)) < 0) { // 파일명 읽기전용으로 open
		fprintf(stderr, "open error for %s\n", argv[3]);
		exit(1);
	}

	off_t bytes = atoi(argv[2]);
    off_t offset, fileSize;

    // 바이트 0이면 종료
    if(bytes == 0) exit(0);
    else if(bytes < 0){ // 바이트 수가 음수일 경우
        offset = atol(argv[1]) + (long)bytes; // 오프셋 위치 바이트 크기만큼 빼줌
        if(offset < 0){
            fprintf(stderr, "offset error\n");
            exit(1); 
        }
        bytes = abs(bytes);
    }
    else offset = atol(argv[1]);

	fileSize = lseek(fd, (off_t)0, SEEK_END); // 파일 크기 저장

	if((lseek(fd, offset, SEEK_SET)) < 0) { //오프셋의 위치를 인자로 받은 오프셋으로 변경
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	len = read(fd, buf, bytes); //fd가 의미하는 파일에서 bytes만큼을 읽어 buf에 저장
	write(1, buf, len); // buf에서 stdout

	if(offset > fileSize) { //인자로 받은 오프셋이 파일 크기보다 큰 경우
		printf("No data to read\n");
		exit(1);
	}

	exit(0);
}
