#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DATA_SIZE 39 // 200 바이트 준수

typedef struct Student{
	char name[DATA_SIZE+1];
	char phone[DATA_SIZE+1];
	char addr[DATA_SIZE+1];
	char major[DATA_SIZE+1];
	char email[DATA_SIZE+1];
}Student;

void init(struct Student *st);
void create_record(int fd,int recordnum);

int main(int argc, char *argv[]){
	if(argc != 3){
		fprintf(stderr, "argc error\n");
		exit(1);
	}

	int fd;
	if((fd = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC, 0644)) < 0){
		fprintf(stderr, "open error\n");
		exit(1);
	}

	create_record(fd, atoi(argv[1]));	

	return 0;
}

void init(struct Student *st){
	for(int i = 0; i < DATA_SIZE; i++){
		st->name[i] = 'a';
		st->phone[i] = 'b';
		st->addr[i] = 'c';
		st->major[i] = 'd';
		st->email[i] = 'e';
	}
	// 마지막 null로 채우기
	st->name[DATA_SIZE] = '\0';
	st->phone[DATA_SIZE] = '\0';
	st->addr[DATA_SIZE] = '\0';
	st->major[DATA_SIZE] = '\0';
	st->email[DATA_SIZE] = '\0';
}

void create_record(int fd, int num){
	struct Student st; // 버퍼 생성
	init(&st);

	for(int i = 0; i < num; i++){
		write(fd, &st, sizeof(st));
	}
}