#ifndef MCSOS_ARCH_CPU_H
#define MCSOS_ARCH_CPU_H

#include <stdint.h>

static inline void cpu_cli(void) {
    __asm__ volatile ("cli" ::: "memory");
}

static inline void cpu_sti(void) {
    __asm__ volatile ("sti" ::: "memory");
}

static inline void cpu_hlt(void) {
    __asm__ volatile ("hlt");
}

static inline uint64_t cpu_read_rflags(void) {
    uint64_t rflags;

    __asm__ volatile (
        "pushfq\n\t"
        "popq %0"
        : "=r"(rflags)
        :
        : "memory"
    );

    return rflags;
}

__attribute__((noreturn))
static inline void cpu_halt_forever(void) {
    cpu_cli();

    for (;;) {
        cpu_hlt();
    }
}

#endif
