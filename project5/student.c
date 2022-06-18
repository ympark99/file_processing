#include <stdio.h>		// 필요한 header file 추가 가능
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "student.h"

char recordbuf[RECORD_SIZE];

//
// 함수 readRecord()는 학생 레코드 파일에서 주어진 rrn에 해당하는 레코드를 읽어서 
// recordbuf에 저장하고, 이후 unpack() 함수를 호출하여 학생 타입의 변수에 레코드의
// 각 필드값을 저장한다. 성공하면 1을 그렇지 않으면 0을 리턴한다.
// unpack() 함수는 recordbuf에 저장되어 있는 record에서 각 field를 추출하는 일을 한다.
//
int readRecord(FILE *fp, STUDENT *s, int rrn);
void unpack(const char *recordbuf, STUDENT *s);

//
// 함수 writeRecord()는 학생 레코드 파일에 주어진 rrn에 해당하는 위치에 recordbuf에 
// 저장되어 있는 레코드를 저장한다. 이전에 pack() 함수를 호출하여 recordbuf에 데이터를 채워 넣는다.
// 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int writeRecord(FILE *fp, const STUDENT *s, int rrn);
void pack(char *recordbuf, const STUDENT *s);

//
// 함수 appendRecord()는 학생 레코드 파일에 새로운 레코드를 append한다.
// 레코드 파일에 레코드가 하나도 존재하지 않는 경우 (첫 번째 append)는 header 레코드를
// 파일에 생성하고 첫 번째 레코드를 저장한다. 
// 당연히 레코드를 append를 할 때마다 header 레코드에 대한 수정이 뒤따라야 한다.
// 함수 appendRecord()는 내부적으로 writeRecord() 함수를 호출하여 레코드 저장을 해결한다.
// 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

//
// 학생 레코드 파일에서 검색 키값을 만족하는 레코드가 존재하는지를 sequential search 기법을 
// 통해 찾아내고, 이를 만족하는 모든 레코드의 내용을 출력한다. 검색 키는 학생 레코드를 구성하는
// 어떤 필드도 가능하다. 내부적으로 readRecord() 함수를 호출하여 sequential search를 수행한다.
// 검색 결과를 출력할 때 반드시 printRecord() 함수를 사용한다. (반드시 지켜야 하며, 그렇지
// 않는 경우 채점 프로그램에서 자동적으로 틀린 것으로 인식함)
//
void searchRecord(FILE *fp, FIELD f, char *keyval);
void printRecord(const STUDENT *s);

//
// 레코드의 필드명을 enum FIELD 타입의 값으로 변환시켜 준다.
// 예를 들면, 사용자가 수행한 명령어의 인자로 "NAME"이라는 필드명을 사용하였다면 
// 프로그램 내부에서 이를 NAME(=1)으로 변환할 필요성이 있으며, 이때 이 함수를 이용한다.
//
FIELD getFieldID(char *fieldname);

//
// 학생 레코드 파일에서 "학번" 키값을 만족하는 레코드를 찾아서 이것을 삭제한다.
// 참고로, 검색 조건은 반드시 학번(ID)만을 사용한다. 레코드의 검색은 searchRecord() 함수와
// 유사한 방식으로 구현한다. 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int deleteRecord(FILE *fp, FIELD f, char *keyval);

//
// 학생 레코드 파일에 새로운 레코드를 추가한다. 과제 설명서에서 언급한 대로, 삭제 레코드가
// 존재하면 반드시 삭제 레코드들 중 하나에 새로운 레코드를 저장한다. 삭제 레코드 리스트 관리와
// 삭제 레코드의 선택은 교재 방식을 따른다. 새로운 레코드의 추가는 appendRecord() 함수와 비슷한
// 방식으로 구현한다. 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int insertRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

FIELD getFieldID(char *fieldname){
	FIELD a;
	if (strcmp(fieldname, "ID") == 0)
		a = ID;
	if (strcmp(fieldname, "NAME") == 0)
		a = NAME;
	if (strcmp(fieldname, "DEPT") == 0)
		a = DEPT;
	if (strcmp(fieldname, "ADDR") == 0)
		a = ADDR;
	if (strcmp(fieldname, "EMAIL") == 0)
		a = EMAIL;
	
	return a;
}

