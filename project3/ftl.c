#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"

AddrMapTbl addrmaptbl;
extern FILE *devicefp;
int reserved_empty_blk = DATABLKS_PER_DEVICE;
/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void print_block(int pbn);
void print_addrmaptbl();

void dd_read(int ppn, char*page_buf);
void dd_write(int ppn,char*page_buf);
void dd_erase(int pbn);

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다
//
void ftl_open()
{
	int i;

	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		addrmaptbl.pbn[i] = -1;
	}

	return;
}

//
// file system을 위한 FTL이 제공하는 write interface
// 'sectorbuf'가 가리키는 메모리의 크기는 'SECTOR_SIZE'이며, 호출하는 쪽에서 미리 메모리를 할당받아야 함
//
void ftl_write(int lsn, char *sectorbuf){
// #ifdef PRINT_FOR_DEBUG			// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
// 	print_addrmaptbl_info();
// #endif

	char page_buf[PAGE_SIZE];
	SpareData* spare = (SpareData *)malloc(SPARE_SIZE);
	int lbn = lsn / PAGES_PER_BLOCK;
	int pbn = addrmaptbl.pbn[lbn];
	int offset = lsn % PAGES_PER_BLOCK;

	// block 첫번째 write
	if(pbn == -1){
		for(int i = 0; i < BLOCKS_PER_DEVICE; i++){
			if(i == reserved_empty_blk) continue;
			char *tmppage = (char*)malloc(PAGE_SIZE);
			dd_read(i * PAGES_PER_BLOCK, tmppage);
			memcpy(spare, tmppage + SECTOR_SIZE, SPARE_SIZE);
			if(spare->lsn == -1){
				pbn = i;
				break;
			}
			free(tmppage);
		}

		int ppn = pbn * PAGES_PER_BLOCK + offset;

		memcpy(page_buf, sectorbuf, SECTOR_SIZE);	
		memcpy(spare, page_buf + SECTOR_SIZE, SPARE_SIZE);

		spare->lsn = lsn; // spare lsn 저장
		memcpy(page_buf + SECTOR_SIZE, spare, SPARE_SIZE);

		dd_write(ppn, page_buf);
		addrmaptbl.pbn[lbn] = pbn;
		return;
	}
	else{
		int ppn = pbn * PAGES_PER_BLOCK + offset;
		int check_file;

		dd_read(ppn, page_buf);
		memcpy(spare, page_buf + SECTOR_SIZE, SPARE_SIZE);

		if(spare->lsn == -1){
			memcpy(page_buf, sectorbuf, SECTOR_SIZE);
			spare->lsn = lsn;
			memcpy(page_buf + SECTOR_SIZE, spare, sizeof(int));
			dd_write(ppn, page_buf);
		}
		else{
			int originppn = pbn * PAGES_PER_BLOCK;
			int newppn = reserved_empty_blk * PAGES_PER_BLOCK;
			int nowppn;
			for(int i = 0; i < PAGES_PER_BLOCK; i++){
				nowppn = originppn + i;
				dd_read(nowppn, page_buf);
				memcpy(spare, page_buf + SECTOR_SIZE, SPARE_SIZE);
				if(ppn = nowppn){
					memcpy(page_buf, sectorbuf, SECTOR_SIZE);
					dd_write(newppn + i, page_buf);
				}
				else dd_write(newppn + i, page_buf);
			}
			
			// update
			addrmaptbl.pbn[lbn] = reserved_empty_blk;
			reserved_empty_blk = pbn;
			dd_erase(pbn);
		}	
	}
	return;
}

//
// file system을 위한 FTL이 제공하는 read interface
// 'sectorbuf'가 가리키는 메모리의 크기는 'SECTOR_SIZE'이며, 호출하는 쪽에서 미리 메모리를 할당받아야 함
// 
void ftl_read(int lsn, char *sectorbuf){
// #ifdef PRINT_FOR_DEBUG			// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
// 	print_addrmaptbl_info();
// #endif

	char page_buf[PAGE_SIZE];
	int lbn = lsn / PAGES_PER_BLOCK;
	int pbn = addrmaptbl.pbn[lbn];
	int offset = lsn % PAGES_PER_BLOCK;
	int ppn = pbn * PAGES_PER_BLOCK + offset;

	dd_read(ppn, page_buf);	
	memcpy(sectorbuf, page_buf, SECTOR_SIZE);

	return;
}

void print_block(int pbn){
	char *page_buf;
	SpareData *sdata;
	int i;
	
	page_buf = (char *)malloc(PAGE_SIZE);
	sdata = (SpareData *)malloc(SPARE_SIZE);

	printf("Physical Block Number: %d\n", pbn);

	for(i = pbn*PAGES_PER_BLOCK; i < (pbn+1)*PAGES_PER_BLOCK; i++)
	{
		dd_read(i, page_buf);
		memcpy(sdata, page_buf+SECTOR_SIZE, SPARE_SIZE);
		printf("\t   %5d-[%7d]\n", i, sdata->lsn);
	}

	free(page_buf);
	free(sdata);

	return;
}

void print_addrmaptbl()
{
	int i;

	printf("Address Mapping Table: \n");
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		if(addrmaptbl.pbn[i] >= 0)
		{
			printf("[%d %d]\n", i, addrmaptbl.pbn[i]);
		}
	}
}
