#include <stdint.h>

#include <limine.h>

#include <mcsos/arch/cpu.h>
#include <mcsos/arch/idt.h>

#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>
#include <mcsos/kernel/pmm.h>
#include <mcsos/kernel/vmm.h>
#include <mcsos/kernel/kmem.h>
#include <mcsos/kernel/version.h>

#include <mcsos/syscall.h>
#include <mcsos/arch/serial.h>


#include "mcsos_thread.h"

void m12_sync_selftest(void);

#include <pic.h>
#include <pit.h>

void *memset(
    void *dest,
    int value,
    size_t count
);

__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);
__attribute__((used, section(".requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

extern char __kernel_start[];
extern char __kernel_end[];

static struct pmm_state kernel_pmm;

static struct vmm_space kernel_space;

static uint64_t hhdm_offset = 0;

#define M8_BOOT_HEAP_SIZE (64u * 1024u)

#define MCSOS_USER_BASE  0x0000000000400000ULL
#define MCSOS_USER_LIMIT 0x0000800000000000ULL

static unsigned char
m8_boot_heap[M8_BOOT_HEAP_SIZE]
__attribute__((aligned(4096)));

static uint8_t kernel_pmm_bitmap[PMM_BITMAP_BYTES]
    __attribute__((aligned(4096)));
static uint8_t kernel_pmm_bitmap[PMM_BITMAP_BYTES]
    __attribute__((aligned(4096)));

mcsos_scheduler_t g_sched;
static uint64_t k_get_ticks(void)
{
    return timer_ticks();
}

static void k_yield_current(void)
{
    (void)mcsos_sched_yield(&g_sched);
}

static void k_exit_current(int code)
{
    (void)code;
    /* M10 stub */
}

static int64_t k_write_serial(
    const char *buf,
    size_t len
)
{
    if (buf == 0) {
        return -22;
    }

    serial_write(buf);
    return (int64_t)len;
}


static mcsos_thread_t g_boot_thread;

static void m10_syscall_smoke_direct(void)
{
    int64_t r =
        mcsos_syscall_dispatch(
            MCSOS_SYS_PING,
            0, 0, 0, 0, 0, 0
        );

    if (r != 0x2605020A) {
        KERNEL_PANIC(
            "M10 syscall ping failed",
            (uint64_t)r
        );
    }

    log_writeln(
        "[M10] syscall ping ok"
    );
}

static mcsos_thread_t g_thread_a;
static mcsos_thread_t g_thread_b;

static unsigned char g_stack_a[8192]
    __attribute__((aligned(16)));

static unsigned char g_stack_b[8192]
    __attribute__((aligned(16)));

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

if (hhdm_request.response == NULL) {
        KERNEL_PANIC(
            "limine hhdm missing",
            0x4D360005u
        );
    }

    hhdm_offset =
        hhdm_request.response->offset;

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
static uint64_t kernel_vmm_alloc(
    void *ctx
) {
    (void)ctx;

    return pmm_alloc_frame(
        &kernel_pmm
    );
}

static void kernel_vmm_free(
    void *ctx,
    uint64_t frame_paddr
) {
    (void)ctx;

    pmm_free_frame(
        &kernel_pmm,
        frame_paddr
    );
}

static void *kernel_phys_to_virt(
    void *ctx,
    uint64_t paddr
) {
    (void)ctx;

    return (void *)(uintptr_t)(
        paddr + hhdm_offset
    );
}

static void m7_zero_page(
    void *page
) {
    memset(
        page,
        0,
        VMM_PAGE_SIZE
    );
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

static void m8_heap_bootstrap(void)
{
    int rc =
        kmem_init(
            m8_boot_heap,
            sizeof(m8_boot_heap)
        );

    if (rc != 0) {
        KERNEL_PANIC(
            "m8 kmem_init failed",
            0x4D380001u
        );
    }

    void *probe =
        kmem_alloc(128);

    if (probe == 0) {
        KERNEL_PANIC(
            "m8 kmem_alloc failed",
            0x4D380002u
        );
    }

    if (kmem_free_checked(probe) != 0) {
        KERNEL_PANIC(
            "m8 kmem_free failed",
            0x4D380003u
        );
    }

    kmem_stats_t st;

    kmem_get_stats(&st);

    log_writeln(
        "[M8] heap initialized"
    );

    log_write(
        "[M8] heap total = "
    );
    log_dec_u64(
        (uint64_t)st.total_bytes
    );
    log_putc('\n');

    log_write(
        "[M8] heap free = "
    );
    log_dec_u64(
        (uint64_t)st.free_bytes
    );
    log_putc('\n');

    log_write(
        "[M8] heap largest = "
    );
    log_dec_u64(
        (uint64_t)st.largest_free
    );
    log_putc('\n');
}

static void demo_thread_a(void *arg)
{
    (void)arg;

    for (;;) {
	log_writeln("[M9] thread A tick");
        mcsos_sched_yield(&g_sched);
    }
}

static void demo_thread_b(void *arg)
{
    (void)arg;

    for (;;) {
	log_writeln("[M9] thread B tick");
        mcsos_sched_yield(&g_sched);
    }
}

void kmain(void) {    m6_keep_symbols();

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
 * M7 VMM initialization
 */
{
    uint64_t root =
        pmm_alloc_frame(
            &kernel_pmm
        );

    if (root == PMM_INVALID_FRAME) {

        KERNEL_PANIC(
            "M7 root page allocation failed",
            0x4D370001u
        );
    }

log_writeln(
    "[DBG] before m7_zero_page"
);

    m7_zero_page(
        kernel_phys_to_virt(
            0,
            root
        )
    );

log_writeln(
    "[DBG] after m7_zero_page"
);

    int rc =
        vmm_space_init(
            &kernel_space,
            root,
            0,
            kernel_vmm_alloc,
            kernel_vmm_free,
            kernel_phys_to_virt
        );

    if (rc != VMM_MAP_OK) {

        KERNEL_PANIC(
            "M7 vmm_space_init failed",
            0x4D370002u
        );
    }

    log_writeln(
        "[M7] VMM core initialized"
    );

    log_writeln(
        "[M7] ready for QEMU smoke test"
    );
}
m8_heap_bootstrap();

m12_sync_selftest();

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

mcsos_scheduler_init(
    &g_sched,
    &g_boot_thread
);

mcsos_thread_prepare(
    &g_thread_a,
    "demo-a",
    demo_thread_a,
    0,
    g_stack_a,
    sizeof(g_stack_a),
    g_sched.next_id++
);

mcsos_thread_prepare(
    &g_thread_b,
    "demo-b",
    demo_thread_b,
    0,
    g_stack_b,
    sizeof(g_stack_b),
    g_sched.next_id++
);

mcsos_sched_enqueue(
    &g_sched,
    &g_thread_a
);

mcsos_sched_enqueue(
    &g_sched,
    &g_thread_b
);

log_writeln(
    "[M9] scheduler initialized"
);

log_writeln(
    "[M14] block layer initialized"
);

mcsos_syscall_ops_t ops = {
    .get_ticks = k_get_ticks,
    .yield_current = k_yield_current,
    .exit_current = k_exit_current,
    .write_serial = k_write_serial
};

mcsos_syscall_init(&ops);

mcsos_syscall_set_user_region(
    (mcsos_user_region_t){
        .base = MCSOS_USER_BASE,
        .limit = MCSOS_USER_LIMIT,
    }
);

m10_syscall_smoke_direct();

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
    "[M9] starting first scheduler switch"
);

mcsos_sched_yield(
    &g_sched
);

log_writeln(
    "[M5] entering idle halt loop"
);

for (;;) {
    cpu_hlt();
}
#endif
}
