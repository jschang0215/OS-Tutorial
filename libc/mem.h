#ifndef MEM_H
#define MEM_H

#include <stdint.h>
// size_t: 크기 저장하는 수 (unsigned int와 동일)
#include <stddef.h>

void memory_copy(uint8_t *source, uint8_t *dest, int nbytes);
void memory_set(uint8_t *dest, uint8_t val, uint32_t len);

uint32_t kmalloc(uint32_t size, size_t align, uint32_t *phys_addr);

#endif