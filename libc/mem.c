#include "mem.h"
#include "../drivers/screen.h"

// 메모리 복사 (stdlib의 memcpy)
void memory_copy(uint8_t *source, uint8_t *dest, int nbytes) {
    int i;
    for(i=0; i<nbytes; i++) {
        *(dest + i) = *(source + i); 
    }
}

void memory_set(uint8_t *dest, uint8_t val, uint32_t len) {
    uint8_t *tmp = (uint8_t *) dest;
    int i;
    for(i=0; i<(int)len; i++) {
        *tmp++ = val;
    }
}

// kernel은 0x1000부터 시작
uint32_t free_mem_addr = 0x10000;

uint32_t kmalloc(uint32_t size, size_t align, uint32_t *phys_addr) {
    // 0x1000단위로 할당
    if(align==1 && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    if(phys_addr) {
        *phys_addr = free_mem_addr;
    }
    uint32_t ret = free_mem_addr;
    free_mem_addr += size;
    return ret;
}