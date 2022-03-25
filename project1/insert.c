#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define LINE_SIZE 512

int main(int argc, char *argv[]){
    // 제대로 된 입력 아닐경우
    if(argc != 4){
        fprintf(stderr, "Please input correct oper\n");
        exit(1);
    }

    int fd;
    off_t fileSize;
    off_t offset = atoi(argv[1]);
    char buf[LINE_SIZE][BUFFER_SIZE];

    // 파일 open
    if((fd = open(argv[3], O_RDWR)) < 0){
        fprintf(stderr, "open error for %s\n", argv[3]);
        exit(1);        
    }
    
    lseek(fd, offset, SEEK_SET); // 오프셋 이동
    fileSize = lseek(fd, 0, SEEK_END);

    if(offset > fileSize){
		printf("offset is bigger than filesize\n");
		exit(1);        
    }

    int len, cnt = 0;
    while ((len = read(fd, buf[cnt], BUFFER_SIZE - 1)) > 0){
		if(len == BUFFER_SIZE - 1){
			buf[cnt][BUFFER_SIZE - 1] = '\0';
			cnt++;
		}
        else{
            buf[cnt][len] = '\0';
            cnt++;
        }

        if(cnt == LINE_SIZE){
            fprintf(stderr, "file size over\n");
            exit(1);
        }
    }

    lseek(fd, offset, SEEK_SET);
    write(fd, argv[2], strlen(argv[2]));

    lseek(fd, offset + strlen(argv[2]), SEEK_SET);

    for(int i = 0; i <= cnt; i++)
        write(fd, buf[i], strlen(buf[i]));
    
    close(fd);
}