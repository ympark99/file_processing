# project3 : Block mapping 기법(강의자료 “Flash Memory Overview”)을 따르는 FTL layer를 구현

### 소스 파일
- blkmap.h: flash memory를 구성하는 block, page, sector, spare area 등의 상수 정의와 address mapping table에 대한 구조체가 정의되어 있음
- devicedriver.c: flash memory에 page 단위로 데이터를 읽고 쓰기 위한 read()와 write() 함수와, 또한 block을 소거하는 erase() 함수가 정의되어 있음
- ftl.c : 상기의 FTL 기법을 따르는 ftl_open(), ftl_write(), ftl_read()를 구현해야 됨
- main.c : file system의 역할을 수행하며 ftl_write()와 ftl_read() 테스트를 위해 제공하는 것으로 필요하면 활용하기 바람

### 준수 사항
- flash memory를 위한 파일명은 반드시 ‘flashmemory’이어야 하며(main.c 참조), 또한 실행 디렉토리에 생성되어야 함
- 프로그램 작성 시 반드시 blkmap.h에 정의되어 있는 상수를 사용해야 하며, 상수 값은 “수정가능”으로 표시된 것만 원하는 값으로 바꿀 수 있음
- flash memory는 초기에 모든 byte 값이 ‘OxFF‘으로 설정되며(main.c 참조), 다른 값으로 초기화하면 채점 시 제대로 동작하지 않음
- block mapping 기법에서는 굳이 spare area에 lsn과 같은 필요한 정보를 넣을 필요가 없으나, 공부하는 차원에서 spare area에 lsn을 저장할 것
- FTL layer에서 flashmemory에 직접 데이터를 읽고 쓸 수 없으며, 반드시 제공되는 제공되는 device driver에서 제공하는 interface를 호출해야 함. 그렇지 않은 경우 채점 프로그램이 제대로 동작하지 않음
