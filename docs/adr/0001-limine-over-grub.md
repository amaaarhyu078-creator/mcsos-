# ADR 0001: Memilih Limine dibanding GRUB/Multiboot2

## Status
Accepted

## Context

MCSOS membutuhkan bootloader yang:

1. Mendukung ELF64 kernel freestanding.
2. Mudah digunakan pada QEMU + OVMF.
3. Mendukung BIOS dan UEFI.
4. Memiliki konfigurasi sederhana.
5. Cocok untuk eksperimen kernel modern x86_64.
6. Memiliki dokumentasi boot protocol yang jelas.
7. Tidak memerlukan kompleksitas userspace boot framework.

Alternatif yang dipertimbangkan:

- Limine
- GRUB2
- Multiboot2 dengan GRUB
- Custom bootloader

## Decision

MCSOS memilih Limine sebagai bootloader utama untuk milestone M2.

## Rationale

### Keunggulan Limine

1. Native support untuk ELF64 higher-half kernel.
2. Konfigurasi sederhana dan minimal.
3. Dukungan BIOS dan UEFI sekaligus.
4. Integrasi baik dengan QEMU dan OVMF.
5. Dokumentasi protocol modern dan kernel-oriented.
6. Mengurangi kompleksitas early boot dibanding GRUB scripting.
7. Memiliki path migrasi menuju protocol boot modern.

### Kelemahan GRUB/Multiboot2

1. Konfigurasi lebih kompleks.
2. Fokus lebih besar pada general-purpose boot management.
3. Layer boot lebih besar dibanding kebutuhan MCSOS M2.
4. Parsing Multiboot2 menambah kompleksitas awal kernel.

### Mengapa bukan custom bootloader

1. Menambah attack surface dan debugging burden.
2. Mengalihkan fokus dari pengembangan kernel.
3. Membutuhkan validasi BIOS/UEFI sendiri.
4. Tidak efisien untuk milestone awal.

## Consequences

### Positif

1. Boot path lebih sederhana.
2. Debugging lebih mudah.
3. Build dan QEMU integration lebih stabil.
4. Kernel dapat fokus pada subsistem inti.

### Negatif

1. Bergantung pada supply-chain Limine.
2. Perubahan protocol Limine harus dipantau.
3. Build environment memerlukan fetch bootloader.

## Evidence

1. Kernel ELF64 berhasil diboot menggunakan Limine.
2. QEMU + OVMF boot stabil pada machine q35 dan pc.
3. Serial log menunjukkan kernel mencapai kmain.
4. ISO hybrid berhasil dibuat dan dijalankan.

