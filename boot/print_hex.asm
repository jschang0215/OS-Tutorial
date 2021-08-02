print_hex:
    pusha

    ; cx loop의 인덱스
    mov cx, 0

hex_loop:
    cmp cx, 4
    je end

    ; dx에 입력 받음 
    mov ax, dx
    ; 마지막 문자만 남김
    and ax, 0x000f
    ; 0의 ASCII 값은 0x30, 0x30 더해야 함
    add al, 0x30
    ; 'A'의 ASCII 값은 0x41
    cmp al, 0x39
    jle step2
    ; 9 이상의 값보다 크면 7더해야 함
    add al, 7

step2:
    ; 뒤에서 부터 채워나가기 때문
    mov bx, HEX_OUT+5
    ; 문자의 인덱스 계산
    sub bx, cx
    ; bx가 가리키는 곳에 al에 저장된 문자 넘김
    mov [bx], al
    ; ex 0x1234 -> 0x4123 (한칸씩 순환)
    ror dx, 4

    add cx, 1
    jmp hex_loop

end:
    ; boot_sect_print_function의 print함수 사용
    mov bx, HEX_OUT
    call print

    popa
    ret

HEX_OUT:
    db '0x0000', 0