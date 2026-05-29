#include <stdint.h>

#include <mcsos/arch/idt.h>

#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>

#include "pic.h"
#include "pit.h"

typedef struct {
    const char *name;
    uint8_t recoverable;
} m4_exception_info_t;

static const m4_exception_info_t exception_info[32] = {
    { "#DE Divide Error", 0 },
    { "#DB Debug", 1 },
    { "NMI Interrupt", 0 },
    { "#BP Breakpoint", 1 },
    { "#OF Overflow", 1 },
    { "#BR Bound Range Exceeded", 0 },
    { "#UD Invalid Opcode", 0 },
    { "#NM Device Not Available", 0 },
    { "#DF Double Fault", 0 },
    { "Coprocessor Segment Overrun", 0 },
    { "#TS Invalid TSS", 0 },
    { "#NP Segment Not Present", 0 },
    { "#SS Stack Segment Fault", 0 },
    { "#GP General Protection Fault", 0 },
    { "#PF Page Fault", 0 },
    { "Reserved", 0 },
    { "#MF x87 Floating-Point Exception", 0 },
    { "#AC Alignment Check", 0 },
    { "#MC Machine Check", 0 },
    { "#XM SIMD Floating-Point Exception", 0 },
    { "#VE Virtualization Exception", 0 },
    { "#CP Control Protection Exception", 0 },
    { "Reserved", 0 },
    { "Reserved", 0 },
    { "Reserved", 0 },
    { "Reserved", 0 },
    { "Reserved", 0 },
    { "Reserved", 0 },
    { "#HV Hypervisor Injection Exception", 0 },
    { "#VC VMM Communication Exception", 0 },
    { "#SX Security Exception", 0 },
    { "Reserved", 0 }
};

static uint64_t trap_count;
static uint64_t trap_vector_count[256];

static uint64_t unexpected_irq_count;

uint64_t m4_trap_count_for_test(void) {
    return trap_count;
}

uint64_t m4_trap_vector_count_for_test(uint64_t vector) {
    if (vector >= 256u) {
        return 0u;
    }

    return trap_vector_count[vector];
}

static void log_trap_frame(
    const x86_64_trap_frame_t *frame
) {
    log_key_value_hex64("trap_vector", frame->vector);
    log_key_value_hex64("trap_error", frame->error_code);

    log_key_value_hex64("trap_rip", frame->rip);
    log_key_value_hex64("trap_cs", frame->cs);
    log_key_value_hex64("trap_rflags", frame->rflags);

    log_key_value_hex64("trap_rax", frame->rax);
    log_key_value_hex64("trap_rbx", frame->rbx);
    log_key_value_hex64("trap_rcx", frame->rcx);
    log_key_value_hex64("trap_rdx", frame->rdx);

    log_key_value_hex64("trap_r8", frame->r8);
    log_key_value_hex64("trap_r9", frame->r9);
    log_key_value_hex64("trap_r10", frame->r10);
    log_key_value_hex64("trap_r11", frame->r11);
    log_key_value_hex64("trap_r12", frame->r12);
    log_key_value_hex64("trap_r13", frame->r13);
    log_key_value_hex64("trap_r14", frame->r14);
    log_key_value_hex64("trap_r15", frame->r15);
}

void x86_64_trap_dispatch(
    x86_64_trap_frame_t *frame
) {
    KERNEL_ASSERT(
        frame != (x86_64_trap_frame_t *)0
    );

    ++trap_count;
    ++trap_vector_count[frame->vector];

/*
 * PIC IRQ range
 * 32..47
 */
if (frame->vector >= 32u &&
    frame->vector <= 47u) {

    uint8_t irq =
        (uint8_t)(frame->vector - 32u);

    if (irq == 0u) {

        timer_on_irq0();

    } else {

        ++unexpected_irq_count;

        if ((unexpected_irq_count % 100u) == 0u) {

            log_key_value_hex64(
                "unexpected_irq_count",
                unexpected_irq_count
            );
        }
    }

    pic_send_eoi(irq);

    return;
}
    /*
     * CPU exception path
     */
    log_write("[M5] trap dispatch: ");

    log_writeln(
        exception_info[frame->vector].name
    );

    log_write("recoverable=");

    log_writeln(
        exception_info[frame->vector].recoverable
            ? "yes"
            : "no"
    );

    log_trap_frame(frame);

    /*
     * Breakpoint remains recoverable
     */
    if (frame->vector == 3u) {

        log_writeln(
            "[M5] breakpoint handled; returning with iretq"
        );

        return;
    }

    /*
     * Everything else is fatal
     */
    KERNEL_PANIC(
        "unrecoverable CPU exception",
        frame->vector
    );
}
