#include <stdint.h>

#include <limine.h>

#include <mcsos/arch/cpu.h>
#include <mcsos/arch/idt.h>

#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>
#include <mcsos/kernel/version.h>

#include <pic.h>
#include <pit.h>

__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);

extern char __kernel_start[];
extern char __kernel_end[];

static void m4_selftest(void) {
    KERNEL_ASSERT(
        __kernel_end > __kernel_start
    );

    KERNEL_ASSERT(
        sizeof(uintptr_t) == 8u
    );

    KERNEL_ASSERT(
        sizeof(x86_64_idt_entry_t) == 16u
    );

    KERNEL_ASSERT(
        x86_64_idt_base_for_test() != 0u
    );

    KERNEL_ASSERT(
        x86_64_idt_limit_for_test() == 4095u
    );

    log_writeln(
        "[M4] selftest: IDT invariants passed"
    );
}

void kmain(void) {
    cpu_cli();

    log_init();

    log_write(MCSOS_NAME);
    log_write(" ");
    log_write(MCSOS_VERSION);
    log_write(" ");
    log_write(MCSOS_MILESTONE);
    log_writeln(" kernel entered");

    log_key_value_hex64(
        "kernel_start",
        (uint64_t)(uintptr_t)__kernel_start
    );

    log_key_value_hex64(
        "kernel_end",
        (uint64_t)(uintptr_t)__kernel_end
    );

    log_key_value_hex64(
        "rflags_before_idt",
        cpu_read_rflags()
    );

    /*
     * IDT must exist before interrupts.
     */
    x86_64_idt_init();

    m4_selftest();

    /*
     * PIC setup
     */
    log_writeln(
        "[M5] remapping PIC"
    );

    pic_remap(
        PIC_MASTER_OFFSET,
        PIC_SLAVE_OFFSET
    );

    /*
     * Safe default:
     * mask everything first.
     */
    pic_mask_all();

    /*
     * PIT timer at 100 Hz
     */
    log_writeln(
        "[M5] configuring PIT"
    );

    pit_configure_hz(100u);

    /*
     * Only IRQ0 enabled.
     */
    pic_unmask_irq(0u);

    log_key_value_hex64(
        "pic_master_mask",
        pic_read_master_mask()
    );

#ifdef MCSOS_M4_TRIGGER_BREAKPOINT
    log_writeln(
        "[M4] triggering intentional breakpoint exception"
    );

    x86_64_trigger_breakpoint_for_test();

    log_writeln(
        "[M4] returned from breakpoint handler"
    );
#endif

#ifdef MCSOS_M4_TRIGGER_PANIC
    KERNEL_PANIC(
        "intentional M4 panic test",
        0x4D43534F533034u
    );
#else
    log_writeln(
        "[M5] enabling interrupts"
    );

    /*
     * Interrupts enabled ONLY after:
     * - IDT ready
     * - PIC remapped
     * - PIT configured
     * - IRQ0 unmasked
     */
    cpu_sti();

    log_writeln(
        "[M5] timer IRQ online"
    );

    log_writeln(
        "[M5] entering idle halt loop"
    );

    for (;;) {
        cpu_hlt();
    }
#endif
}
