#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    char buf;
    int fd1, fd2, fd3;

    // 제대로 된 입력 아닐경우
    if(argc != 4){
        fprintf(stderr, "Please input correct oper\n");
        exit(1);
    }

    if((fd3 = open(argv[3], O_CREAT | O_WRONLY | O_APPEND, 0666)) < 0){
        fprintf(stderr, "open error for %s\n", argv[3]);
        exit(1);
    }

    // 파일 1 open
    if((fd1 = open(argv[1], O_RDONLY)) < 0){
        fprintf(stderr, "open error for %s\n", argv[1]);
        exit(1);        
    }

    // 파일2 open
    if((fd2 = open(argv[2], O_RDONLY)) < 0){
        fprintf(stderr, "open error for %s\n", argv[2]);
        exit(1);        
    }        

    // 파일1
    while (read(fd1, &buf, 1) != 0)
        write(fd3, &buf, 1);
    close(fd1);
    // 파일2
    while (read(fd2, &buf, 1) != 0)
        write(fd3, &buf, 1);
    close(fd2);    
    close(fd3);
}