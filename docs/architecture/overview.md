# MCSOS Architecture Overview

## Target Platform

MCSOS menargetkan:

- Arsitektur x86_64
- Windows 11 x64 host
- WSL 2 sebagai development environment
- QEMU + OVMF sebagai virtualisasi dan firmware UEFI

## Kernel Model

MCSOS menggunakan model kernel monolitik pendidikan untuk tujuan pembelajaran sistem operasi modern.

Kernel ditulis menggunakan bahasa C freestanding dengan komponen low-level terbatas pada assembly inline yang diperlukan untuk operasi hardware.

## Build Environment

Toolchain utama:

- clang
- ld.lld
- GNU binutils
- make
- qemu-system-x86_64

## Boot Strategy

Tahap awal boot menggunakan bootloader Limine untuk mempermudah transisi menuju higher-half kernel x86_64.

## Non Goals

Pada tahap awal praktikum, MCSOS belum menargetkan:

- multitasking
- userspace
- filesystem penuh
- network stack
- driver hardware kompleks
- SMP support
