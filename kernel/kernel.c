#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "../libc/string.h"
#include "../libc/mem.h"
// uint32_t: unsigned int
#include <stdint.h> 

void kernel_main() {
    isr_install();
    irq_install();

    kprint("Type Command (End to halt):\n");
}

void user_input(char *input) {
    if(strcmp(input, "END")==0) {
        kprint("Stop System...\n");
        // hlt: halt CPU
        asm volatile("hlt");
    } else if(strcmp(input, "PAGE")==0) {
        uint32_t phys_addr;
        uint32_t page = kmalloc(1000, 1, &phys_addr);
        char page_str[16] = "";
        hex_to_ascii(page, page_str);
        char phys_str[16] = "";
        hex_to_ascii(phys_addr, phys_str);
        kprint("Page: ");
        kprint(page_str);
        kprint(", Physical Address: ");
        kprint(phys_str);
        kprint("\n");
    }
    kprint("> Typed: ");
    kprint(input);
    kprint("\n");
}