#ifndef IDT_H
#define IDT_H

#include "../cpu/types.h"
#include <stdint.h>

// Segment Selector
#define KERNEL_CS 0x08

// 모든 Interrupt의 구조
typedef struct {
    // Handler 함수의 하위 16bit 주소 
    uint16_t low_offset;
    // Kernel Segment Selector
    uint16_t sel;
    // 무조건 0
    uint8_t always0;
    // 7번 bit: Interrupt 존재
    // 5-6번 bit: caller의 권한 (0=kernel, ..., 3=user)
    // 4번 bit: 0=Interrupt Gate
    // 3-0 bit: 1110 = 32bit Interrupt Gate
    uint8_t flags;
    // Handler 함수의 상위 16bit 주소 
    uint16_t high_offset;
} __attribute__((packed)) idt_gate_t;

// Interrupt Handler의 배열에 대한 포인터
// Assembly의 lidt으로 읽음
typedef struct {
    // 크기
    uint16_t limit;
    // 주소
    uint32_t base;
} __attribute__((packed)) idt_register_t;

// 256아니면 CPU 패닉
#define IDT_ENTRIES 256
idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void set_idt_gate(int n, uint32_t handler);
void set_idt();

#endif