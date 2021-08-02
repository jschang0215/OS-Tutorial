#ifndef TYPES_H
#define TYPES_H

// 데이터 크기에 맞는 선언문

#define low_16(address) (uint16_t)((address) & 0xffff)
#define high_16(address) (uint16_t)((address) >> 16 & 0xffff)

#endif