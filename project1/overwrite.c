#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]){
    // 제대로 된 입력 아닐경우
    if(argc != 4){
        fprintf(stderr, "Please input correct oper\n");
        exit(1);
    }

    int fd;
    off_t offset = atoi(argv[1]);
    off_t fileSize;    

    // 파일 open
    if((fd = open(argv[3], O_RDWR)) < 0){
        fprintf(stderr, "open error for %s\n", argv[3]);
        exit(1);        
    }

    fileSize = lseek(fd, (off_t)0, SEEK_END);

    // offset > filesize : error
    if(offset > fileSize){
        fprintf(stderr, "offset is large than filesize\n");
        exit(1);
    }
    else lseek(fd, offset, SEEK_SET);

    write(fd, argv[2], strlen(argv[2]));

    close(fd);
}