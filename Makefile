# kernel/<파일명>.c, drivers/<파일명>.c 파일들을 매칭
# 파일명들을 띄어쓰기로 구분된 문자열 생성 
C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)
# C_SOURCES의 파일명에서 마지막 .c대신 .o으로 대체
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o}

CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb

# gcc의 flag
# 외부 코드 포함 안하게 함
# warning을 에러로 표시하게 함
CFLAGS = -g -ffreestanding -Wall -Wextra -fno-exceptions -m32

all: run

os-image.bin: boot/bootsect.bin kernel.bin
	cat $^ > os-image.bin

kernel.bin: boot/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

# 디버깅 위해 생성
kernel.elf: boot/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^

run: os-image.bin
	qemu-system-i386 -drive file=os-image.bin,format=raw,index=0,if=floppy,media=disk

debug: os-image.bin kernel.elf
	qemu-system-i386 -s -fda os-image.bin -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# %.o: %.c .o로 끝나는 파일와 동일한 파일명의 .c파일
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o
