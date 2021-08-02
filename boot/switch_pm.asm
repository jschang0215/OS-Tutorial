[bits 16]
switch_to_pm:
    ; 1. Diasble Interrupt 
    cli
    ; 2. Load GDT Descriptor 
    lgdt [gdt_descriptor]
    ; 3. cr0 레지스터를 32bit 모드로 설정 
    mov eax, cr0
    or eax, 0x1 
    mov cr0, eax 
    ; 4. far jump로 pipeline flush
    jmp CODE_SEG:init_pm

[bits 32]
init_pm:
    ; 5. Segment 레지스터 설정 
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax

    ; 6. Stack 설정 
    mov ebp, 0x90000
    mov esp, ebp

    ; 7. 32bit 모드의 Label 호출 
    call BEGIN_PM