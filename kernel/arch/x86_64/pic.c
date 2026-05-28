#include <stdint.h>

#include "io.h"
#include "pic.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21

#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC_EOI      0x20

#define ICW1_ICW4    0x01
#define ICW1_INIT    0x10

#define ICW4_8086    0x01

static uint8_t pic_master_mask = 0xFF;
static uint8_t pic_slave_mask  = 0xFF;

uint8_t pic_read_master_mask(void) {
    return pic_master_mask;
}

uint8_t pic_read_slave_mask(void) {
    return pic_slave_mask;
}

void pic_mask_all(void) {
    pic_master_mask = 0xFF;
    pic_slave_mask  = 0xFF;

    outb(PIC1_DATA, pic_master_mask);
    outb(PIC2_DATA, pic_slave_mask);
}

void pic_unmask_irq(uint8_t irq) {
    if (irq < 8) {
        pic_master_mask &= (uint8_t) ~(1u << irq);
        outb(PIC1_DATA, pic_master_mask);
    } else {
        irq -= 8;

        pic_slave_mask &= (uint8_t) ~(1u << irq);
        outb(PIC2_DATA, pic_slave_mask);
    }
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_remap(uint8_t master_offset, uint8_t slave_offset) {
    uint8_t master_mask = inb(PIC1_DATA);
    uint8_t slave_mask  = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, master_offset);
    io_wait();

    outb(PIC2_DATA, slave_offset);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();

    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();

    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    pic_master_mask = master_mask;
    pic_slave_mask  = slave_mask;

    outb(PIC1_DATA, pic_master_mask);
    outb(PIC2_DATA, pic_slave_mask);
}
