; 문자열 출력
print:
    pusha

start:
    ; bx가 문자열 시작 주소 
    mov al, [bx]
    cmp al, 0
    je done

    mov ah, 0x0e
    int 0x10

    add bx, 1
    jmp start

done:
    popa
    ret

; 줄 바꾸기 
print_nl:
    pusha

    mov ah, 0x0e
    ; newline 문자 
    mov al, 0x0a
    int 0x10
    ; carriage return 문자 
    mov al, 0x0d
    int 0x10

    popa
    ret
