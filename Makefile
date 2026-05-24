.RECIPEPREFIX := >
SHELL := /usr/bin/env bash

ARCH := x86_64
BUILD_DIR := build

KERNEL := $(BUILD_DIR)/kernel.elf
MAP := $(BUILD_DIR)/kernel.map

CC := clang
LD := ld.lld
OBJDUMP := objdump
READELF := readelf
NM := nm

CFLAGS := \
--target=x86_64-unknown-none-elf \
-std=c17 \
-ffreestanding \
-fno-stack-protector \
-fno-stack-check \
-fno-pic \
-fno-pie \
-fno-lto \
-m64 \
-march=x86-64 \
-mabi=sysv \
-mno-red-zone \
-mno-mmx \
-mno-sse \
-mno-sse2 \
-mcmodel=kernel \
-Wall \
-Wextra \
-Werror \
-Ikernel/arch/x86_64/include \
-Ikernel/include

LDFLAGS := \
-nostdlib \
-static \
-z max-page-size=0x1000 \
-T linker.ld \
-Map=$(MAP)

SRC_C := $(shell find kernel -name '*.c' | LC_ALL=C sort)
OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC_C))

.PHONY: all build inspect run clean

all: build

build: $(KERNEL)

$(BUILD_DIR)/%.o: %.c
>mkdir -p $(dir $@)
>$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJ) linker.ld
>mkdir -p $(BUILD_DIR)
>$(LD) $(LDFLAGS) -o $@ $(OBJ)

inspect:
>$(READELF) -h $(KERNEL)
>$(READELF) -l $(KERNEL)
>$(NM) -n $(KERNEL) | head


clean:
>rm -rf $(BUILD_DIR)
ISO := $(BUILD_DIR)/mcsos.iso

image: $(ISO)

$(ISO): $(KERNEL)
>mkdir -p iso_root/boot
>mkdir -p iso_root/boot/limine

>cp $(KERNEL) iso_root/boot/kernel.elf

>cp third_party/limine/limine-bios-cd.bin iso_root/boot/limine/
>cp third_party/limine/limine-uefi-cd.bin iso_root/boot/limine/
>cp third_party/limine/limine-bios.sys iso_root/boot/limine/

>cp limine.cfg iso_root/boot/limine/limine.conf

>xorriso -as mkisofs \
>-b boot/limine/limine-bios-cd.bin \
>-no-emul-boot \
>-boot-load-size 4 \
>-boot-info-table \
>--efi-boot boot/limine/limine-uefi-cd.bin \
>-efi-boot-part \
>--efi-boot-image \
>--protective-msdos-label \
>iso_root \
>-o $(ISO)

run: image
>qemu-system-x86_64 \
>-machine q35 \
>-cdrom $(ISO) \
>-serial stdio \
>-no-reboot \
>-no-shutdown
