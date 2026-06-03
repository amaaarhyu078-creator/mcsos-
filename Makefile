.RECIPEPREFIX := >

SHELL := /usr/bin/env bash

BUILD_DIR := build

KERNEL := $(BUILD_DIR)/kernel.elf
BP_KERNEL := $(BUILD_DIR)/kernel.breakpoint.elf
PANIC_KERNEL := $(BUILD_DIR)/kernel.panic.elf
FAULT_INT3_KERNEL := $(BUILD_DIR)/kernel.fault-int3.elf

MAP := $(BUILD_DIR)/kernel.map
BP_MAP := $(BUILD_DIR)/kernel.breakpoint.map
PANIC_MAP := $(BUILD_DIR)/kernel.panic.map

DISASM := $(BUILD_DIR)/kernel.disasm.txt
SYMS := $(BUILD_DIR)/kernel.syms.txt

CC := clang
LD := ld.lld
OBJDUMP := objdump
READELF := readelf
NM := nm

COMMON_CFLAGS := --target=x86_64-unknown-none-elf -std=c17 -ffreestanding -fno-builtin -fno-stack-protector -fno-stack-check -fno-pic -fno-pie -fno-lto -m64 -march=x86-64 -mabi=sysv -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mcmodel=kernel -Wall -Wextra -Werror -Ikernel/arch/x86_64/include -Ikernel/include

COMMON_ASFLAGS := --target=x86_64-unknown-none-elf -ffreestanding -fno-pic -fno-pie -m64 -mno-red-zone -Wall -Wextra -Werror -Ikernel/arch/x86_64/include -Ikernel/include

CFLAGS := $(COMMON_CFLAGS)
ASFLAGS := $(COMMON_ASFLAGS)

BP_CFLAGS := $(COMMON_CFLAGS) -DMCSOS_M4_TRIGGER_BREAKPOINT=1
PANIC_CFLAGS := $(COMMON_CFLAGS) -DMCSOS_M4_TRIGGER_PANIC=1
FAULT_INT3_CFLAGS := $(COMMON_CFLAGS) -DMCSOS_M4_TRIGGER_BREAKPOINT=1

LDFLAGS := -nostdlib -static -z max-page-size=0x1000 -T linker.ld

SRC_C := $(shell find kernel -name '*.c' | LC_ALL=C sort)
SRC_S := $(shell find kernel -name '*.S' | LC_ALL=C sort)

OBJ := $(patsubst %.c,$(BUILD_DIR)/normal/%.o,$(SRC_C)) \
$(patsubst %.S,$(BUILD_DIR)/normal/%.o,$(SRC_S))

BP_OBJ := $(patsubst %.c,$(BUILD_DIR)/breakpoint/%.o,$(SRC_C)) \
$(patsubst %.S,$(BUILD_DIR)/breakpoint/%.o,$(SRC_S))

PANIC_OBJ := $(patsubst %.c,$(BUILD_DIR)/panic/%.o,$(SRC_C)) \
$(patsubst %.S,$(BUILD_DIR)/panic/%.o,$(SRC_S))

FAULT_INT3_OBJ := $(patsubst %.c,$(BUILD_DIR)/fault-int3/%.o,$(SRC_C)) \
$(patsubst %.S,$(BUILD_DIR)/fault-int3/%.o,$(SRC_S))

.PHONY: all build breakpoint panic fault-int3 inspect audit clean distclean

all: build inspect

build: $(KERNEL)

breakpoint: $(BP_KERNEL)

panic: $(PANIC_KERNEL)

fault-int3: $(FAULT_INT3_KERNEL)

$(BUILD_DIR)/normal/%.o: %.c
>mkdir -p $(dir $@)
>$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/normal/%.o: %.S
>mkdir -p $(dir $@)
>$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/breakpoint/%.o: %.c
>mkdir -p $(dir $@)
>$(CC) $(BP_CFLAGS) -c $< -o $@

$(BUILD_DIR)/breakpoint/%.o: %.S
>mkdir -p $(dir $@)
>$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/panic/%.o: %.c
>mkdir -p $(dir $@)
>$(CC) $(PANIC_CFLAGS) -c $< -o $@

$(BUILD_DIR)/panic/%.o: %.S
>mkdir -p $(dir $@)
>$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/fault-int3/%.o: %.c
>mkdir -p $(dir $@)
>$(CC) $(FAULT_INT3_CFLAGS) -c $< -o $@

$(BUILD_DIR)/fault-int3/%.o: %.S
>mkdir -p $(dir $@)
>$(CC) $(ASFLAGS) -c $< -o $@

