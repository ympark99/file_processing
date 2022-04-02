#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SUFFLE_NUM	10000 // 이 값은 마음대로 수정 가능
#define BUFFER_SIZE 1024
#define RECORD_SIZE 200

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
void rand_read(int *read_order_list, int fd, int num_of_records);

int main(int argc, char *argv[]){
	int *read_order_list;
	int num_of_records;
	int file_size;

	if(argc != 2){
		fprintf(stderr, "argc error\n");
		exit(1);		
	}

	int fd;
	if((fd = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "open error\n");
		exit(1);
	}	

	if((file_size = lseek(fd, 0, SEEK_END)) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}	
	lseek(fd, 0, SEEK_SET);

	num_of_records = file_size / RECORD_SIZE;

	read_order_list = (int *)calloc(num_of_records, sizeof(int));

	// 이 함수를 실행하면 'read_order_list' 배열에 읽어야 할 레코드 번호들이 순서대로
	// 나열되어 저장됨. 'num_of_records'는 레코드 파일에 저장되어 있는 전체 레코드의 수를 의미함.
	GenRecordSequence(read_order_list, num_of_records);


	// 'read_order_list'를 이용하여 레코드 파일로부터 전체 레코드를 random 하게 읽어들이고,
	// 이때 걸리는 시간을 측정하는 코드 구현
	rand_read(read_order_list, fd, num_of_records);

	return 0;
}

void rand_read(int *read_order_list, int fd, int num_of_records){
	char buf[BUFFER_SIZE];
	struct timeval start,end;

	gettimeofday(&start,NULL);

	for(int i = 0; i < num_of_records; i++) {
		lseek(fd, (read_order_list[i] * RECORD_SIZE), SEEK_SET);
		read(fd, buf, RECORD_SIZE);
		memset(buf, 0, BUFFER_SIZE);
	}

	gettimeofday(&end,NULL);

	printf("%ld usec\n",((end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec));
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}