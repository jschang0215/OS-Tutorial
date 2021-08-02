gdt_start:
    ; GDT는 8byte Null로 시작함 
    dd 0x0
    dd 0x0

; GDT Code Segment
gdt_code:
    ; Segment Limit
    ; 0-15 bit
    dw 0xffff
    ; Segement Base
    ; 16-31 bit
    dw 0x0
    ; Segment Base
    ; 0-7 bit
    db 0x0
    ; flag 8bit
    db 10011010b
    ; flag 4bit + Segment Limit 16-19
    db 11001111b
    ; Segment Base
    ; 24-31 bit
    db 0x0

; Code Segment와 base와 segment limit 동일 
; flag 만 다름 
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0 
    ; flag 
    db 10010010b
    ; flag + Segment Limit 
    db 11001111b
    db 0x0 

gdt_end:

gdt_descriptor:
    ; GDT Size 
    ; 실제 크기는 gdt_end gdt_start 사이 크기이므로 1 뺀다
    dw gdt_end - gdt_start - 1
    ; GDT Address
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start