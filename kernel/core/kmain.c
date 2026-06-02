#include <stdint.h>

#include <limine.h>

#include <mcsos/arch/cpu.h>
#include <mcsos/arch/idt.h>

#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>
#include <mcsos/kernel/pmm.h>
#include <mcsos/kernel/version.h>

#include <pic.h>
#include <pit.h>

__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);
__attribute__((used, section(".requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};
extern char __kernel_start[];
extern char __kernel_end[];

static struct pmm_state kernel_pmm;

static uint8_t kernel_pmm_bitmap[PMM_BITMAP_BYTES]
    __attribute__((aligned(4096)));

static uint32_t limine_to_boot_type(
    uint64_t type
) {
    switch (type) {
    case LIMINE_MEMMAP_USABLE:
        return BOOT_MEM_USABLE;

    case LIMINE_MEMMAP_RESERVED:
        return BOOT_MEM_RESERVED;

    case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
        return BOOT_MEM_BOOTLOADER_RECLAIMABLE;

#if LIMINE_API_REVISION >= 2
    case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
#else
    case LIMINE_MEMMAP_KERNEL_AND_MODULES:
#endif
        return BOOT_MEM_KERNEL_AND_MODULES;

    case LIMINE_MEMMAP_FRAMEBUFFER:
        return BOOT_MEM_FRAMEBUFFER;

    case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
        return BOOT_MEM_ACPI_RECLAIMABLE;

    case LIMINE_MEMMAP_ACPI_NVS:
        return BOOT_MEM_ACPI_NVS;

    case LIMINE_MEMMAP_BAD_MEMORY:
        return BOOT_MEM_BAD_MEMORY;

    default:
        return BOOT_MEM_RESERVED;
    }
}

static void m6_memory_init(void) {
    struct limine_memmap_response *resp =
        memmap_request.response;

    if (resp == NULL) {
        KERNEL_PANIC(
            "limine memmap missing",
            0x4D360001u
        );
    }

    struct boot_mem_region regions[256];

    uint64_t count = resp->entry_count;

    if (count > 256) {
        count = 256;
    }

    for (uint64_t i = 0; i < count; i++) {
        struct limine_memmap_entry *entry =
            resp->entries[i];

        regions[i].base =
            entry->base;

        regions[i].length =
            entry->length;

        regions[i].type =
            limine_to_boot_type(
                entry->type
            );
    }

    if (!pmm_init_from_map(
            &kernel_pmm,
            regions,
            (size_t)count,
            kernel_pmm_bitmap,
            sizeof(kernel_pmm_bitmap),
            PMM_MAX_PHYS_BYTES)) {

        KERNEL_PANIC(
            "pmm_init_from_map failed",
            0x4D360002u
        );
    }

    log_writeln(
        "[M6] pmm initialized"
    );

    log_write(
        "[M6] frames managed = "
    );
    log_dec_u64(
        pmm_frame_count(
            &kernel_pmm
        )
    );
    log_putc('\n');

    log_write(
        "[M6] frames free = "
    );
    log_dec_u64(
        pmm_free_count(
            &kernel_pmm
        )
    );
    log_putc('\n');

    uint64_t frame =
        pmm_alloc_frame(
            &kernel_pmm
        );

    if (frame == PMM_INVALID_FRAME) {
        KERNEL_PANIC(
            "pmm_alloc_frame failed",
            0x4D360003u
        );
    }

    log_write(
        "[M6] sample frame = "
    );
    log_hex64(frame);
    log_putc('\n');

    if (!pmm_free_frame(
            &kernel_pmm,
            frame)) {

        KERNEL_PANIC(
            "pmm_free_frame failed",
            0x4D360004u
        );
    }
}
static void m6_keep_symbols(void) {
    (void)&kernel_pmm;
    (void)&kernel_pmm_bitmap;
}

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
    m6_keep_symbols();

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

m6_memory_init();

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
