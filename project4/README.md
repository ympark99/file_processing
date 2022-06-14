# project4 : 레코드 저장 및 검색

### 개요
“학생” 레코드 파일에 저장되어 있는 각 레코드(record)는 다음과 같은 특징을 갖는다.
- 하나의 레코드는 다섯 개의 필드(field)로 구성된다.
- 레코드 구성하는 필드의 순서, 이름, 최대 크기(바이트 단위)는 다음과 같다.
     학번(8), 이름(10), 학과(12), 주소(30), 이메일(20)
- 레코드 구분은 고정 길이 레코드 (fixed-length record) 방식을 이용한다.
레코드 내의 필드 구분은 구분자(delimiter) 방식을 이용한다.
- 구분자는 ‘#“을 사용한다.
파일에 저장되는 각 레코드의 길이는 당연히 85 바이트가 된다. (student.h 참조)

학생 레코드 파일에는 헤더 레코드(header record)가 존재하며, 전체 레코드의 수를 저장하는 4 바이트 공간과 예약 공간을 위한 4 바이트로서 총 8 바이트의 공간이 필요하다. 학생 레코드의 번호는 rrn=0, 1, 2, ... 의 값을 갖는다.

구현해야 할 프로그램은 아래 두 가지 기능, append와 search을 가진다.

### 레코드 추가(append)
사용자로부터 레코드 파일명과 필드값 리스트를 입력 받는다. 학생 레코드는 5개의 필드로 구성되어 있으므로 5개의 “필드값”의 인자가 필요하며, 필드값은 영문, 숫자, 기호로만 구성되어 있다고 가정한다. 프로그램 수행 결과에 대한 출력은 없다. 필드값은 반드시 큰따옴표로 묶어서 입력한다.

- $ a.out –a record_file_name “field_value1” “field_value2” “field_value3” “field_value4” “field_value5”

실행파일과 같은 디렉토리에 있는 'records.dat' 파일에 대해 새로운 학생 레코드를 저장하는 예는 다음과 같다.

- $ a.out –a records.dat “20071234” “Gildong Hong” “Computer” “Dongjak-gu, Seoul” “gdhong@ssu.ac.kr”

### 레코드 검색(search)
사용자로부터 레코드 파일명, 필드명, 필드값을 입력받고, 제공된 출력함수를 이용하여 검색 결과를 출력한다 (student.c 참조). 검색 키로 사용하는 필드명으로 ID, NAME, DEPT, ADDR, EMAIL을 각각 사용한다 (주의: 필드명은 반드시 대문자로 표현됨). 검색 조건(필드명=필드값)은 반드시 큰따옴표로 묶어서 입력한다.

- $ a.out –s record_file_name “field_name=field_value”

실행파일과 같은 디렉토리에 있는 'records.dat' 파일에 대해서, “NAME=Gildong Hong”을 만족하는 레코드를 검색하고 그 결과를 출력하면 다음과 같다.

- $ a.out –s records.dat “NAME=Gildong Hong”
  - 20071234 | Gildong Hong | Computer | Dongjak-gu, Seoul | gdhong@ssu.ac.kr
  - 20041328 | Gildong Hong | Computer | Gawnak-gu, Seoul | gildong@ssu.ac.kr
