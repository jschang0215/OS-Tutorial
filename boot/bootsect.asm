[org 0x7c00]
KERNEL_OFFSET equ 0x1000

; BIOS는 boot drive를 dl 레지스터에 저장
mov [BOOT_DRIVE], dl

mov bp, 0x9000
mov sp, bp

mov bx, MSG_REAL_MODE
call print
call print_nl

; Disk에서 kernel read
call load_kernel
; Interrupt disable, GDT load
call switch_to_pm

%include "boot/print.asm"
%include "boot/print_hex.asm"
%include "boot/disk.asm"
%include "boot/gdt.asm"
%include "boot/32bit_print.asm"
%include "boot/switch_pm.asm"

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print
    call print_nl

    mov bx, KERNEL_OFFSET
    mov dh, 48
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
BEGIN_PM:
    mov ebx, MSG_PROT_MODE
    call print_string_pm
    
    ; Kernel에게 넘겨줌
    call KERNEL_OFFSET
    
    jmp $

; dl레지스터가 덮어씌어질 수 있으므로 메모리에 저장
BOOT_DRIVE db 0

MSG_REAL_MODE db "Started in 16-bit Real Mode", 0x00
MSG_PROT_MODE db "Landed in 32-bit Protected Mode", 0x00
MSG_LOAD_KERNEL db "Loading Kernel into Memory", 0x00

times 510-($-$$) db 0
dw 0xaa55