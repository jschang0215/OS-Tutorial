disk_load:
    pusha
    push dx

    ; int 0x13 함수 0x02는 'read'
    mov ah, 0x02
    ; 읽을 sector 개수 
    mov al, dh
    ; cl은 sector 
    ; 0x01이 boot sector
    ; 0x02가 첫번째 sector 
    mov cl, 0x02

    ; ch는 cylinder 
    mov ch, 0x00

    ; dl은 drive number
    ; (0 = floppy, 1 = floppy2, 0x80 = hdd, 0x81 = hdd2)
    ; caller에서 BIOS에 의해 dl가 설정

    ; dh는 head number
    mov dh, 0x00

    ; [es:bx] 는 데이터가 저장될 곳에 대한 포인터
    ; caller가 설정

    ; BIOS Interrupt
    int 0x13
    ; Error 
    ; Carry Bit에 저장되는 경우
    jc disk_error

    pop dx
    ; BIOS는 al에 읽은 sector 개수 저장
    ; 이 value을 처음에 읽을 개수로 저장한 dh와 비교
    cmp al, dh
    jne sectors_error
    popa 
    ret

disk_error:
    mov bx, DISK_ERROR
    call print
    call print_nl
    ; ah에 에러 코드 저장
    ; http://stanislavs.org/helppc/int_13-1.html 에 에러 코드
    mov dh, ah
    call print_hex
    jmp disk_loop

sectors_error:
    mov bx, SECTORS_ERROR
    call print

disk_loop:
    jmp $

DISK_ERROR:
    db 'Disk read error', 0x00
SECTORS_ERROR:
    db 'Incorrect number of sectors read', 0x00
