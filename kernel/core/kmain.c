#include <limine.h>

__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);

void kmain(void) {
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
