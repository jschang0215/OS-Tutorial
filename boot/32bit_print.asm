; 32bit protected mode사용 
[bits 32]

; Assembly에서 상수 선언 방법
; VGA 메모리는 0xb8000부터 시작 
VIDEO_MEMORY equ 0xb8000
; 각 문자에 대한 색깔 지정하는 byte
WHITE_ON_BLACK equ 0x0f

print_string_pm:
    pusha
    mov edx, VIDEO_MEMORY

print_string_pm_loop:
    ; [ebx]에 출력할 문자 저장되어 있음
    mov al, [ebx]
    mov ah, WHITE_ON_BLACK

    ; 문자열의 끝인지 확인
    cmp al, 0x00
    je print_string_pm_done

    ; 출력할 문자와 색 정보 VGA 메모리에 저장
    mov [edx], ax
    ; 문자열에서 다음 문자
    add ebx, 1
    ; VGA 메모리는 2byte 단위로 저장됨 
    add edx, 2

    jmp print_string_pm_loop

print_string_pm_done:
    popa
    ret