#include <stdint.h>

#include "io.h"
#include "pit.h"

#include "mcsos/kernel/log.h"

#define PIT_CHANNEL0_DATA 0x40
#define PIT_COMMAND       0x43

#define PIT_COMMAND_MODE3 0x36

static volatile uint64_t g_ticks = 0;

uint64_t timer_ticks(void) {
    return g_ticks;
}
void timer_wait_ticks(uint64_t delta) {
    uint64_t target =
        timer_ticks() + delta;

    while (timer_ticks() < target) {
        __asm__ volatile ("hlt");
    }
}
void pit_configure_hz(uint32_t hz) {
    if (hz == 0) {
        hz = 100;
    }

    uint16_t divisor =
        (uint16_t)(PIT_BASE_FREQUENCY_HZ / hz);

    outb(PIT_COMMAND, PIT_COMMAND_MODE3);

    outb(PIT_CHANNEL0_DATA,
          (uint8_t)(divisor & 0xFF));

    outb(PIT_CHANNEL0_DATA,
          (uint8_t)((divisor >> 8) & 0xFF));
}

void timer_on_irq0(void) {
    g_ticks++;

    if ((g_ticks % 100u) == 0u) {

        log_write("[MCSOS:TIMER] ticks=");

        log_key_value_hex64(
            "count",
            g_ticks
        );
    }
}