void main(int argc, char *argv[])
{
	FILE *fp;			// 모든 file processing operation은 C library를 사용할 것
	char *filename = argv[2];

	if ((fp = fopen(argv[2], "r+")) == NULL){ // 파일 없을 경우
		fp = fopen(argv[2], "w+");
	}

	if(!strcmp(argv[1], "-a")){ // append
		appendRecord(fp, argv[3], argv[4], argv[5], argv[6], argv[7]);
	}
	else if(!strcmp(argv[1], "-s")){ // search		
		char *tmp =(char *)malloc(strlen(argv[3])+1);
		strcpy(tmp, argv[3]);

		char *keyval;
		char *field_string;

		char *ptr = strtok(tmp, "=");
		field_string = ptr; // field

		if (ptr != NULL)
			ptr = strtok(NULL, "=");
		keyval = ptr; // key value

		FIELD field = getFieldID(field_string);
		
		searchRecord(fp, field, keyval);
		free(tmp);
	}
	else if(!strcmp(argv[1], "-d")){ // delete
		char *tmp =(char *)malloc(strlen(argv[3])+1);
		strcpy(tmp, argv[3]);

		char *keyval;
		char *field_string;

		char *ptr = strtok(tmp, "=");
		field_string = ptr; // field

		if (ptr != NULL)
			ptr = strtok(NULL, "=");
		keyval = ptr; // key value

		FIELD field = getFieldID(field_string);
		
		deleteRecord(fp, field, keyval);
		free(tmp);
	}
	fclose(fp);
	exit(0);
}

void printRecord(const STUDENT *s)
{
	printf("%s | %s | %s | %s | %s\n", s->id, s->name, s->dept, s->addr, s->email);
}

void pack(char *recordbuf, const STUDENT *s){
	sprintf(recordbuf,"%s#%s#%s#%s#%s#",s->id, s->name, s->dept, s->addr, s->email);
}

void unpack(const char *recordbuf, STUDENT *s){
	char buf[5][31];
	char *ptr;
	char *tmp = (char *)malloc(strlen(recordbuf) + 1);
	strcpy(tmp, recordbuf);
	ptr = strtok(tmp, "#");
	int cnt = 0;
	while (ptr != NULL){
		strcpy(buf[cnt], ptr);
		ptr = strtok(NULL, "#");
		cnt++;
	}
	strcpy(s->id, buf[0]);
    strcpy(s->name, buf[1]);
	strcpy(s->dept, buf[2]);
	strcpy(s->addr, buf[3]);
	strcpy(s->email, buf[4]);
	free(tmp);
}

int readRecord(FILE *fp, STUDENT *s, int rrn){
	fseek(fp, HEADER_SIZE, SEEK_SET); // 헤더 크기 이동 (rrn 0)
	fseek(fp, (rrn - 1) * RECORD_SIZE, SEEK_CUR);
    if(fread(recordbuf, RECORD_SIZE, 1, fp) == 0){
		return 0;
	}
    else{
		return 1;
	}
}

int writeRecord(FILE *fp, const STUDENT *s, int rrn){
	pack(recordbuf, s);

	fseek(fp, HEADER_SIZE, SEEK_SET); // 헤더 크기 이동 (rrn 0)
	fseek(fp, (rrn - 1) * RECORD_SIZE, SEEK_CUR);
    if(fwrite(recordbuf, RECORD_SIZE, 1, fp) == 0)
		return 0;
	return 1;
}

int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email){
	STUDENT s;
	strcpy(s.id, id);
	strcpy(s.name, name);
	strcpy(s.dept, dept);
	strcpy(s.addr, addr);
	strcpy(s.email, email);

	int records;
	int reserved;	
	
	char *header_buf = malloc(sizeof(char) * HEADER_SIZE); // 헤더 레코드
	// 아무것도 없다면 헤더 레코드 생성
	if(readRecord(fp, &s, 0) == 0){
		records = 1;
		reserved = -1; // 삭제 리스트 헤드 없으므로 -1 저장
		memcpy(header_buf, &records, 4);
		memcpy(header_buf+4, &reserved, 4);

		fseek(fp, 0, SEEK_SET); // 헤더 크기 이동 (rrn 0)
		fwrite(header_buf, sizeof(char) * RECORD_SIZE, 1, fp); // 헤더 써주기
	}

	fseek(fp, 0, SEEK_SET);
	fread(header_buf, sizeof(char) * HEADER_SIZE, 1, fp); // 어디에 쓸지 읽기
	memcpy(&records, header_buf, 4);
	writeRecord(fp, &s, records); // 레코드 추가
	records++; // 전체 개수 1 추가
	memcpy(header_buf, &records, 4);
	fseek(fp, 0, SEEK_SET); // (rrn 0) 이동
	fwrite(header_buf, sizeof(char) * 4, 1, fp); // 헤더 써주기
	free(header_buf);
}

