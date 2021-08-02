#include "timer.h"
#include "isr.h"
#include "ports.h"
#include "../libc/function.h"

uint32_t tick = 0;

static void timer_callback(registers_t *regs) {
    tick++;
    UNUSED(regs);
}

void init_timer(uint32_t freq) {
    // Timer는 IRQ0 이용 
    // timer_callback IRQ0에 설치
    register_interrupt_handler(IRQ0, timer_callback);

    // PIT reload value 설정
    // divisor가 0이 되면 reload 됨
    uint32_t divisor = 1193180 / freq;
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

    // 명령 전송
    // 0x43: Command Port
    // 0x36=00110110b: 16bit (Channel0, Access Mode: lo/hi byte
    // , Operating Mode: square wave generator, 16bit-binary)
    port_byte_out(0x43, 0x36);
    // 0x40: Channel 0 data port
    // PIT reload value의 하위 byte
    port_byte_out(0x40, low);
    // PIT reload value의 상위 byte
    port_byte_out(0x40, high);
}