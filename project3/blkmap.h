#ifndef	_BLOCK_MAPPING_H_
#define	_BLOCK_MAPPING_H_

#define TRUE			1
#define	FALSE			0

#define	SECTOR_SIZE		512			
#define	SPARE_SIZE		16			
#define	PAGE_SIZE		(SECTOR_SIZE+SPARE_SIZE)
#define SECTORS_PER_PAGE	1
#define	PAGES_PER_BLOCK		4				// 수정 가능
#define	SECTORS_PER_BLOCK	(SECTORS_PER_PAGE*PAGES_PER_BLOCK)
#define	BLOCK_SIZE		(PAGE_SIZE*PAGES_PER_BLOCK)
#define	BLOCKS_PER_DEVICE	16				// 수정 가능
#define	DEVICE_SIZE		(BLOCK_SIZE*BLOCKS_PER_DEVICE)
#define	DATABLKS_PER_DEVICE	(BLOCKS_PER_DEVICE - 1)	// spare block로서 블록 하나를 사용
#define DATAPAGES_PER_DEVICE (DATABLKS_PER_DEVICE * PAGES_PER_BLOCK) // 데이터 저장할 수 있는 실제 page 수
//
// flash memory의 spare area를 다루기 위한 구조체
//
typedef struct
{
	int lsn;			// page에 데이터를 저장할 때 spare area에 lsn도 같이 저장함
	char dummy[SPARE_SIZE - 4];
} SpareData;

//
// FTL이 관리하는 address mapping table을 위한 구조체
//
typedef struct
{
	int pbn[DATABLKS_PER_DEVICE];
} AddrMapTbl;

#endif
