#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
	char buf1[BUFFER_SIZE], buf2[BUFFER_SIZE], buf3[BUFFER_SIZE];
	off_t offset, fileSize;
	int deleteSize;
	int fd1, fd2;

    // 제대로 된 입력 아닐경우
    if(argc != 4){
        fprintf(stderr, "Please input correct oper\n");
        exit(1);
    }

    // 파일 open
	if((fd1 = open(argv[3], O_RDWR)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[3]);
		exit(1);
	}


	deleteSize = atoi(argv[2]);

    if(deleteSize == 0) exit(0); // 바이트 수 0이면 종료
    else if(deleteSize < 0){ // 바이트 수가 음수일 경우
        offset = atol(argv[1]) + (long)deleteSize; // 오프셋 위치 바이트 크기만큼 빼줌
        if(offset < 0){
            fprintf(stderr, "offset error\n");
            exit(1); 
        }
        deleteSize = abs(deleteSize);
    }
    else offset = atol(argv[1]);

	fileSize = lseek(fd1, 0, SEEK_END); // 파일 크기 계산

	if(offset + deleteSize > fileSize) { // 삭제할 바이트 수가 파일 사이즈보다 큰 경우
		printf("no remained data to delete\n");
		exit(1);
	}    

	if((lseek(fd1, 0, SEEK_SET)) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	read(fd1, buf1, offset); // 파일 인자 읽기
	read(fd1, buf2, deleteSize); // 삭제 바이트 읽기
	while((read(fd1, buf3, BUFFER_SIZE)) > 0) {
        // 파일 내용 buf3에 저장(백업)
	}

    // fd2 새로 생성
	if((fd2 = creat(argv[3], 0666)) < 0) {
		fprintf(stderr, "creat error for %s\n", argv[3]);
		exit(1);
	}

	write(fd2, buf1, offset); // fd2에 offset만큼 write
	write(fd2, buf3, fileSize - (offset + deleteSize)); // 백업해둔 data write

	exit(0);
}
