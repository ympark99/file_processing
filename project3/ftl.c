#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"

AddrMapTbl addrmaptbl;
SpareData spare;
extern FILE *devicefp;
int free_block;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void print_block(int pbn);
void print_addrmaptbl();

void ftl_setFreeBlock();
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
	int lbn;
	char *page_buf;
	page_buf = (char*)malloc(PAGE_SIZE);
	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		addrmaptbl.pbn[i] = -1;
	}

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		dd_read(i * PAGES_PER_BLOCK, page_buf);
		memcpy(&lbn, &page_buf[SECTOR_SIZE], sizeof(int));
		if(lbn > -1)
			addrmaptbl.pbn[lbn] = i;
	}
	free(page_buf);

	ftl_setFreeBlock();

	return;
}

//
// file system을 위한 FTL이 제공하는 write interface
// 'sectorbuf'가 가리키는 메모리의 크기는 'SECTOR_SIZE'이며, 호출하는 쪽에서 미리 메모리를 할당받아야 함
//
void ftl_write(int lsn, char *sectorbuf){
#ifdef PRINT_FOR_DEBUG			// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_addrmaptbl_info();
#endif

	//
	// block mapping 기법에서 overwrite가 발생하면 이를 해결하기 위해 반드시 하나의 empty block이
	// 필요하며, 초기값은 flash memory에서 맨 마지막 block number를 사용함
	// overwrite를 해결하고 난 후 당연히 reserved_empty_blk는 overwrite를 유발시킨 (invalid) block이 되어야 함
	// 따라서 reserved_empty_blk는 고정되어 있는 것이 아니라 상황에 따라 계속 바뀔 수 있음
	//
	int reserved_empty_blk = DATABLKS_PER_DEVICE;

	if(lsn >= (reserved_empty_blk * PAGES_PER_BLOCK)){
		fprintf(stderr, "lsn's Maximum is 60\n");
		return;
	}

	char page_buf[PAGE_SIZE];
	int lbn = lsn / PAGES_PER_BLOCK;
	int pbn = addrmaptbl.pbn[lbn];
	int offset = lsn % PAGES_PER_BLOCK;	

	// block 첫번째 write
	if(pbn == -1){
		for(int i = 0; i < BLOCKS_PER_DEVICE; i++){
			int check = 0;
			char *tmppage = (char*)malloc(PAGE_SIZE);
			dd_read(i * PAGES_PER_BLOCK, tmppage);
			memcpy(&check, &tmppage[SECTOR_SIZE], 4);
			if(check == -1){
				pbn = i;
				break;
			}
			free(tmppage);
		}

		int ppn = pbn * PAGES_PER_BLOCK + offset;
		memcpy(page_buf, sectorbuf, SECTOR_SIZE);	

		if(offset == 0){ // offset 0일 경우
			// ppn : block 첫페이지
			// lsn -> ppn write
			memcpy(&page_buf[SECTOR_SIZE], &lbn, 4);
			memcpy(&page_buf[SECTOR_SIZE + 4], &lsn, 4);
			dd_write(ppn, page_buf);
		}else{
			// lsn -> ppn write
			memcpy(&page_buf[SECTOR_SIZE + 4], &lsn, 4);
			dd_write(ppn, page_buf);
			// pbn 첫 페이지에 lbn write
			ppn = pbn * PAGES_PER_BLOCK;
			dd_read(ppn, page_buf);
			memcpy(&page_buf[SECTOR_SIZE], &lbn, 4);
			dd_write(ppn, page_buf);
		}
		addrmaptbl.pbn[lbn] = pbn;
		return;
	}else{
		int ppn = pbn * PAGES_PER_BLOCK + offset;
		int check_file;

		dd_read(ppn, page_buf);
		memcpy(&check_file, &page_buf[SECTOR_SIZE + 4], 4);
		
		// page 첫번째 write
		if(check_file == -1){
			memcpy(page_buf, sectorbuf, SECTOR_SIZE);
			memcpy(&page_buf[SECTOR_SIZE + 4], &lsn, 4);
			dd_write(ppn, page_buf);
			return;

		}
		//in place update x
		else{
			int originppn = pbn * PAGES_PER_BLOCK + offset;
			int newpbn = free_block;
			int newppn = newpbn * PAGES_PER_BLOCK + offset;

			for(int i = 0; i < PAGES_PER_BLOCK; i++){
				if(i != offset){
					dd_read(pbn * PAGES_PER_BLOCK + i , page_buf);
					dd_write(newpbn * PAGES_PER_BLOCK + i, page_buf);
				}
			}
			dd_read(originppn, page_buf);
			memcpy(page_buf, sectorbuf, SECTOR_SIZE);
			dd_write(newppn, page_buf);
			
			// update
			dd_erase(pbn);
			addrmaptbl.pbn[lbn] = newpbn;
			ftl_setFreeBlock();
			return;
		}	
	}
	return;
}

//
// file system을 위한 FTL이 제공하는 read interface
// 'sectorbuf'가 가리키는 메모리의 크기는 'SECTOR_SIZE'이며, 호출하는 쪽에서 미리 메모리를 할당받아야 함
// 
void ftl_read(int lsn, char *sectorbuf){
#ifdef PRINT_FOR_DEBUG			// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_addrmaptbl_info();
#endif

	if(lsn >= (DATABLKS_PER_DEVICE * PAGES_PER_BLOCK)){
		fprintf(stderr, "lsn's Maximum is 60");
		return;
	}
	if(addrmaptbl.pbn[lsn / PAGES_PER_BLOCK] == -1){
		fprintf(stderr, "There is no data in %d block", lsn);
		return;
	}

	char page_buf[PAGE_SIZE];
	int lbn = lsn / PAGES_PER_BLOCK;
	int pbn = addrmaptbl.pbn[lbn];
	int offset = lsn % PAGES_PER_BLOCK;
	int ppn = pbn * PAGES_PER_BLOCK + offset;

	int savedlsn;
	dd_read(ppn, page_buf);
	
	memcpy(&savedlsn, &page_buf[SECTOR_SIZE + 4], 4);
	if(savedlsn == -1){
		fprintf(stderr,"There is no data in %d page", lsn);
		return;
	}
	else{
		memcpy(sectorbuf, page_buf, SECTOR_SIZE);
		return;
	}
}

//
// for debugging
//
void print_block(int pbn){
	char *page_buf;
	SpareData *sdata;
	int i;
	
	page_buf = (char *)malloc(PAGE_SIZE);
	sdata = (SpareData *)malloc(SPARE_SIZE);

	printf("Physical Block Number: %d\n", pbn);

	for(i = pbn*PAGES_PER_BLOCK; i < (pbn+1)*PAGES_PER_BLOCK; i++)
	{
		read(i, page_buf);
		memcpy(sdata, page_buf+SECTOR_SIZE, SPARE_SIZE);
		printf("\t   %5d-[%7d]\n", i, sdata->lsn);
	}

	free(page_buf);
	free(sdata);

	return;
}

//
// for debugging
//
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

void ftl_setFreeBlock(){
	int list[BLOCKS_PER_DEVICE]= {0,};

	for(int i =0; i<BLOCKS_PER_DEVICE-1;i++)
	{
		if(addrmaptbl.pbn[i] != -1)
			list[addrmaptbl.pbn[i]] = 1;
	}

	for(int i = 0; i<BLOCKS_PER_DEVICE; i++)
	{
		if(list[i] != 1)
			free_block = i;
	}
			
}
