# Readiness Review M2 - Boot Image dan Early Serial Console

## Identitas

- Proyek: MCSOS 260502
- Praktikum: M2
- Target: x86_64, QEMU, OVMF, Limine
- Nama/Kelompok: salma
- Commit hash: 160940df66d038f1116a9f2d7d2ec1b7977fbb88
- Tanggal: 2026-05-19

## Ringkasan Status

Status yang diajukan: siap uji QEMU tahap M2.

Alasan ringkas:
Kernel ELF64 berhasil dibangun menggunakan Clang dan ld.lld dalam mode freestanding. Image ISO berhasil dibuat menggunakan Limine dan dapat dijalankan pada QEMU + OVMF. Serial console berhasil mengeluarkan marker boot M2 dan kernel masuk ke controlled halt loop tanpa reboot loop.

## Evidence Matrix

| Evidence | Lokasi | Status | Catatan |
|---|---|---|---|
| Preflight M2 | `build/meta/m2-preflight.txt` | PASS | Seluruh dependency utama tersedia |
| Kernel ELF | `build/kernel.elf` | PASS | ELF64 x86_64 |
| Kernel map | `build/kernel.map` | PASS | Symbol kernel valid |
| readelf header | `build/inspect/readelf-header.txt` | PASS | Entry point benar |
| readelf PHDR | `build/inspect/readelf-program-headers.txt` | PASS | Segment load valid |
| objdump | `build/inspect/objdump-disassembly.txt` | PASS | Disassembly dapat diperiksa |
| ISO | `build/mcsos.iso` | PASS | ISO bootable berhasil dibuat |
| ISO checksum | `build/mcsos.iso.sha256` | PASS | Checksum valid |
| Serial log | `build/qemu-serial.log` | PASS | Marker boot M2 muncul |
| Git commit | `build/meta/m2-commit.txt` | PASS | Commit hash tersimpan |

## Invariants yang Diperiksa

1. Kernel adalah ELF64 x86_64.
2. Entry point sesuai linker script.
3. Kernel tidak memakai hosted libc.
4. Source dikompilasi dengan `-ffreestanding` dan `-mno-red-zone`.
5. Serial console tersedia sebelum subsistem kompleks.
6. Kernel tidak kembali setelah `kmain`.
7. Output QEMU disimpan sebagai log file.

## Failure Modes yang Diuji atau Dianalisis

| Failure mode | Pernah terjadi? | Diagnosis | Perbaikan |
|---|---|---|---|
| Toolchain salah | Tidak | Clang dan ld.lld valid | Tidak diperlukan |
| OVMF tidak ditemukan | Tidak | OVMF terdeteksi normal | Tidak diperlukan |
| Limine gagal fetch | Tidak | GitHub dapat diakses | Tidak diperlukan |
| ISO gagal dibuat | Tidak | xorriso dan limine-install berjalan normal | Tidak diperlukan |
| QEMU log kosong | Tidak | Serial log berhasil muncul | Tidak diperlukan |
| Entry point salah | Tidak | Entry point sesuai linker script | Tidak diperlukan |
| Reboot loop | Tidak | Kernel masuk halt loop normal | Tidak diperlukan |
| CRLF script | Tidak | Script menggunakan LF | Tidak diperlukan |

## Keputusan Readiness

- [x] Lulus M2: siap uji QEMU tahap M2.
- [ ] Belum lulus M2: perlu perbaikan.

## Catatan Reviewer

Seluruh checkpoint M2 berhasil dipenuhi:
- build kernel ELF64
- inspect ELF
- fetch Limine
- build ISO
- boot QEMU
- serial logging
- local grading

Kernel berhasil mencapai `kmain` dan menghasilkan output serial sesuai marker yang dipersyaratkan.# Readiness Review M2 - Boot Image dan Early Serial Console

## Identitas

- Proyek: MCSOS 260502
- Praktikum: M2
- Target: x86_64, QEMU, OVMF, Limine
- Nama/Kelompok: salma
- Commit hash: 160940df66d038f1116a9f2d7d2ec1b7977fbb88
- Tanggal: 2026-05-19

## Ringkasan Status

Status yang diajukan: siap uji QEMU tahap M2.

Alasan ringkas:
Kernel ELF64 berhasil dibangun menggunakan Clang dan ld.lld dalam mode freestanding. Image ISO berhasil dibuat menggunakan Limine dan dapat dijalankan pada QEMU + OVMF. Serial console berhasil mengeluarkan marker boot M2 dan kernel masuk ke controlled halt loop tanpa reboot loop.

## Evidence Matrix

| Evidence | Lokasi | Status | Catatan |
|---|---|---|---|
| Preflight M2 | `build/meta/m2-preflight.txt` | PASS | Seluruh dependency utama tersedia |
| Kernel ELF | `build/kernel.elf` | PASS | ELF64 x86_64 |
| Kernel map | `build/kernel.map` | PASS | Symbol kernel valid |
| readelf header | `build/inspect/readelf-header.txt` | PASS | Entry point benar |
| readelf PHDR | `build/inspect/readelf-program-headers.txt` | PASS | Segment load valid |
| objdump | `build/inspect/objdump-disassembly.txt` | PASS | Disassembly dapat diperiksa |
| ISO | `build/mcsos.iso` | PASS | ISO bootable berhasil dibuat |
| ISO checksum | `build/mcsos.iso.sha256` | PASS | Checksum valid |
| Serial log | `build/qemu-serial.log` | PASS | Marker boot M2 muncul |
| Git commit | `build/meta/m2-commit.txt` | PASS | Commit hash tersimpan |

## Invariants yang Diperiksa

1. Kernel adalah ELF64 x86_64.
2. Entry point sesuai linker script.
3. Kernel tidak memakai hosted libc.
4. Source dikompilasi dengan `-ffreestanding` dan `-mno-red-zone`.
5. Serial console tersedia sebelum subsistem kompleks.
6. Kernel tidak kembali setelah `kmain`.
7. Output QEMU disimpan sebagai log file.

## Failure Modes yang Diuji atau Dianalisis

| Failure mode | Pernah terjadi? | Diagnosis | Perbaikan |
|---|---|---|---|
| Toolchain salah | Tidak | Clang dan ld.lld valid | Tidak diperlukan |
| OVMF tidak ditemukan | Tidak | OVMF terdeteksi normal | Tidak diperlukan |
| Limine gagal fetch | Tidak | GitHub dapat diakses | Tidak diperlukan |
| ISO gagal dibuat | Tidak | xorriso dan limine-install berjalan normal | Tidak diperlukan |
| QEMU log kosong | Tidak | Serial log berhasil muncul | Tidak diperlukan |
| Entry point salah | Tidak | Entry point sesuai linker script | Tidak diperlukan |
| Reboot loop | Tidak | Kernel masuk halt loop normal | Tidak diperlukan |
| CRLF script | Tidak | Script menggunakan LF | Tidak diperlukan |

## Keputusan Readiness

- [x] Lulus M2: siap uji QEMU tahap M2.
- [ ] Belum lulus M2: perlu perbaikan.

## Catatan Reviewer

Seluruh checkpoint M2 berhasil dipenuhi:
- build kernel ELF64
- inspect ELF
- fetch Limine
- build ISO
- boot QEMU
- serial logging
- local grading

Kernel berhasil mencapai `kmain` dan menghasilkan output serial sesuai marker yang dipersyaratkan.
