# OS from Scratch

Carlos Fenollosa @cfenollosa님의 [os-tutorial](https://github.com/cfenollosa/os-tutorial), University of Birmingham의 Nick Blundell님의 [Writing a Simple Operating System - from Scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf), OS-Dev Wiki의 [Creating an Operating System](https://wiki.osdev.org/Creating_an_Operating_System#Keyboard)을 참고해 공부한 내용을 정리한 노트입니다. 코드는 cfenollosa님의 os-tutorial을 기반으로 하고 os-tutorial에서 설명하지 않은 필요한 이론들을 Nick Blundell님의 자료와, OS-Dev Wiki를 참고해 [저의 티스토리](https://jschang.tistory.com/category/OS/OS%20from%20Scratch)에 한국어로 정리하였습니다. 

## Requirements
본 자료를 이해하기 위해서는 다음 지식이 필요한 것 같습니다.
* 기초적인 C언어 지식 (함수, 포인터, 메모리)
* 약간의 Assembly 지식 (mov, 레지스터, Label 사용, jmp, loop)
    * 자세한 내용들은 코드가 등장할 때마다 설명합니다
* 열정!

## How to use this tutorial
본 자료를 100% 활용하기 위해서는 [티스토리](https://jschang.tistory.com/category/OS/OS%20from%20Scratch)의 포스트 번호 순서를 따라 이론과 코드를 익히고, 더 자세한 코드는 해당 포스트 번호의 cfenollosa님의 [os-tutorial](https://github.com/cfenollosa/os-tutorial) 코드를 참고하면 됩니다.

## 이론
본 자료에 사용되는 이론들을 모두 모은 것입니다. 자세한 내용들은 [저의 티스토리에](https://jschang.tistory.com/category/OS/OS%20from%20Scratch)에 한국어로 정리되어있습니다. 

### BIOS
BIOS는 부팅 시 하드웨어를 확인하는 역할을 합니다. 컴퓨터가 부팅할 때 짧게 벨소리가 들리는 것이 BIOS가 하드웨어에 문제가 없다는 것을 인식한 신호입니다.

### Boot Sector
Boot Sector는 Kernel을 불러오는 역할을 합니다. Boot Sector는 디스크에서 가장 앞에는 512byte크기로 있습니다. Boot Sector의 마지막은 0xAA55로 돼있는데, 이는 BIOS가 디스크가 부팅 가능하다는 것을 확인할 수 있게 합니다. 이번 강의에서 다룰 가장 간단한 형태의 Boot Sector는 처음 3개 byte는 무한 루프를 의미하며, 마지막 2byte는 0xAA55입니다. 가운데 있는 값은 0으로 초기화 해줍니다.

### Little Endian
x86아키텍처를 사용하는 대부분의 컴퓨터는 Little Endian로 데이터를 저장합니다. Little Endian은 한 단위의 데이터가 메모리에 저장될때 거꾸로 배열되는 형태를 의미합니다. 위 그림과 같이 0xAA55를 저장하면 55 AA로 저장되는 것을 확인할 수 있습니다. 더 자세한 내용은 [링크](https://genesis8.tistory.com/37) 을 참고하세요.

### Assembly $, $$ 명령어
$는 Assembly에서 현재 주소를 의미하며, $$는 처음 시작 주소를 의미합니다.
```assembly
times 510-($-$$) db 0
```
이를 기반으로 위 코드의 의미를 분석해봅시다. times는 어떤 값을 반복해 생성하는 명령어입니다. ($-$$)는 처음 위치에서 현재 위치가 떨어진 거리, 즉 현재 위치 앞에 존재하는 byte개수를 의미합니다. Boot Sector는 총 512byte이고 마지막 2byte는 0xAA55이므로, 510byte에서 무한 루프 코드를 제외한 나머지는 0으로 채워야 합니다. 무한 루프 코드의 길이가 $-$$이므로 510-($-$$) 만큼 0을 생성해야 합니다.

### TTY Mode
TTY Mode는 일반 CLI 콘솔 환경을 의미하며, ah 레지스터에 0x0e를 대입하면 TTY Mode로 진입한다. 

### Interrupt
Interrupt는 장치나 프로그램에서 OS로 보내는 신호로 OS에게 현재 실행 중인 서비스를 멈추게 합니다. 컴퓨터는 한번에 하나의 프로그램 밖에 실행하지 못하는데, Interrupt를 통해 Multi Tasking하는 것처럼 보이게 합니다.
본 강좌에서는 문자를 출력하기 위해 0x10 Interrupt를 사용합니다. al 레지스터에 값을 저장하고 int 0x10명령어를 통해 interrupt를 발생시키면 al 레지스터에 저장된 값이 비디오 서비스로 출력됩니다.

### 메모리 구조
BIOS는 Boot Sector를 0x7c00주소부터 채우게 됩니다. 이를 고려해 Boot Sector를 만듭니다. 

### Assembly org 명령어
```assembly
[org 0x7c00]
```
위 Assembly 코드는 모든 Assembly코드에서 0x7c00만큼 offset을 설정하는 코드입니다. 위 코드를 삽입하면 이후 나타나는 메모리 주소에서 0x7c00만큼의 주소가 더해져 계산됩니다.

### Assembly Stack
bp레지스터는 Stack의 Bottom을 의미하며, sp레지스터는 stack의 top을 의미합니다. 이때 Stack은 bp에서 시작해 아래로 sp까지 자라나는 구조이며, Stack에 원소를 넣을수록 sp가 줄어듭니다. Stack의 원소는 push, pop을 이용해 넣고 빼며, words, doublewords 크기의 데이터만 가능합니다.

### Assembly pusha, popa
pusha는 모든 레지스터에 저장된 값을 Stack에 저장, popa는 Stack에 저장됐던 모든 값을 원래 레지스터 값에 옮겨 원래 상태로 돌려놓는 기능을 합니다. 이와 같은 기능은 현재 상태에서 다른 코드의 기능을 사용하고 싶을 때 활용됩니다.

### Assembly 문자열
Assembly에서 문자열은 다음과 같이 사용됩니다.
```assembly
HELLO:
    db 'Hello World',0x00
```
이때 문자열의 끝에는 0x00 Null 문자를 삽입해야 합니다. Assembly에서 newline은 0x0a (Newline) + 0x0d (Carriage Return)으로 이루어집니다. 따라서 이를 출력하기 위한 코드는 다음과 같이 됩니다.

```assembly
mov ah, 0x0e
; newline 문자 
mov al, 0x0a
int 0x10
; carriage return 문자 
mov al, 0x0d
int 0x10
```

### Assembly에서 다른 파일 사용
Assembly 코드에서 다른 파일의 코드를 실행시키기 위해서는 다음 코드를 작성합니다.

```assembly
%include “파일명.asm”
이때 코드에 포함되어 있는 Label의 위치를 주의하며 파일을 불러와야 합니다.
```

### Assembly ror
0x1234을 ror 연산하면 값은 0x4123로 변환되고 ror 연산을 반복하면 0x3412, 0x2341, 0x1234로 됩니다.

### Segmentation
Segmentation은 03. Boot Sector Memory에서 설명했던 offset을 설정하는 개념입니다. Segmentation은 모든 데이터에 offset을 설정하며 Segment를 저장하는 레지스터로는 cs, ds, ss, es가 있습니다. 더 자세한 정보는 [https://wiki.osdev.org/Segmentation](https://wiki.osdev.org/Segmentation) 을 참조해보세요.

Segment을 계산하는 과정은 약간 특이합니다. Segment가 0x4d, 메모리 주소가 0x20이면, Segment을 반영한 메모리 주소는 (0x4d*0x10)+0x20=0x4f0 으로 계산합니다. 즉, Segment에 0x10을 곱하여 사용합니다. 따라서 0x7c00을 offset으로 사용하기 위해서는 0x7c0을 segment로 사용해야 합니다.

Segment를 표현하는 방법은 [es:변수 주소]입니다. es레지스터에 저장된 offset으로 변수의 실제 주소를 계산할 수 있습니다. 

### Disk 구조
먼저 디스크 구조를 쉽게 설명한 동영상을 첨부합니다.
[https://www.youtube.com/watch?v=gd5vKobmZ3Q](https://www.youtube.com/watch?v=gd5vKobmZ3Q)
디스크에 대해 필요한 지식은 다음과 같습니다.

* Sector: 디스크에 저장된 데이터는 512byte씩 sector로 저장됨
* Platter: 하드 디스크에 있는 하나의 디스크 판
* Head: head가 platter 표면 움직여 데이터 쓰거나 읽음
* Cylinder: 하드 디스크에서 여러 platter에 있는 같은 위치의 track을 모아서 cylinder라 함
* Track: sector의 모임 (하드 디스크 원판에서 한 둘레에 있음)
더 자세한 내용은 [https://jhnyang.tistory.com/105](https://jhnyang.tistory.com/105)을 참고하세요.

### Carry Bit
Carry bit는 나머지 bit가 다 차 있을 때 사용하는 남겨진 bit입니다. 사용 가능한 bit들이 모두 1로 차있는 상태에서 1을 더하면 Carry bit가 1 증가합니다. Carry bit를 쉽게 설명한 자료를 첨부합니다. https://www.quora.com/Can-you-explain-in-an-easy-way-what-is-a-carry-bit

### Int 0x13
디스크에 있는 데이터를 불러오기 위해서 직접 디스크를 조작할 필요없이 BIOS에 있는 기능을 쓰면 됩니다. 이를 가능하게 하는 것이 int 0x13 Interrupt입니다. ah레지스터를 0x02로 설정하면 디스크 'read' 모드에 들어가게 되고, cl 레지스터에 읽을 sector을 지정하며, ch 레지스터에는 cylinder 번호가 저장됩니다. dh 레지스터에는 head number가 저장되고, dl 레지스터에는 drive number가 들어가게 되는데, 이는 BIOS에 의해 disk number가 불러오게 되며, dl레지스터에 저장되는 값에 따라 읽는 디스크는 다음과 같습니다.

* 0 = floppy
* 1 = floppy2
* 0x80 = hdd
* 0x81 = hdd2
int 0x13 Interrupt가 발생하게 되면 es:bx 가 가리키는 위치에 데이터가 저장되게 됩니다. 따라서 Interrupt를 발생시키기 전에 bx 레지스터 데이터를 load 할 주소를 저장해두어야 합니다. 만약 Interrupt 발생 후 디스크를 읽는 과정에서 에러가 발생하면 Carry bit에 값이 저장됩니다. jc 명령어를 이용해 Carry bit에 데이터가 저장될 경우 error가 발생한 것을 알 수 있습니다. ah 레지스터에는 이때의 error code가 저장되게 됩니다.

디스크를 읽는 과정을 완료하면 BIOS는 al 레지스터에 읽은 sector의 번호를 반환하게 됩니다. 이를 통해 원하는 sector를 제대로 읽었는지 확인할 수 있습니다.

### 32 bit protected mode
Protected 모드는 80286 이후의 인텔 CPU에서 사용하는 Opertating 모드입니다. 32bit protected 모드를 통해 가상 메모리 주소를 사용할 수 있으며 메모리와 하드웨어의 IO보호 기능을 사용할 수 있습니다. 자세한 내용은 다음 페이지를 참고하세요. [https://wiki.osdev.org/Protected_Mode](https://wiki.osdev.org/Protected_Mode)

### VGA Memory
이번 강의부터는 BIOS Interrupt를 사용하지 않고 VGA 메모리를 설정해 화면에 문자열을 출력합니다. VGA 메모리는 0xb8000부터 시작하고, VGA 메모리에서는 문자를 2byte로 저장합니다. 2byte는 해당 문자의 ASCII코드와 색 등의 특성을 지니는 byte로 구성돼있습니다. 만약 row행 col열 픽셀에 특정 문자를 출력하고 싶으면 0xb8000 + 2*(row*(화면 세로 크기) + col)에 문자를 지정하면 됩니다.

### Assembly 상수 선언
Assembly 에서 상수를 지정하는 방법은 다음과 같습니다.
```assembly
<상수명> equ <값>
```

### GDT
GDT (Global Descriptor Table)은 메모리의 Segment와 Protected 모드의 구성 요소를 담은 메모리에 존재하는 자료구조입니다. 다음 블로그에 GDT에 대해 잘 설명되어 있으며 [https://itguava.tistory.com/14](https://itguava.tistory.com/14) University of Birmingham의 Nick Blundell이 쓰신 Writing a Simple Operating System-from Scratch도 꼭 참고해보시길 바랍니다.

Boot Sector Segmentation 강의에서는 16bit offset이 메모리를 다루게 하는 한계를 극복하기 위해 Segment를 사용했습니다. 즉, 16bit에서 0x4fe56과 같은 메모리 주소를 segment를 사용하지 않을 경우, 해당 메모리 주소를 16bit로 표현할 수 없는데 segment를 다음과 같이 이용하면:
```assembly
mov bx, 0x4000
mov es, bx
mov [es:0xfe56], ax
```
0x4fe56를 표현할 수 있었습니다.

32 bit 모드에서도 segmenting 이용해 접근 가능한 메모리의 범위를 넓혀주지만, 이를 구현하는 형식은 완전히 다릅니다. 16bit 에서 단순히 segment 레지스터에 저장된 값에 16을 곱한 후 메모리 주소에 더해주는 것이 아니라, segment 레지스터는 GDT안의 Segment Descriptor (SD)에 대한 인덱스가 됩니다. Segment Descriptor는 다음과 같은 구성 요소를 가집니다.

* Base Address (32bits): Segment가 시작되는 물리적 주소
* Segment Limit (20bits): Segment의 크기를 의미합니다
* 기타 Flag, Privilige 등 CPU가 Segment를 해석하는데 필요한 정보

실제로 Segment Desciptor에서 Base Address와 Segment Limit은 연속적으로 저장되지 않고 조각으로 나누어져 저장되어 있습니다. 또한 CPU는 GDT의 초기값이 Null Descriptor, 즉 8byte가 모두 0으로 되어 있도록 요구합니다. 이를 통해 protected 모드에 진입한 후 Segment Descriptor의 설정을 잊은 상태에서 메모리를 접근할 시 CPU가 에러를 발생시키게 해 문제점을 찾아 해결할 수 있습니다.

Code Segment의 Segment Descriptor은 다음과 같습니다.

* Base: 0x0
* Limit: 0xffff
* Present: 1 (메모리에 Segment가 존재함을 의미, 가상 메모리에 사용됨)
* Privilige: 0 (0이 가장 높은 권한)
* Descriptor Type: 1 (1은 Code나 Data Segment을 의미)
* Type
    * Code: 1
    * Conforming: 0 (0으로 설정함으로써 더 낮은 권한을 가진 Segment에서는 해당 Segment의 코드를 실행할 수 없게 함)
    * Readable: 1 (Readable 하면 코드에 정의된 모든 상수를 읽을 수 있음)
    * Accessed: 0 (디버깅에 사용됨)
* 기타 Flag
    * Granularity: 1 (Limit에 16x16x16을 곱함, 즉 0xffff은 0xffff0000으로 됨)
    * 32-bit default: 1
    * 64-bit code segment: 0
    * AVL: 0 (디버깅 등에 사용)
Data Segment의 Segment Descriptor은 Code Segment와 같은 구성요소와 내용을 가지고 있지만 Flag부분만 다릅니다.

* 기타 Flag
    * Code: 0
    * Expand down: 0 (어려운 내용)
    * Writable: 1
    * Accessed: 0
위와 같이 구성된 GDT를 CPU에서 접근할 때는 GDT Descriptor을 이용합니다. GDT Descriptor는 6byte 크기의 구조로

* GDT Size (16bits)
* GDT Address (32bits)
을 포함하고 있습니다.

### Pipelining
이 [블로그](https://jokerkwu.tistory.com/120#:~:text=%ED%8C%8C%EC%9D%B4%ED%94%84%EB%9D%BC%EC%9D%B4%EB%8B%9D%EC%9D%B4%EB%9E%80%3F,%EB%A5%BC%20%EC%8B%A4%ED%96%89%ED%95%98%EB%8A%94%20%EA%B8%B0%EB%B2%95%EC%9D%B4%EB%8B%A4.)에서 Pipelining에 대해 알기 쉽게 설명해 놓았습니다. Pipelining은 하나의 명령어가 실행되는 도중 동시에 다른 명령어를 시작하는 방식으로 동시에 여러개의 명령어를 실행해 처리량을 올리는 것입니다.

### 32 bit mode로 진입하는 순서
32bit 모드로 진입하는 순서는 다음과 같습니다.

1. Interrupt 비활성화
2. GDT Load
3. CPU의 control 레지스터 cr0에 값 설정
4. CPU Pipeline Flush
5. Segment 레지스터 값 설정
6. Stack 설정
7. 32 bit 코드의 Label 실행

### Cross Compiler
Cross Compiler는 한 플랫폼에서 다른 플랫폼의 실행 파일을 만들어주는 컴파일러 입니다. 예를 들어 Linux환경에서 Embedded환경의 실행 파일을 만들 때 Cross Compiler를 이용해 실행 파일을 만들어줍니다.

### Linking
이 [블로그](https://jhnyang.tistory.com/40)에서 Linking에 대해 알기 쉽게 설명해 놓았습니다. A.cpp라는 파일을 컴파일하면 A.o라는 Object Code가 생성되는데, 이 Object Code는 기계어로 작성된 로직과 디버깅, Symbol 정보 등의 부가적인 정보들이 있는 파일입니다. 생성된 Object Code들은 Linker에 의해 Linking 되어 실행파일이 만들어집니다. 이 과정에서 여러 Object Code들을 하나로 합치고, 여기에 라이브러리 파일들을 합쳐 실행 파일을 만들게 됩니다.

### Kernel
Kernel은 하드웨어 바로 위 단계에서 작동하며 애플리케이션들의 CPU와 메모리 등의 리소스를 관리하는 메모리에 항상 상주하고 있는 프로그램입니다.  Kernel에는 여러 디바이스 드라이버들이 있어 블루투스나 파일 시스템 등을 사용하려면 Kernel을 이용해 접근하게 됩니다.

### ELF (Excuatble and Linkable Format)
ELF 포맷은 리눅스에서 사용하는 실행 파일의 형식입니다. ELF 포맷은 주로 리눅스에서 실행 파일, 공유 라이브러리, Object 파일, Coredump 파일, Kernel boot 이미지에 사용됩니다. 자세한 설명은 이 블로그를 참고하세요.

### Makefile
리눅스나 OSX의 Shell에서 Makefile이 있는 디렉토리에서 make 명령어를 실행하면 Makefile에 적힌 대로 파일 간의 종속 관계를 파악해 Shell에 명령어들이 순차적으로 실행됩니다. 이를 통해 컴파일을 쉽게 할 수 있습니다. Makefile에 대해서는 이 블로그에 자세히 설명되어있습니다.

Makefile에는 다양한 명령어들이 있지만, 본 강의에서는 다음 명령어들만 사용하겠습니다.

* target: dependency1, dependency2, ... : target 파일을 만들기 위해 필요한 구성요소 (dependency)
* command: 해당 target 파일을 만들기 위한 명령어
* all: Makefile에 파라미터가 입력되지 않았을 때 실행할 타겟절
* clean: make clean 명령어를 실행시키는 경우 make를 통해 만들어진 모든 파일을 삭제
* $@: Target 파일
* $<: 첫 번째 dependency
* $^: 모든 dependency

### Shell에서 파일 합치기
두 파일 bootsect.bin, kernel.bin을 이어 붙어 os-image.bin이라는 파일을 만들기 위한 명령어는 다음과 같습니다.
```bash
cat bootsect.bin kernel.bin > os-image.bin
```

### Monolithic/Micro Kernel
Monolithic Kernel은 애플리케이션을 제외한 모든 시스템 기능들이 커널의 각 영역에 들어가 있는 형태를 의미합니다. Linux Kernel이 Monolithic Kernel의 형태입니다.

반면 Micro Kernel은 Monolithic Kernel이 관리하던 시스템 기능들이 Kernel위에 서버의 형태로 존재하는 형태입니다. 이를 통해 하나의 시스템 서비스가 다운돼도 Kernel 전체에 문제가 생기지 않습니다. 대표적으로 OSX의 Darwin Kernel과 Windows의 Windows NT Kernel이 Micro Kernel입니다.

### I/O Programming
Input/Output을 담당하는 하드웨어 장치는  Controller Chip을 통해 CPU와 상호작용합니다. Controller Chip에는 CPU가 읽고 쓸 수 있는 레지스터들이 있으며, 해당 레지스터들의 상태로 장치에게 무엇을 해야 할지 명령하게 합니다. Controller의 레지스터들은 메모리의 I/O Address Space에 맵핑되어 있어 Assembly의 in, out 명령어를 통해 I/O Address에 데이터를 일고 쓸 수 있습니다.

예를 들어 Floppy Disk를 회전시키게 하는 DOR레지스터는 0x3F2 I/O Address에 맵핑되어 있는데, Floppy Disk을 회전시키기 위한 Assembly 코드는 다음과 같습니다.

```assembly
mov dx, 0x3f2
; DOR port의 값을 AL레지스터에 읽음
in al, dx
; motor bit 킴
or al, 00001000b
; DOR 레지스터 값 업데이트
out dx, al
```

### Inline Assembly
C Code에서 Assembly를 사용하기 위해서는 다음과 같은 Inline Assembly를 이용합니다.

```C
__asm__( "assembly code" : 출력변수 : 입력변수);
```
GCC 컴파일러에서는 지금까지 사용한 NASM과 달리 GAS 문법을 사용합니다. GAS에서는 target과 desitination operand가 바뀐 형태로 표기됩니다. 또한, ':'를 기준으로 출력 변수와 입력되는 변수를 넣어줄 수 있습니다. 실제 코드를 예를 들어보겠습니다.

```C
unsigned char result;
unsigned short port;
__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
```
 Inline Assembly에서는 레지스터를 표현하기 위해서는 %%al과 같이 사용하며, %%dx는 EDX레지스터를 의미합니다. "=a" (result)는 명령어 실행 후 AL레지스터의 값을 result변수에 대입하라는 의미이고, "d" (port)는 EDX레지스터의 값을 port변수 값으로 설정하라는 의미입니다. 즉, 위 코드의 의미는 다음과 같습니다.

1. EDX레지스터의 값을 port 변수 값으로 설정한다.
2. (NASM 기준) in al, edx: EDX레지스터가 가리키는 port의 값을 AL레지스터에 읽는다.
3. AL레지스터의 값을 result 변수에 저장한다.

### ISR (Interrupt Service Routine)
CPU가 A 작업을 하고 있다가 키보드 입력과 같은 Interrupt을 받으면 CPU는 현재 진행 중인 A 작업을 멈추고 Interrupt Sercie Routine으로 점프해 Interrupt에 관한 Routine을 처리합니다. Interrupt가 처리되면 원래 A 작업으로 돌아갑니다. 

### IDT (Interrupt Descriptor Table)
GDT와 매우 유사한 구조를 가지고 있는 IDT는 CPU에게 256개의 Interrupt에 대한 Handler을 찾게 하며, 중요한 첫 32개 Interrupt는 다음과 같습니다.

0 - Division by zero exception
1 - Debug exception
2 - Non maskable interrupt
3 - Breakpoint exception
4 - 'Into detected overflow'
5 - Out of bounds exception
6 - Invalid opcode exception
7 - No coprocessor exception
8 - Double fault (pushes an error code)
9 - Coprocessor segment overrun
10 - Bad TSS (pushes an error code)
11 - Segment not present (pushes an error code)
12 - Stack fault (pushes an error code)
13 - General protection fault (pushes an error code)
14 - Page fault (pushes an error code)
15 - Unknown interrupt exception
16 - Coprocessor fault
17 - Alignment check exception
18 - Machine check exception
19-31 - Reserved

### #ifndef, #define
복잡한 프로그램의 경우 여러 헤더 파일이 꼬여, 헤더 파일의 중복 선언이 일어날 수 있습니다. 이를 막기 위해 해당 헤더 파일이 이미 선언된 경우, 중복된 선언을 방지하기 위해 #ifndef, #define을 사용합니다. 

```C
#ifndef TYPES_H
#define TYPES_H

... 정의

#endif
```
위 코드(types.h)에서 만약 types.h가 이미 선언된 경우 재선언하지 않고, 선언되지 않는 경우 #define과 #endif 사이의 types.h의 내용을 선언합니다.

### 매크로 함수
매크로 함수는 #define 부분에 인자를 전달해 함수처럼 사용할 수 있는 매크로를 의미합니다. 
```C
#define SUM(X,Y) ((X)+(Y))
```
이때 매크로 함수의 인자는 어떤 타입이나 가능하며, 매크로 함수 내부에서 인자를 사용할 때는 괄호를 반드시 사용해야 합니다.

### __attribute__((packed))
```C
typedef struct {
    char c;
    int x;
} myStruct;
```
상식적으로, myStruct의 크기는 char 1byte + int 4byte = 5byte이어야 합니다. 하지만 실제로 sizeof(myStruct)는 8입니다. 이는 CPU가 메모리에 4byte 단위로 접근할 시 가장 속도가 빠르기 때문에, 4byte 단위로 데이터를 저장하기 때문입니다. 이때 char는 4byte 공간을 차지하게 되고, 1byte를 제외한 나머지 3byte는 의미 없는 데이터로 채워지게 됩니다.

```C
typedef struct {
    char c;
    int x;
} __attribute__((packed)) myStruct;
```
myStruct가 메모리를 낭비하지 않고 5byte가 되도록 하기 위해서는 __attribute__((packed)) 특성을 설정해 컴파일러가 구조체의 실제 크기만큼 메모리를 할당하게 합니다.

### __volatile__
__volatile__은 컴파일러가 코드를 최적화하지 않고, 있는 그대로 실행하게 합니다. 예를 들어, 컴파일러는 Inline Assembly에서 이후의 코드에서 사용하지 않는 변수를 변경하면, 해당 부분의 코드를 최적화해 수행하지 않습니다. __volatile__을 설정하면 이처럼 컴파일러에 의해 코드의 최적화가 발생하지 않습니다.

### lidtl
lidtl Assembly 명령어는 IDT를 로딩하는 명령어입니다. lidtl 명령어는 IDT Description Structure의 주소를 인자로 받습니다.

### Assembly에서 인자가 있는 함수 사용
Assembly에서 C 코드에 선언된 함수를 사용하기 위해서는 extern <함수 이름> 명령어로 해당 함수를 불러와 call <함수 이름> 명령어로 함수를 실행합니다. 이때 함수의 인자들은 Stack에 push 되어 전달됩니다. 즉, 다음과 같은 함수 func가 있을 때,
```C
func(arg1, arg2, arg3)
```
func의 인자들은 오른쪽 인자들부터 Stack에 push 되어 사용합니다.

### sti, cli
Assembly에서 sti는 Interrupt Flag (IF)를 IF=1인 상태로 만들고, cli는 IF=0인 상태를 만듭니다. IF=0이면 Interrupt가 발생하지 않게 억제되며, IF=1이면 Interrupt를 처리합니다.

### Computer Buses
버스는 데이터 버스, 주소 버스, 제어 버스로 나뉩니다.

* 데이터 버스: 데이터 버스는 데이터가 이동할 수 있도록 하며 32, 64개의 선들로 구성되어 있다.
* 주소 버스: 데이터의 출발지나 도착지의 메모리 주소를 전달한다.
* 제어 버스: 데이터 버스와 주소 버스를 제어한다.
    * Read
    * Write
    * Transfer Acknowledgment (데이터가 잘 보내졌는지)
    * Clock Signal (언제 데이터 보냈는지 표시)
데이터가 전송되는 방식에는 Serial, Parallel 방식이 있습니다. 현대 CPU의 대부분의 경우 Serial 방식을 사용합니다.

* Serial: 데이터가 순차적으로 하나의 Channel을 통해 전송된다.
* Parallel: 데이터가 여러 Channel을 통해 동시에 전송된다. 이때 각 Channel을 통해 들어오는 데이터가 동시에 도달해야 한다.

### IRQ (Interrupt Request)
IRQ는 컴퓨터의 주변 기기가 CPU에게 어떤 이벤트가 발생했다는 것을 알리는 line입니다. 즉, CPU가 어떤 일이 처리하다가도 주변 기기에서 Interrupt가 들어오면 하던 일을 중단하고 해당 Interrupt을 처리하게 됩니다. IRQ 번호에 따른 Interrupt 내용은 다음과 같습니다.

0. IRQ 타이머
1. 키보드
2. IRQ 캐스 케이드 공유
3. 시리얼포트 2, COM2, COM4 ( 모뎀 / 마우스)
4. 시리얼포트 1, COM1, COM3 ( 모뎀 / 마우스)
5. LPT2 병렬포트 혹은
6. FDD 컨트롤러
7. LPT1 병열 포트 등 혹은 사운드카드
8. RTC( Real Time Clock)
9. 예비, 주로 미디 카드(MPU401)에서 사용 , SOUND ,VGA . USB, MPEG II
10. 예비 , SOUND ,VGA . USB,MPEG II,
11. 예비, SCSI 아답터, SOUND ,VGA . USB,MPEG II
12. PS/2 마우스
13. 코프로세서 (수치처리보조연산자 )
14. IDE 하드 컨트롤러 Primary
15. IDE 하드 컨트롤러 Secondary

### PCI (Peripheral Component Interconnect)
PCI는 CPU와 컴퓨터 주변기기를 연결하는 Local Bus입니다. PCI는 Bridge, Master, Slave으로 구분됩니다.

* Bridge: 시스템과 PCI 버스를 연결해준다
* Master: 버스 전송을 초기화하고 주소, 데이터 전송, 제어 신호들을 조정한다
* Slave: 마스터에 의존하는 수동적인 메모리 디바이스이다

CPU가 boot 되면 PIC는 IRQ 0-7을 INT 0x8-0xF에, IRQ 8-15을 INT 0x70-0x77에 맵핑합니다. 지난 강의들에서 ISR를 0-31에 맵핑하였으므로 IRQ를 ISR 32-47에 맵핑해야 겹치지 않습니다.

PIC는 I/O port을 통해 통신하고, 이때 Master PIC는 0x20을 통해 명령, 0x21을 통해 데이터를 처리합니다. Slave PIC는 0xA0을 통해 명령, 0xA1을 통해 데이터를 처리합니다.

### PIT (Programmable Interval Timer)
PIT chip(문서)은 oscillator, prescaler, frequency dividers로 구성되며, frequency divider은 Timer가 IRQ0와 같은 외부 Circuitry를 제어할 수 있게 합니다. PIT의 oscillator는 약 1.193180 MHz로 작동하는데, freqeuncy divider는 이 진동수를 divider로 나누어서 더 느린 진동수로 작동하게 합니다. 입력 진동수에 따라 펄스가 발생할 때마다 counter를 감소시키고, counter가 0이 되면 counter가 리셋됩니다. 예를 들어 입력 진동수가 200Hz이고, counter가 10번마다 리셋되면, 출력 진동수는 200/10=20Hz가 되는 것입니다. PIT의 frequency divider는 크기가 16bit이며, 0부터 65535 사이의 값을 가질 수 있습니다.

PIT의 출력은 3개의 Channel로 구성되며, 이중 Channel 0는 IRQ0에 연결됩니다. PIT chip의 command 레지스터는 0x43 I/O port이며, Channel 0의 data  I/O port는 0x40이며, frequency divider를 넘겨줍니다. 0x43 port에는 다음 데이터를 통해 명령을 줄 수 있습니다.

### Keyboard Interrupt
키보드의 Interrupt은 IRQ1을 통해 처리합니다. 키보드 입력이 주어지면 PIC는 key가 눌리거나(key down) 땠을 때의(key up) scancode를 0x60 I/O port에 저장합니다. 이때 key up의 scancode는 key down의 scancode + 0x80의 값을 가집니다. 본 강의에서는 입력으로 주어진 scancode를 ASCII code로 변환해 출력하는 것을 구현합니다.

### Paging 기법
컴퓨터는 메모리를 가상 메모리로서 관리하는데, 이를 통해 실제 메모리를 추상화해 프로세스들이 연속된 메모리를 사용하는 것처럼 만들 수 있게 합니다. 이때, 가상 메모리와 실제 메모리를 매핑하는 데에 Paging 기법이 사용됩니다. Paging 기법은 가상 메모리의 공간을 일정한 크기로 나눈 블록인 Page을 단위로 사용합니다. 실제 메모리도 Page와 같은 크기로 나눈 블록인 Frame 단위도 사용되며, Page가 하나의 Frame을 할당받습니다. 이 과정에서 Page는 프로세스의 Page 정보와 Page에 대한 Frame의 시작 주소를 값으로 하는 Page Table형태로 관리됩니다.

## 참고사항
본 자료는 제가 공부하면서 작성한 것이므로 오개념이 있을 수 있습니다. 잘못된 점을 발견해주신 분은 [티스토리](https://jschang.tistory.com/category/OS/OS%20from%20Scratch) 댓글로 알려주시면 감사하겠습니다.
