#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1024

void seq_read(int fd);

int main(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stderr, "argc error\n");
		exit(1);		
	}

	int fd;
	if((fd = open(argv[1], O_RDONLY)) < 0){
		fprintf(stderr, "open error\n");
		exit(1);
	}

	seq_read(fd);

	return 0;
}

void seq_read(int fd){
	struct timeval start, end;
	char buf[BUFFER_SIZE];

	lseek(fd, 0, SEEK_SET);

	gettimeofday(&start,NULL); // 시간 측정

	while((read(fd, buf, BUFFER_SIZE)) > 0){
		memset(buf, 0, BUFFER_SIZE);
	}
	
	gettimeofday(&end,NULL); // 시간 측정 끝

	printf("%ld usec\n",((end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec));
}