void searchRecord(FILE *fp, FIELD f, char *keyval){
	STUDENT s;

	if(readRecord(fp, &s, 0) == 0) return; // 빈 파일이면 리턴
	int records;
	char *header_buf = malloc(sizeof(char) * HEADER_SIZE); // 헤더 레코드
	
	fseek(fp, 0, SEEK_SET);
	fread(header_buf, sizeof(char) * HEADER_SIZE, 1, fp); // 어디에 쓸지 읽기
	memcpy(&records, header_buf, 4); // 레코드 개수 가져오기

	STUDENT s_search[records]; // 찾은 학생 (최대 records 크기이므로)
	int s_num = 0;

	for(int i = 1; i < records; i++){
		if(readRecord(fp, &s, i) == 0)
			break;
		
		unpack(recordbuf, &s); // 구조체 읽은 값저장

		switch (f){ // FIELD 처리
			case 0:
				if (strcmp(s.id, keyval) == 0){
					s_search[s_num] = s;
					s_num++;
				}
				break;
			case 1:
				if (strcmp(s.name, keyval) == 0){
					s_search[s_num] = s;
					s_num++;
				}
				break;
			case 2:
				if (strcmp(s.dept, keyval) == 0){
					s_search[s_num] = s;
					s_num++;
				}
				break;
			case 3:
				if (strcmp(s.addr, keyval) == 0){
					s_search[s_num] = s;
					s_num++;
				}
				break;
			case 4:
				if (strcmp(s.email, keyval) == 0){
					s_search[s_num] = s;
					s_num++;
				}
				break;
			default:
				break;
		}
	}

	for(int i = 0; i < s_num; i++){
		printRecord(&s_search[i]);
	}

	free(header_buf);
}

int deleteRecord(FILE *fp, FIELD f, char *keyval){
	STUDENT s;
    if(readRecord(fp, &s, 0) == 0) return -1; // 빈 파일이면 리턴
	int records, reserved; // 레코드 수, 최근 삭제
	char *header_buf = malloc(sizeof(char) * HEADER_SIZE); // 헤더 레코드
	fseek(fp, 0, SEEK_SET);
	fread(header_buf, sizeof(char) * HEADER_SIZE, 1, fp); // 어디에 쓸지 읽기
	memcpy(&records, header_buf, 4); // 레코드 개수 가져오기
	memcpy(&reserved, header_buf + 4, 4); // 최근 삭제 rrn 가져오기
	printf("record : %d reser : %d\n", records, reserved);

	STUDENT s_search; // 찾은 학생 (최대 records 크기이므로)

	for(int i = 1; i < records; i++){ // 해당 id 찾기
		int backup = ftell(fp); // 삭제할 레코드로 돌아갈 위치 저장
		if(readRecord(fp, &s, i) == 0)
			break;
		unpack(recordbuf, &s); // 구조체 읽은 값저장

		if (strcmp(s.id, keyval) == 0){ // 삭제할 레코드 발견시
			s_search = s;

			fseek(fp, backup, SEEK_SET); // 삭제할 레코드 위치로 이동
			char del_mark = '*'; // 삭제할 마크
			char *recent_rrn = malloc(sizeof(char) * 4); // 4바이트 최근 삭제레코드 번호
			memcpy(recent_rrn, &reserved, 4);

			if(fwrite(&del_mark, 1, 1, fp) == 0) // 삭제 마크 저장
				return -1;
			printf("pre rrn : %s\n", recent_rrn);
			if(fwrite(&recent_rrn, 4, 1, fp) == 0) // 이전 rrn 저장
				return -1;
			free(recent_rrn);

			// 해더 파일-> 예약 공간으로 이동
			fseek(fp, 4, SEEK_SET);

			// 최근 삭제 rrn 갱신
			char *reserve_buf = malloc(sizeof(char) * 4); // 삭제할 레코드
			memcpy(reserve_buf, &i+1, 4); // 최근 삭제 rrn 갱신, 헤더 포함하므로 +1
			printf("new rrn : %s\n", reserve_buf);
			fwrite(reserve_buf, sizeof(char) * 4, 1, fp); // 삭제 rrn 써주기
			free(reserve_buf);

			break;
		}
	}

	free(header_buf);
}

int insertRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email){


}