$(KERNEL): $(OBJ) linker.ld
>mkdir -p $(BUILD_DIR)
>$(LD) $(LDFLAGS) -Map=$(MAP) -o $@ $(OBJ)

$(BP_KERNEL): $(BP_OBJ) linker.ld
>mkdir -p $(BUILD_DIR)
>$(LD) $(LDFLAGS) -Map=$(BP_MAP) -o $@ $(BP_OBJ)

$(PANIC_KERNEL): $(PANIC_OBJ) linker.ld
>mkdir -p $(BUILD_DIR)
>$(LD) $(LDFLAGS) -Map=$(PANIC_MAP) -o $@ $(PANIC_OBJ)

$(FAULT_INT3_KERNEL): $(FAULT_INT3_OBJ) linker.ld
>mkdir -p $(BUILD_DIR)
>$(LD) $(LDFLAGS) -o $@ $(FAULT_INT3_OBJ)

inspect: $(KERNEL)
>$(READELF) -h $(KERNEL) > $(BUILD_DIR)/kernel.readelf.header.txt
>$(READELF) -l $(KERNEL) > $(BUILD_DIR)/kernel.readelf.programs.txt
>$(NM) -n $(KERNEL) > $(SYMS)
>$(OBJDUMP) -d -Mintel $(KERNEL) > $(DISASM)

audit: inspect breakpoint panic fault-int3

clean:
>rm -rf $(BUILD_DIR)

distclean: clean
>rm -rf iso_root limine evidence
grade: $(KERNEL)
>mkdir -p build
>cp $(KERNEL) build/mcsos-m5.elf
>cp $(MAP) build/mcsos-m5.map

>$(READELF) -h build/mcsos-m5.elf \
>    > build/readelf-header.txt

>$(READELF) -S build/mcsos-m5.elf \
>    > build/readelf-sections.txt

>$(READELF) -l build/mcsos-m5.elf \
>    > build/readelf-program-headers.txt

>$(NM) -n build/mcsos-m5.elf \
>    > build/symbols.txt

>$(NM) -u build/mcsos-m5.elf \
>    > build/undefined.txt

>$(OBJDUMP) -d -Mintel build/mcsos-m5.elf \
>    > build/disassembly.txt

>echo "[M5] grade artifacts generated"
ISO := $(BUILD_DIR)/mcsos.iso

iso: $(KERNEL)
>mkdir -p iso_root/boot
>cp $(KERNEL) iso_root/boot/kernel.elf

>xorriso -as mkisofs \
>    -b boot/limine/limine-bios-cd.bin \
>    -no-emul-boot \
>    -boot-load-size 4 \
>    -boot-info-table \
>    --efi-boot boot/limine/limine-uefi-cd.bin \
>    -efi-boot-part \
>    --efi-boot-image \
>    --protective-msdos-label \
>    iso_root \
>    -o $(ISO)

>echo "[M5] ISO generated at $(ISO)"
HOSTCC ?= cc

build/test_pmm_host: kernel/core/pmm.c tests/test_pmm_host.c
>mkdir -p build
>$(HOSTCC) \
>    -std=c17 \
>    -Wall \
>    -Wextra \
>    -Werror \
>    -Ikernel/include \
>    tests/test_pmm_host.c \
>    kernel/core/pmm.c \
>    -o build/test_pmm_host

check-m6: build/test_pmm_host
>./build/test_pmm_host
build/vmm.o: kernel/core/vmm.c kernel/include/mcsos/kernel/vmm.h kernel/include/types.h
>mkdir -p build
>$(CC) \
>    $(COMMON_CFLAGS) \
>    -c kernel/core/vmm.c \
>    -o build/vmm.o

build/test_vmm_host: kernel/core/vmm.c tests/test_vmm_host.c \
                      kernel/include/mcsos/kernel/vmm.h \
                      kernel/include/types.h
>mkdir -p build
>$(HOSTCC) \
>    -DMCSOS_HOST_TEST \
>    -std=c17 \
>    -Wall \
>    -Wextra \
>    -Werror \
>    -Ikernel/include \
>    kernel/core/vmm.c \
>    tests/test_vmm_host.c \
>    -o build/test_vmm_host

check-m7: build/vmm.o build/test_vmm_host
>./build/test_vmm_host
>nm -u build/vmm.o
>objdump -dr build/vmm.o > build/vmm.objdump.txt
>grep -q "invlpg" build/vmm.objdump.txt
>grep -q "cr3" build/vmm.objdump.txt

check: check-m7
