global _start
[bits 32]

_start:
    [extern kernel_main] ; kernel.c kernel_main 함수
    call kernel_main ; C함수 호출
    jmp $