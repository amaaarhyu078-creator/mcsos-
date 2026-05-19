# MCSOS Readiness Gates

## M0 Gate

Repository harus memenuhi:

- repository berada di filesystem Linux WSL
- toolchain berhasil terpasang
- dokumentasi baseline tersedia
- threat model awal tersedia
- verification matrix tersedia

## M1 Gate

Repository harus memenuhi:

- freestanding compilation berhasil
- ELF object dapat diinspeksi
- reproducible build baseline tersedia
- QEMU capability evidence tersedia

## M2 Gate

Repository harus memenuhi:

- kernel ELF64 berhasil dibangun
- higher-half linker aktif
- ISO bootable berhasil dibuat
- kernel boot pada QEMU/OVMF
- serial logging aktif
- inspection artifact tersedia
