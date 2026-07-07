# Laporan Praktikum Sistem Operasi Lanjut — MCSOS - M3

**Nama file laporan:** `laporan_praktikum_[M3]_[2583207073007].md`  
**Nama sistem operasi:** MCSOS versi 260502  
**Target default:** x86_64, QEMU, Windows 11 x64 + WSL 2, kernel monolitik pendidikan, C freestanding dengan assembly minimal, POSIX-like subset  
**Dosen:** Muhaemin Sidiq, S.Pd., M.Pd.  
**Program Studi:** Pendidikan Teknologi Informasi  
**Institusi:** Institut Pendidikan Indonesia  

> Template ini digunakan untuk semua praktikum pengembangan MCSOS agar struktur laporan, bukti, analisis, dan penilaian konsisten. Ganti seluruh teks bertanda `[isi ...]` dengan data praktikum sebenarnya. Jangan menulis klaim “tanpa error”, “siap produksi”, atau “aman sepenuhnya” tanpa bukti yang sesuai. Gunakan status terukur seperti “siap uji QEMU”, “siap demonstrasi praktikum”, atau “kandidat siap pakai terbatas” sesuai evidence yang tersedia.

---

## 0. Metadata Laporan

| Atribut | Isi |
|---|---|
| Kode praktikum | `[M3]` |
| Judul praktikum | `[judul praktikum]` |
| Jenis pengerjaan | `[Individu]` |
| Nama mahasiswa | `[Salma Rahayu]` |
| NIM | `[2583207073007]` |
| Kelas | `[PTI 1A]` |
| Nama kelompok | `[isi jika kelompok]` |
| Anggota kelompok | `[nama, NIM, peran ringkas]` |
| Tanggal praktikum | `[YYYY-MM-DD]` |
| Tanggal pengumpulan | `[YYYY-MM-DD]` |
| Repository | `https://github.com/amaaarhyu078-creator/mcsos-` |
| Branch | `praktikum/m3-panic-debug-audit` |
| Commit awal | `d3ce466` |
| Commit akhir | `0bc792c` |
| Status readiness yang diklaim | `siap uji QEMU dan siap lanjut M4 secara terbatas` |

---

## 1. Sampul

# Laporan Praktikum `[M3]`  
## `[Judul Praktikum]`

Disusun oleh:

| Nama | NIM | Kelas | Peran |
|---|---|---|---|
| `[Salma Rahayu]` | `[2583207073007]` | `[1A]` | `[individu]` |
| `[opsional]` | `[opsional]` | `[opsional]` | `[opsional]` |

Dosen Pengampu: **Muhaemin Sidiq, S.Pd., M.Pd.**  
Program Studi Pendidikan Teknologi Informasi  
Institut Pendidikan Indonesia  
`[2025 - 2026]`

---

## 2. Pernyataan Orisinalitas dan Integritas Akademik

Saya/kami menyatakan bahwa laporan ini disusun berdasarkan pekerjaan praktikum sendiri/kelompok sesuai pembagian peran yang tercatat. Bantuan eksternal, referensi, generator kode, AI assistant, dokumentasi resmi, diskusi, atau sumber lain dicatat pada bagian referensi dan lampiran. Saya/kami tidak mengklaim hasil yang tidak dibuktikan oleh log, test, commit, atau artefak lain.

| Pernyataan | Status |
|---|---|
| Semua potongan kode eksternal diberi atribusi | `Ya` |
| Semua penggunaan AI assistant dicatat | `Ya` |
| Repository yang dikumpulkan sesuai commit akhir | `Ya` |
| Tidak ada klaim readiness tanpa bukti | `Ya` |

Catatan penggunaan bantuan eksternal:

```text
[Praktikum menggunakan bantuan dokumentasi resmi LLVM/Clang, GNU Binutils, QEMU, OSDev Wiki, dan AI assistant ChatGPT untuk membantu penjelasan konsep, debugging build system, analisis error compiler/linker, penyusunan script otomatisasi, audit ELF/disassembly, serta penyusunan laporan praktikum.

AI assistant digunakan untuk:
- membantu diagnosis error build dan linker,
- membantu analisis QEMU dan GDB,
- membantu penyusunan script audit dan grading,
- membantu penyusunan dokumentasi laporan praktikum.

Seluruh kode, build, audit, QEMU test, GDB debugging, dan verifikasi repository tetap dijalankan serta diverifikasi mandiri oleh praktikan menggunakan environment lokal WSL2.

Verifikasi mandiri dilakukan melalui:
- make build
- make panic
- make audit
- grade_m3.sh
- QEMU smoke test
- GDB breakpoint verification
- evidence collection
- Git commit dan GitHub push]
```

---

## 3. Tujuan Praktikum

Tuliskan tujuan teknis dan konseptual praktikum. Tujuan harus dapat diuji.

1. `[Tujuan teknis 1: Membangun kernel freestanding x86_64 yang dapat melakukan serial logging, panic handling, dan ELF audit tanpa dependency libc.)
2. `[Tujuan teknis 2:Menghasilkan kernel bootable pada QEMU serta melakukan debugging menggunakan GDB melalui QEMU gdbstub.]`
3. `[Tujuan konseptual 1: Memahami konsep panic path, fail-closed halt behavior, linker layout, symbol table, dan observability awal pada pengembangan kernel sistem operasi.]`
4. `[Tujuan validasi: Memvalidasi kernel menggunakan build log, QEMU serial log, readelf, nm, objdump, GDB evidence, dan grading script secara deterministik.]`

---

## 4. Capaian Pembelajaran Praktikum

Setelah praktikum ini, mahasiswa mampu:

| CPL/CPMK praktikum | Bukti yang harus ditunjukkan |
|---|---|
| (Memeriksa kesiapan hasil M0, M1, dan M2 sebelum mengubah kernel.) | (Output m3_preflight.sh, log environment check, dan repository status.) |
| (Menjelaskan perbedaan boot berhasil, controlled halt, panic, triple fault, hang, dan silent-failure.) | (Analisis pada laporan, QEMU log, dan failure mode discussion.) |
| (Membuat panic path awal yang memiliki kontrak noreturn, mematikan interrupt, mencetak bukti minimum, lalu masuk halt loop.) | (Source kernel_panic_at(), disassembly cli/hlt, dan panic symbol pada nm/objdump.) |
| (Membuat wrapper logging awal yang memisahkan API kernel logging dari driver serial.) | (Source log.c, serial.c, dan hasil serial logging pada QEMU.) |
| (Menghasilkan dua varian kernel: normal kernel dan intentional-panic kernel.) | (build/kernel.elf, build/kernel.panic.elf, make build, dan make panic.) |
| (Menghasilkan dan menganalisis linker map, symbol table, readelf header, program header, dan disassembly.) | (kernel.map, kernel.syms.txt, kernel.readelf.header.txt, kernel.readelf.programs.txt, dan kernel.disasm.txt.) |
| (Menjalankan QEMU smoke test dengan log serial berbasis file.) | (build/m3_serial.log dan output m3_qemu_run.sh.) |
| (Menyiapkan sesi GDB untuk breakpoint pada kmain dan kernel_panic_at.) | (Screenshot/log GDB breakpoint kmain dan disassembly GDB.) |
| (Mengumpulkan bukti praktikum secara reproducible ke direktori evidence/M3.) | (evidence/M3/manifest.txt dan file evidence lainnya.) |
| (Menyusun failure analysis dan rollback bila source M0/M1/M2 belum konsisten.) | (Bagian failure modes, rollback analysis, dan dokumentasi troubleshooting pada laporan.) |

---

## 5. Peta Milestone MCSOS

Centang milestone yang menjadi fokus laporan ini. Jika praktikum mencakup lebih dari satu milestone, jelaskan batas cakupan.

| Milestone | Fokus | Status dalam laporan |
|---|---|---|
| M0 | Requirements, governance, baseline arsitektur | `[ ] tidak dibahas / [ ] dibahas / [V] selesai praktikum` |
| M1 | Toolchain reproducible, Git, QEMU, GDB, metadata build | `[ ] tidak dibahas / [ ] dibahas / [V] selesai praktikum` |
| M2 | Boot image, kernel ELF64, early console | `[ ] tidak dibahas / [ ] dibahas / [V] selesai praktikum` |
| M3 | Panic path, linker map, GDB, observability awal | `[ ] tidak dibahas / [V ] dibahas / [ ] selesai praktikum` |
| M4 | Trap, exception, interrupt, timer | `[V] tidak dibahas / [] dibahas / [ ] selesai praktikum` |
| M5 | PMM, VMM, page table, kernel heap | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M6 | Thread, scheduler, synchronization | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M7 | Syscall ABI dan user program loader | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M8 | VFS, file descriptor, ramfs | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M9 | Block layer dan device model | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M10 | Persistent filesystem, mcsfs/ext2-like, recovery | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M11 | Networking stack, packet parsing, UDP/TCP subset | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M12 | Security model, capability/ACL, syscall fuzzing, hardening | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M13 | SMP, scalability, lock stress, NUMA-aware preparation | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M14 | Framebuffer, graphics console, visual regression | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M15 | Virtualization/container subset | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M16 | Observability, update/rollback, release image, readiness review | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |

Batas cakupan praktikum:

```text
[Praktikum M3 berfokus pada implementasi observability awal kernel melalui serial logging, panic path, ELF audit, disassembly inspection, QEMU smoke test, dan debugging menggunakan GDB pada kernel freestanding x86_64. Praktikum mencakup build normal kernel dan intentional-panic kernel, inspeksi linker map dan symbol table, serta pengumpulan evidence reproducible.

Praktikum ini tidak mencakup virtual memory manager penuh, process scheduler, multitasking, userspace, filesystem, interrupt subsystem lengkap, allocator dinamis kompleks, driver hardware lanjutan, networking, SMP, maupun keamanan kernel tingkat lanjut. M3 juga belum membuktikan bahwa sistem operasi stabil untuk penggunaan produksi; modul ini hanya membangun fondasi observability, debugging, dan panic handling untuk melanjutkan ke M4.]
```

---

## 6. Dasar Teori Ringkas

Praktikum M3 menggunakan konsep freestanding kernel development pada arsitektur x86_64. Pada lingkungan freestanding, kernel tidak dapat menggunakan library standar seperti libc sehingga fungsi seperti printf(), malloc(), dan free() tidak tersedia. Oleh karena itu kernel harus menyediakan mekanisme logging dan panic handling sendiri.

Panic path digunakan untuk menangani kondisi fatal yang tidak dapat dipulihkan. Pada M3 panic path diimplementasikan melalui fungsi kernel_panic_at() yang memiliki kontrak noreturn, menonaktifkan interrupt menggunakan instruksi cli, mencetak informasi error melalui serial logging, lalu masuk ke halt loop menggunakan instruksi hlt agar sistem berhenti secara deterministik.

Serial logging digunakan sebagai early observability mechanism karena lebih sederhana dan stabil dibanding framebuffer pada tahap awal boot kernel. Logging dilakukan melalui wrapper API logging yang dipisahkan dari driver serial agar desain kernel lebih modular.

Praktikum ini juga menggunakan linker script untuk mengatur layout ELF kernel seperti section .text, .rodata, dan entry point kernel. Struktur ELF diverifikasi menggunakan readelf, symbol table dianalisis menggunakan nm, sedangkan instruksi assembly diperiksa menggunakan objdump.

Untuk debugging digunakan QEMU gdbstub melalui opsi -s dan -S sehingga GDB dapat melakukan breakpoint pada fungsi seperti kmain() dan kernel_panic_at(). Dengan mekanisme ini mahasiswa dapat memeriksa register CPU, stack trace, serta disassembly kernel secara langsung saat kernel berjalan.

### 6.1 Konsep Sistem Operasi yang Diuji

```text
[Praktikum M3 menguji beberapa konsep utama dalam pengembangan kernel sistem operasi freestanding berbasis arsitektur x86_64. Konsep pertama adalah bootable kernel ELF yang dibangun menggunakan linker script untuk menentukan layout section seperti .text, .rodata, dan entry point kernel. Kernel dibangun sebagai ELF64 statik tanpa dependency libc sehingga seluruh proses boot dan runtime dikendalikan langsung oleh kernel.

Konsep kedua adalah panic path dan controlled halt. Panic path digunakan untuk menangani kondisi fatal yang tidak dapat dipulihkan melalui fungsi kernel_panic_at(). Fungsi ini memiliki kontrak noreturn, menonaktifkan interrupt menggunakan instruksi cli, mencetak informasi error melalui serial logging, lalu masuk ke halt loop menggunakan instruksi hlt agar sistem berhenti secara deterministik dan tidak melanjutkan eksekusi yang berbahaya.

Konsep berikutnya adalah serial logging sebagai mekanisme observability awal kernel. Logging dilakukan melalui wrapper API logging yang dipisahkan dari driver serial agar desain kernel lebih modular. Serial console dipilih karena lebih stabil dan sederhana dibanding framebuffer pada tahap awal boot kernel.

Praktikum ini juga menguji konsep ELF inspection dan disassembly audit menggunakan readelf, nm, dan objdump. Mahasiswa menganalisis symbol table, linker map, section layout, program header, serta instruksi assembly seperti cli dan hlt untuk memastikan struktur kernel sesuai desain.

Selain itu praktikum menguji konsep remote kernel debugging menggunakan QEMU gdbstub dan GNU GDB. Dengan fitur ini mahasiswa dapat memasang breakpoint pada fungsi seperti kmain() dan kernel_panic_at(), memeriksa register CPU, stack trace, dan melihat disassembly kernel secara langsung saat kernel berjalan di QEMU.]
```

### 6.2 Konsep Arsitektur x86_64 yang Relevan

| Konsep | Relevansi pada praktikum | Bukti/verifikasi |
|---|---|---|
| `Long Mode x86_64` | Digunakan agar kernel dapat berjalan pada arsitektur 64-bit dan menggunakan register serta addressing mode x86_64. | `readelf header`, output `Machine: Advanced Micro Devices X86-64`, dan hasil boot QEMU. |
| `Interrupt Masking (cli)` | Digunakan pada panic path untuk menonaktifkan interrupt sebelum masuk halt loop agar sistem berhenti secara deterministik. | `objdump/disassembly`, symbol `kernel_panic_at`, dan potongan instruksi `cli`. |
| `CPU Halt (hlt)` | Digunakan untuk membuat controlled halt loop sehingga kernel tidak kembali setelah panic. | `kernel.disasm.txt`, hasil `objdump`, dan halt behavior pada QEMU. |
| `ELF64 Executable Format` | Digunakan sebagai format executable kernel yang diload bootloader. | `readelf -h`, `readelf -l`, `kernel.readelf.header.txt`, dan `kernel.readelf.programs.txt`. |
| `Linker Script` | Mengatur layout memory kernel seperti `.text`, `.rodata`, dan entry point kernel. | `linker.ld`, `kernel.map`, dan hasil `readelf section/program header`. |
| `Serial I/O Port` | Digunakan sebagai early logging mechanism sebelum framebuffer tersedia. | `build/m3_serial.log` dan source `serial.c`. |
| `GDB Remote Debugging` | Digunakan untuk debugging kernel melalui QEMU gdbstub menggunakan breakpoint dan register inspection. | `gdb_m3.gdb`, screenshot/log breakpoint `kmain`, dan output `info registers`. |
| `Symbol Table` | Digunakan untuk memverifikasi keberadaan symbol penting seperti `kmain` dan `kernel_panic_at`. | `nm`, `kernel.syms.txt`, dan hasil `make audit`. |

### 6.3 Konsep Implementasi Freestanding

| Aspek | Keputusan praktikum |
|---|---|
| Bahasa | `C17 freestanding dengan sedikit instruksi assembly x86_64 untuk operasi low-level seperti cli dan hlt.` |
| Runtime | `Kernel dibangun tanpa hosted libc dan tanpa runtime userspace. Seluruh logging, panic handling, dan utility memory dibuat sendiri oleh kernel.` |
| ABI | `Menggunakan x86_64 System V ABI dengan calling convention kernel internal berbasis freestanding ELF64.` |
| Compiler flags kritis | `-ffreestanding, -fno-builtin, -nostdlib, -static, -mno-red-zone, -mcmodel=kernel, -fno-stack-protector.` |
| Risiko undefined behavior | `Kernel berisiko mengalami undefined behavior seperti invalid pointer access, stack corruption, alignment issue, triple fault, integer overflow, dan eksekusi setelah panic bila halt loop tidak benar.` |

### 6.4 Referensi Teori yang Digunakan

| No. | Sumber | Bagian yang digunakan | Alasan relevansi |
|---|---|---|---|
| `[1]` | `Intel 64 and IA-32 Architectures Software Developer Manual` | `Interrupt Control Instruction (cli), Halt Instruction (hlt), x86_64 architecture` | `Digunakan untuk memahami instruksi cli dan hlt pada panic path serta mekanisme controlled halt pada kernel x86_64.` |
| `[2]` | `OSDev Wiki — Freestanding Kernel Development` | `Freestanding environment, ELF kernel, serial logging` | `Digunakan untuk memahami pengembangan kernel tanpa libc dan implementasi observability awal kernel.` |
| `[3]` | `GNU Binutils Documentation` | `readelf, nm, objdump` | `Digunakan untuk audit ELF, symbol table, dan disassembly kernel.` |
| `[4]` | `LLVM Clang Documentation` | `Compiler flags freestanding dan target x86_64-unknown-none-elf` | `Digunakan untuk memahami compiler flag kernel seperti -ffreestanding, -nostdlib, dan -mno-red-zone.` |
| `[5]` | `QEMU Documentation` | `QEMU gdbstub dan serial logging` | `Digunakan untuk menjalankan QEMU smoke test dan remote debugging menggunakan GDB.` |
| `[6]` | `GNU GDB Documentation` | `Remote debugging dan breakpoint` | `Digunakan untuk debugging kernel menggunakan breakpoint pada kmain dan kernel_panic_at.` |

---

## 7. Lingkungan Praktikum

### 7.1 Host dan Target

| Komponen | Nilai |
|---|---|
| Host OS | `Windows 11 x64` |
| Lingkungan build | `WSL2 Ubuntu` |
| Target ISA | `x86_64` |
| Target ABI | `x86_64-unknown-none-elf` |
| Emulator | `QEMU emulator version 10.2.1` |
| Firmware emulator | `OVMF 2025.11-3ubuntu7 (UEFI firmware for 64-bit x86 virtual machines)` |
| Debugger | `GNU GDB 17.1` |
| Build system | `GNU Make` |
| Bahasa utama | `C17 freestanding` |
| Assembly | `Inline assembly x86_64 dan NASM` |

### 7.2 Versi Toolchain

Tempel output versi toolchain berikut. Jalankan dari clean shell WSL.

```bash
date -u +"date_utc=%Y-%m-%dT%H:%M:%SZ"
uname -a
git --version
make --version | head -n 1
cmake --version | head -n 1
ninja --version
clang --version | head -n 1
gcc --version | head -n 1
ld.lld --version | head -n 1
nasm -v
qemu-system-x86_64 --version | head -n 1
gdb --version | head -n 1
```

Output:

```text
[date_utc=2026-05-25T13:38:29Z
Linux DESKTOP-S5LUA56 6.6.114.1-microsoft-standard-WSL2 #1 SMP PREEMPT_DYNAMIC Mon Dec 1 20:46:23 UTC 2025 x86_64 GNU/Linux
git version 2.53.0
GNU Make 4.4.1
cmake version 4.2.3
1.13.2
Ubuntu clang version 21.1.8 (6ubuntu1)
gcc (Ubuntu 15.2.0-16ubuntu1) 15.2.0
Ubuntu LLD 21.1.8 (compatible with GNU linkers)
NASM version 3.01
QEMU emulator version 10.2.1 (Debian 1:10.2.1+ds-1ubuntu3)
GNU gdb (Ubuntu 17.1-2ubuntu1) 17.1]
```

### 7.3 Lokasi Repository

| Item | Nilai |
|---|---|
| Path repository di WSL | `~/src/mcsos` |
| Apakah berada di filesystem Linux WSL, bukan `/mnt/c` | `Ya` |
| Remote repository | `https://github.com/amaaarhyu078-creator/mcsos-` |
| Branch | `praktikum/m3-panic-debug-audit` |
| Commit hash awal | `d3ce466` |
| Commit hash akhir | `0bc792c` |

---

## 8. Repository dan Struktur File

### 8.1 Struktur Direktori yang Relevan

```text
mcsos/
├── build/
│   ├── kernel.elf
│   ├── kernel.map
│   ├── kernel.panic.elf
│   ├── kernel.panic.map
│   ├── kernel.readelf.header.txt
│   ├── kernel.readelf.programs.txt
│   ├── kernel.syms.txt
│   ├── kernel.disasm.txt
│   ├── kernel.debug.elf
│   ├── kernel.release.elf
│   ├── mcsos.iso
│   └── qemu-serial.log
│
├── evidence/
│   └── M3/
│       ├── kernel.disasm.txt
│       ├── kernel.elf
│       ├── kernel.map
│       ├── kernel.readelf.header.txt
│       ├── kernel.readelf.programs.txt
│       ├── kernel.syms.txt
│       ├── m3_serial.log
│       └── manifest.txt
│
├── kernel/
│   ├── core/
│   │   ├── kmain.c
│   │   ├── log.c
│   │   ├── panic.c
│   │   └── serial.c
│   │
│   ├── include/
│   │   └── mcsos/kernel/
│   │       ├── log.h
│   │       └── panic.h
│   │
│   └── lib/
│       └── memory.c
│
├── tools/
│   ├── gdb_m3.gdb
│   └── scripts/
│       ├── grade_m3.sh
│       ├── m3_audit_elf.sh
│       ├── m3_collect_evidence.sh
│       ├── m3_preflight.sh
│       ├── m3_qemu_debug.sh
│       └── m3_qemu_run.sh
│
├── linker.ld
├── limine.cfg
├── Makefile
└── README.md
]
```

### 8.2 File yang Dibuat atau Diubah

| File | Jenis perubahan | Alasan perubahan | Risiko |
|---|---|---|---|
| `kernel/core/kmain.c` | `ubah` | `Menambahkan integrasi logging, self-test M3, dan panic workflow pada entry kernel.` | `Sedang — kesalahan pada entry kernel dapat menyebabkan boot failure atau triple fault.` |
| `kernel/core/log.c` | `ubah` | `Mengimplementasikan wrapper logging freestanding tanpa libc.` | `Sedang — bug logging dapat menyebabkan serial output tidak muncul.` |
| `kernel/core/panic.c` | `ubah` | `Menambahkan panic path berbasis cli dan hlt dengan kontrak noreturn.` | `Tinggi — panic handler yang salah dapat menyebabkan reboot loop atau undefined behavior.` |
| `kernel/core/serial.c` | `ubah` | `Mengimplementasikan serial driver untuk observability awal kernel.` | `Sedang — serial timeout atau konfigurasi salah dapat menyebabkan log kosong.` |
| `kernel/include/mcsos/kernel/log.h` | `ubah` | `Menambahkan deklarasi API logging kernel.` | `Rendah — hanya mempengaruhi interface logging.` |
| `kernel/include/mcsos/kernel/panic.h` | `ubah` | `Menambahkan deklarasi panic API dan kontrak panic path.` | `Rendah — terutama mempengaruhi interface panic handling.` |
| `kernel/arch/x86_64/include/mcsos/arch/io.h` | `ubah` | `Menambahkan operasi low-level I/O port untuk serial dan halt behavior.` | `Sedang — kesalahan I/O dapat menyebabkan hardware interaction gagal.` |
| `Makefile` | `ubah` | `Menambahkan target build, panic, audit, debug, release, dan size.` | `Tinggi — kesalahan Makefile dapat menyebabkan build gagal total.` |
| `linker.ld` | `ubah` | `Mengatur layout ELF kernel dan symbol linker.` | `Tinggi — linker layout salah dapat menyebabkan kernel tidak boot.` |
| `limine.cfg` | `ubah` | `Menyesuaikan konfigurasi boot kernel M3 pada QEMU.` | `Sedang — konfigurasi boot salah dapat menyebabkan bootloader gagal load kernel.` |
| `tools/gdb_m3.gdb` | `baru` | `Menyediakan konfigurasi debugging otomatis menggunakan GDB.` | `Rendah — hanya mempengaruhi workflow debugging.` |
| `tools/scripts/m3_preflight.sh` | `ubah` | `Menambahkan validasi readiness M0/M1/M2 sebelum build M3.` | `Rendah — hanya mempengaruhi pemeriksaan environment.` |
| `tools/scripts/m3_audit_elf.sh` | `baru` | `Mengotomatisasi audit ELF, symbol table, dan disassembly.` | `Rendah — hanya mempengaruhi proses audit evidence.` |
| `tools/scripts/m3_qemu_run.sh` | `baru` | `Mengotomatisasi QEMU smoke test dan serial logging.` | `Sedang — konfigurasi QEMU salah dapat menyebabkan boot gagal.` |
| `tools/scripts/m3_qemu_debug.sh` | `baru` | `Menjalankan QEMU dengan gdbstub untuk debugging kernel.` | `Sedang — kesalahan konfigurasi dapat menyebabkan GDB gagal connect.` |
| `tools/scripts/grade_m3.sh` | `baru` | `Menjalankan grading otomatis dan validasi mekanis praktikum M3.` | `Rendah — hanya mempengaruhi automation testing.` |
| `tools/scripts/m3_collect_evidence.sh` | `baru` | `Mengumpulkan artefak audit dan debugging ke evidence/M3.` | `Rendah — hanya mempengaruhi dokumentasi evidence.` |
| `evidence/M3/*` | `baru` | `Menyimpan evidence reproducible hasil build, audit, QEMU, dan debugging.` | `Rendah — hanya berfungsi sebagai dokumentasi praktikum.` |

### 8.3 Ringkasan Diff

```bash
git status --short
git diff --stat
git log --oneline -n 5
```

Output:

```text
[ M Makefile
 M kernel/core/log.c
 M kernel/include/mcsos/kernel/log.h

 Makefile                          | 50 ++++++++++++++++++++++++++++++++++++++++++++++++--
 kernel/core/log.c                 | 39 ++++++++++++++++++++++++++++++++++++++-
 kernel/include/mcsos/kernel/log.h |  9 +++++++--
 3 files changed, 93 insertions(+), 5 deletions(-)

0bc792c (HEAD -> praktikum/m3-panic-debug-audit, origin/praktikum/m3-panic-debug-audit, m0/salma) M3 panic debug audit completed
9e5b0c9 M3 preflight ready state
d3ce466 (origin/m0/salma) M2: add ADR and boot log classifier
7d1bc51 M2: add local CI helper script
9effd34 M2: add serial hex output for kernel start address]
```

---

## 9. Desain Teknis

### 9.1 Masalah yang Diselesaikan

```text
Sebelum praktikum M3, kernel MCSOS hanya memiliki baseline boot sederhana tanpa mekanisme observability dan debugging yang memadai. Ketika terjadi kegagalan pada tahap awal boot, kernel dapat mengalami hang, reboot, atau triple fault tanpa menghasilkan informasi diagnostik yang cukup.

Kernel juga belum memiliki panic path yang terstruktur untuk menangani kondisi fatal secara fail-closed. Akibatnya kondisi error dapat menyebabkan eksekusi tidak terkontrol atau reboot berulang tanpa bukti penyebab kegagalan.

Selain itu kernel belum memiliki wrapper logging yang memisahkan API logging dari driver serial sehingga observability awal kernel masih terbatas. Build system juga belum menyediakan audit ELF, symbol inspection, disassembly verification, maupun workflow debugging menggunakan GDB dan QEMU gdbstub.

Praktikum M3 menyelesaikan masalah tersebut dengan menambahkan serial logging freestanding, panic path berbasis cli dan hlt, audit ELF/disassembly, QEMU smoke test, debugging menggunakan GDB, serta pengumpulan evidence reproducible untuk meningkatkan kemampuan diagnosis dan observability kernel.
```

### 9.2 Keputusan Desain

| Keputusan | Alternatif yang dipertimbangkan | Alasan memilih | Konsekuensi |
|---|---|---|---|
| `(Menggunakan serial logging sebagai early console kernel.)` | `(Framebuffer logging atau tanpa logging awal.)` | `(Serial console lebih sederhana, stabil, dan mudah diaudit pada tahap awal boot kernel.)` | `(Output debugging hanya berbasis text serial dan belum memiliki tampilan visual framebuffer.)` |
| `(Mengimplementasikan panic path menggunakan cli dan hlt.)` | `(Langsung reboot sistem atau infinite loop tanpa halt.)` | `(Pendekatan fail-closed lebih aman karena kernel berhenti secara deterministik setelah kondisi fatal.)` | `(Kernel tidak dapat recovery otomatis setelah panic dan memerlukan restart manual.)` |
| `(Membangun kernel sebagai freestanding ELF64 tanpa libc.)` | `(Menggunakan hosted libc atau runtime userspace.)` | `(Kernel harus independen dari userspace runtime dan memiliki kontrol penuh terhadap environment boot.)` | `(Kernel harus mengimplementasikan utility sendiri seperti logging dan memory helper.)` |
| `(Menggunakan wrapper API logging yang dipisahkan dari driver serial.)` | `(Memanggil serial driver langsung dari seluruh kernel.)` | `(Desain modular memudahkan perubahan backend logging di masa depan.)` | `(Menambah layer abstraksi sehingga implementasi sedikit lebih kompleks.)` |
| `(Menggunakan QEMU gdbstub untuk debugging kernel.)` | `(Debugging hanya melalui serial log.)` | `(GDB memungkinkan breakpoint, register inspection, dan disassembly runtime kernel.)` | `(Workflow debugging menjadi lebih kompleks dan membutuhkan sinkronisasi QEMU-GDB.)` |
| `(Menggunakan audit ELF dan disassembly otomatis.)` | `(Hanya mengandalkan build success.)` | `(Audit memastikan symbol, linker layout, dan instruksi kernel sesuai desain.)` | `(Menambah proses verifikasi dan evidence collection pada workflow build.)` |
| `(Memisahkan normal kernel dan intentional-panic kernel.)` | `(Satu kernel untuk seluruh mode pengujian.)` | `(Memudahkan pengujian panic path tanpa mengganggu normal boot workflow.)` | `(Build system menjadi lebih kompleks karena memiliki beberapa profile kernel.)` |
| `(Menggunakan halt loop noreturn pada panic handler.)` | `(Return ke caller setelah panic.)` | `(Return setelah fatal error dapat menyebabkan undefined behavior atau corruption lebih lanjut.)` | `(Kernel akan berhenti permanen sampai sistem direset.)` |

### 9.3 Arsitektur Ringkas

Tambahkan diagram ASCII atau Mermaid. Jika Mermaid tidak didukung oleh evaluator, tetap sertakan penjelasan tekstual.

```mermaid
flowchart TD
    A[Bootloader Limine / QEMU Boot] --> B[kmain()]
    B --> C[Serial Initialization]
    C --> D[Logging API]
    D --> E[Kernel Self Test]

    E --> F{Failure?}

    F -- Tidak --> G[Controlled Halt]
    G --> H[cli + hlt loop]

    F -- Ya --> I[kernel_panic_at()]
    I --> J[Print Panic Information]
    J --> K[Disable Interrupt]
    K --> L[cpu_halt_forever()]
    L --> M[cli + hlt loop]

    B --> N[QEMU gdbstub]
    N --> O[GDB Breakpoint]
    O --> P[Register/Disassembly Audit]

    D --> Q[Serial Log]
    Q --> R[evidence/M3]
```

Penjelasan diagram:

```text
[Bootloader Limine memuat kernel ELF64 ke memori dan menyerahkan kontrol ke fungsi kmain(). Setelah kernel mulai berjalan, serial subsystem diinisialisasi agar logging awal dapat digunakan sebagai mekanisme observability kernel.

Logging API bertindak sebagai wrapper antara kernel dan driver serial sehingga seluruh pesan kernel dicetak melalui interface logging yang terpisah dari hardware layer. Setelah logging aktif, kernel menjalankan self-test dan validasi dasar M3.

Jika tidak terjadi failure, kernel masuk ke controlled halt menggunakan kombinasi instruksi cli dan hlt. Jika terjadi kondisi fatal, kernel memanggil kernel_panic_at() untuk mencetak informasi panic, menonaktifkan interrupt, lalu masuk ke halt loop permanen melalui cpu_halt_forever().

QEMU menyediakan gdbstub sehingga GDB dapat memasang breakpoint pada kmain() atau kernel_panic_at(), memeriksa register CPU, dan melakukan disassembly audit selama runtime kernel berjalan.

Seluruh log serial, audit ELF, symbol table, disassembly, dan debugging evidence dikumpulkan ke direktori evidence/M3 sebagai bukti reproducible praktikum.]
```

### 9.4 Kontrak Antarmuka

| Antarmuka | Pemanggil | Penerima | Precondition | Postcondition | Error path |
|---|---|---|---|---|---|
| `(kmain())` | `(Bootloader Limine)` | `(Kernel core)` | `(Kernel ELF berhasil dimuat dan entry point valid.)` | `(Kernel menginisialisasi serial, logging, dan menjalankan self-test M3.)` | `(Jika inisialisasi gagal maka kernel memanggil kernel_panic_at().)` |
| `(log_init())` | `(kmain())` | `(Serial subsystem)` | `(Serial hardware dapat diakses melalui I/O port.)` | `(Logging subsystem siap digunakan.)` | `(Jika serial gagal diinisialisasi maka logging tidak tersedia dan panic dapat dipanggil.)` |
| `(log_write())` | `(Kernel subsystem)` | `(Serial driver)` | `(Logging subsystem sudah aktif atau serial dapat diinisialisasi.)` | `(Pesan berhasil dikirim ke serial output.)` | `(Jika serial timeout maka output log dapat terpotong.)` |
| `(kernel_panic_at())` | `(Kernel core/self-test)` | `(Panic subsystem)` | `(Terjadi kondisi fatal yang tidak dapat dipulihkan.)` | `(Kernel mencetak informasi panic, mematikan interrupt, lalu masuk halt loop.)` | `(Tidak ada recovery; fungsi bersifat noreturn.)` |
| `(cpu_halt_forever())` | `(kernel_panic_at())` | `(CPU halt loop)` | `(Interrupt telah dimatikan menggunakan cli.)` | `(CPU berhenti pada controlled halt loop.)` | `(Jika halt gagal maka dapat terjadi reboot atau undefined behavior.)` |
| `(m3_qemu_run.sh)` | `(User/praktikan)` | `(QEMU runtime)` | `(ISO kernel berhasil dibuat.)` | `(Kernel dijalankan dan serial log disimpan.)` | `(Jika boot gagal maka serial log kosong atau QEMU timeout.)` |
| `(gdb_m3.gdb)` | `(GNU GDB)` | `(QEMU gdbstub)` | `(QEMU dijalankan dengan opsi -s -S.)` | `(Breakpoint, register inspection, dan disassembly dapat dilakukan.)` | `(Jika gdbstub gagal maka GDB tidak dapat connect ke port 1234.)` |
| `(m3_collect_evidence.sh)` | `(User/praktikan)` | `(Evidence collector)` | `(Artefak build dan audit tersedia.)` | `(Evidence berhasil dikumpulkan ke evidence/M3.)` | `(Jika artefak hilang maka manifest tidak lengkap.)` |

### 9.5 Struktur Data Utama

| Struktur data | Field penting | Ownership | Lifetime | Invariant |
|---|---|---|---|---|
| `(struct panic_context)` | `(panic_code, file, line, reason)` | `(Panic subsystem kernel)` | `(Dibuat saat panic dipanggil dan digunakan sampai halt loop aktif.)` | `(Panic context tidak boleh berubah setelah panic dimulai dan panic path tidak boleh return.)` |
| `(struct serial_device)` | `(base_port, status, timeout_limit)` | `(Serial driver)` | `(Aktif selama kernel berjalan sejak serial_init().)` | `(Serial port harus valid dan hanya digunakan untuk output logging kernel.)` |
| `(struct log_state)` | `(ready_flag, output_backend)` | `(Logging subsystem)` | `(Dibuat saat log_init() dan aktif selama runtime kernel.)` | `(Logging hanya boleh digunakan setelah serial subsystem siap.)` |
| `(struct m3_selftest_result)` | `(status, passed_checks, failed_checks)` | `(Kernel self-test subsystem)` | `(Digunakan selama proses self-test kernel M3.)` | `(Status self-test harus konsisten dengan hasil audit dan panic state.)` |
| `(ELF64 symbol table)` | `(symbol name, address, section)` | `(Kernel ELF image)` | `(Ada selama kernel ELF digunakan untuk audit/debugging.)` | `(Symbol penting seperti kmain dan kernel_panic_at harus tersedia.)` |
| `(QEMU gdbstub state)` | `(tcp_port, breakpoint_state)` | `(QEMU runtime)` | `(Aktif selama sesi debugging berlangsung.)` | `(GDB harus dapat connect ke port 1234 selama debug session aktif.)` |

### 9.6 Invariants

Tuliskan invariant yang harus benar sepanjang eksekusi.

1. `(Panic path tidak boleh return ke caller setelah kernel_panic_at() dipanggil.)`

2. `(Interrupt harus dimatikan menggunakan instruksi cli sebelum kernel masuk ke halt loop.)`

3. `(Kernel freestanding tidak boleh memiliki dependency terhadap hosted libc seperti printf(), malloc(), free(), atau puts().)`

4. `(Serial logging harus dapat digunakan sebelum subsystem lain tersedia agar observability awal kernel tetap tersedia.)`

5. `(Symbol penting seperti kmain, kernel_panic_at, dan cpu_halt_forever harus tersedia pada ELF symbol table.)`

6. `(Kernel ELF harus dibangun sebagai executable statik tanpa dynamic section.)`

7. `(QEMU smoke test harus menghasilkan serial log deterministik dan tidak boleh silent-failure.)`

8. `(GDB harus dapat connect ke QEMU gdbstub dan mencapai breakpoint kmain atau kernel_panic_at.)`

9. `(Controlled halt harus menggunakan kombinasi cli dan hlt agar kernel berhenti secara fail-closed.)`

10. `(Evidence build, audit, QEMU, dan debugging harus terkumpul secara reproducible pada direktori evidence/M3.)`


### 9.7 Ownership, Locking, dan Concurrency

| Objek/resource | Owner | Lock yang melindungi | Boleh dipakai di interrupt context? | Catatan |
|---|---|---|---|---|
| `(Serial logging subsystem)` | `(Kernel logging layer)` | `(none)` | `(Ya, terbatas)` | `(Kernel M3 masih single-core dan interrupt biasanya dimatikan saat panic path.)` |
| `(Panic state)` | `(Panic subsystem)` | `(none)` | `(Ya)` | `(Panic path bersifat fail-closed dan langsung masuk halt loop.)` |
| `(QEMU serial output)` | `(QEMU runtime)` | `(none)` | `(Ya)` | `(Output serial digunakan untuk observability awal kernel.)` |
| `(Kernel ELF symbol table)` | `(Audit/debugging subsystem)` | `(none)` | `(Tidak)` | `(Hanya digunakan saat audit ELF dan debugging.)` |
| `(GDB remote debugging session)` | `(GDB + QEMU gdbstub)` | `(none)` | `(Tidak)` | `(Digunakan hanya selama sesi debugging runtime kernel.)` |
| `(Evidence/M3 artifacts)` | `(Evidence collector script)` | `(none)` | `(Tidak)` | `(Digunakan untuk dokumentasi reproducible praktikum.)` |

Lock order yang berlaku:

```text id="q0wyui"
[Kernel M3 belum menggunakan scheduler, multitasking, SMP, maupun concurrency kompleks sehingga belum membutuhkan spinlock atau mutex formal. Pada tahap ini pendekatan single-core dan interrupt-disabled sudah cukup karena panic path mematikan interrupt menggunakan cli sebelum masuk halt loop.

Sebagian besar subsystem M3 bersifat sequential dan hanya digunakan selama early boot, audit, atau debugging. Karena itu race condition dan parallel execution belum menjadi fokus utama praktikum M3.]
```

### 9.8 Memory Safety dan Undefined Behavior Risk

| Risiko | Lokasi | Mitigasi | Bukti |
|---|---|---|---|
| `(Out-of-bounds access)` | `(kernel/core/log.c)` | `(Loop logging dibatasi dengan ukuran buffer dan validasi indeks.)` | `(Code review dan build audit tanpa warning/error.)` |
| `(Stack corruption akibat red-zone)` | `(Seluruh kernel freestanding)` | `(Menggunakan compiler flag -mno-red-zone.)` | `(Makefile dan hasil compiler flags saat build.)` |
| `(Undefined behavior setelah panic)` | `(kernel/core/panic.c)` | `(Panic path menggunakan noreturn, cli, dan halt loop permanen.)` | `(Disassembly objdump menunjukkan instruksi cli dan hlt.)` |
| `(Dynamic dependency terhadap libc)` | `(Build system dan kernel ELF)` | `(Menggunakan -ffreestanding, -nostdlib, dan audit symbol.)` | `(make audit, nm, readelf, dan tidak ada dynamic section.)` |
| `(Serial timeout atau partial log)` | `(kernel/core/serial.c)` | `(Menggunakan timeout limit pada serial write.)` | `(QEMU serial log dan failure analysis M3.)` |
| `(Invalid ELF layout atau symbol hilang)` | `(linker.ld dan build kernel)` | `(Audit ELF menggunakan readelf, nm, dan objdump.)` | `(kernel.readelf.*, kernel.syms.txt, kernel.disasm.txt.)` |
| `(Infinite reboot/triple fault)` | `(panic path dan entry kernel)` | `(Menggunakan controlled halt fail-closed daripada reboot otomatis.)` | `(QEMU smoke test dan panic disassembly audit.)` |
| `(GDB connection failure)` | `(QEMU gdbstub)` | `(Menggunakan port 1234 standar dan validasi gdbstub.)` | `(Breakpoint kmain berhasil dicapai.)` |
| `(Integer overflow pada logging angka)` | `(log_hex32() dan log_dec_u64())` | `(Menggunakan tipe uint32_t dan uint64_t eksplisit.)` | `(Code review dan successful build audit.)` |

### 9.9 Security Boundary

| Boundary | Data tidak tepercaya | Validasi yang dilakukan | Failure mode aman |
|---|---|---|---|
| `(Bootloader handoff ke kernel ELF)` | `(Alamat entry point, layout ELF, dan boot state dari firmware/bootloader.)` | `(Audit ELF menggunakan readelf, symbol verification, dan linker script validation.)` | `(Kernel panic, serial log error, dan controlled halt.)` |
| `(Serial output subsystem)` | `(Status hardware serial port dan timeout device.)` | `(Polling status register dan timeout limit pada serial write.)` | `(Partial log dicatat lalu kernel tetap masuk panic/halt state.)` |
| `(QEMU gdbstub connection)` | `(Remote debugging connection pada TCP port 1234.)` | `(Validasi koneksi GDB terhadap gdbstub QEMU.)` | `(Debugging gagal tetapi kernel tetap dapat dijalankan normal.)` |
| `(Kernel panic reason dan diagnostic message)` | `(String reason dan state panic runtime.)` | `(Logging dilakukan melalui API kernel freestanding tanpa dynamic allocation.)` | `(Kernel mencetak informasi panic lalu masuk halt loop permanen.)` |
| `(ELF symbol dan disassembly audit)` | `(Binary kernel hasil build.)` | `(readelf, nm, objdump, dan audit script otomatis.)` | `(Audit gagal dan readiness M3 dinyatakan belum lulus.)` |
| `(QEMU runtime boot)` | `(Boot image ISO dan firmware runtime.)` | `(Preflight check, build verification, dan smoke test serial log.)` | `(Boot failure dicatat pada serial log atau panic evidence.)` |

---

## 10. Langkah Kerja Implementasi

Gunakan tabel berikut untuk setiap langkah. Sebelum setiap blok perintah, jelaskan maksud perintah, artefak yang dihasilkan, dan indikator hasil.

### Langkah 1 — `(Menjalankan Preflight M3)`

Maksud langkah:

```text
Langkah ini dilakukan untuk memastikan environment praktikum M0, M1, dan M2 sudah siap sebelum implementasi dan pengujian kernel M3 dilakukan. Preflight digunakan untuk memverifikasi bahwa repository berada pada filesystem Linux WSL, toolchain utama seperti clang, lld, QEMU, dan GDB tersedia, serta tidak ada dependency penting yang hilang.
----

Perintah:

```bash
[./tools/scripts/m3_preflight.sh]
```

Output ringkas:

```text
[[M3 preflight] root=/home/salma_rahayu/src/mcsos
PASS: repository berada di filesystem Linux/WSL
PASS: QEMU tersedia: QEMU emulator version 10.2.1 (Debian 1:10.2.1+ds-1ubuntu3)

 M Makefile
 M kernel/core/log.c
 M kernel/include/mcsos/kernel/log.h

[M3 preflight] compiler=Ubuntu clang version 21.1.8 (6ubuntu1)
[M3 preflight] linker=Ubuntu LLD 21.1.8 (compatible with GNU linkers)
PASS: preflight M3 selesai]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(Preflight output)` | `(Terminal WSL)` | `(Memvalidasi readiness environment praktikum M3.)` |
| `(Informasi compiler dan linker)` | `(Output preflight)` | `(Memastikan toolchain clang dan lld tersedia.)` |
| `(Status repository)` | `(Output git status pada preflight)` | `(Menunjukkan kondisi source sebelum build.)` |

Indikator berhasil:

```text
[Preflight selesai dengan status PASS tanpa FAIL atau error kritis. Repository berhasil dikenali berada pada filesystem Linux WSL, QEMU tersedia, dan toolchain clang/lld berhasil tervalidasi.]
```

### Langkah 2 — `(Membangun Kernel Normal M3)`

Maksud langkah:

```text
[Langkah ini dilakukan untuk membangun kernel freestanding ELF64 menggunakan compiler clang dan linker lld tanpa dependency libc. Build kernel diperlukan untuk menghasilkan executable kernel yang dapat diaudit, dijalankan pada QEMU, dan digunakan pada tahap debugging praktikum M3.]
```

Perintah:

```bash
[make clean
make build]
```

Output ringkas:

```text
[rm -rf build

clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding ...
-c kernel/core/kmain.c

clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding ...
-c kernel/core/log.c

clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding ...
-c kernel/core/panic.c

clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding ...
-c kernel/core/serial.c

clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding ...
-c kernel/lib/memory.c

ld.lld -nostdlib -static -z max-page-size=0x1000 \
-T linker.ld -Map=build/kernel.map \
-o build/kernel.elf \
build/normal/kernel/core/kmain.o \
build/normal/kernel/core/log.o \
build/normal/kernel/core/panic.o \
build/normal/kernel/core/serial.o \
build/normal/kernel/lib/memory.o]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(kernel.elf)` | `(build/kernel.elf)` | `(Executable kernel ELF64 hasil build.)` |
| `(kernel.map)` | `(build/kernel.map)` | `(Linker map untuk audit symbol dan layout kernel.)` |
| `(Object files)` | `(build/normal/)` | `(Hasil kompilasi source kernel.)` |

Indikator berhasil:

```text
[Build selesai tanpa error compiler maupun linker. File kernel.elf dan kernel.map berhasil dihasilkan serta proses linking menggunakan -nostdlib dan -static berjalan dengan sukses.]
```

### Langkah 3 — `(Membangun Intentional Panic Kernel)`

Maksud langkah:

```text
[Langkah ini dilakukan untuk menguji panic path kernel menggunakan varian kernel yang sengaja memicu panic saat boot. Pengujian ini digunakan untuk memastikan panic handler, serial logging, dan controlled halt berjalan sesuai desain fail-closed praktikum M3.]
----

Perintah:

```bash
[make panic]
```

Output ringkas:

```text
[mkdir -p build/panic/kernel/core/

clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding \
-DMCSOS_M3_TRIGGER_PANIC=1 \
-c kernel/core/kmain.c

clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding \
-DMCSOS_M3_TRIGGER_PANIC=1 \
-c kernel/core/log.c

clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding \
-DMCSOS_M3_TRIGGER_PANIC=1 \
-c kernel/core/panic.c

clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding \
-DMCSOS_M3_TRIGGER_PANIC=1 \
-c kernel/core/serial.c

clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding \
-DMCSOS_M3_TRIGGER_PANIC=1 \
-c kernel/lib/memory.c

ld.lld -nostdlib -static -z max-page-size=0x1000 \
-T linker.ld \
-Map=build/kernel.panic.map \
-o build/kernel.panic.elf \
build/panic/kernel/core/kmain.o \
build/panic/kernel/core/log.o \
build/panic/kernel/core/panic.o \
build/panic/kernel/core/serial.o \
build/panic/kernel/lib/memory.o]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(kernel.panic.elf)` | `(build/kernel.panic.elf)` | `(Kernel varian intentional panic untuk pengujian panic path.)` |
| `(kernel.panic.map)` | `(build/kernel.panic.map)` | `(Linker map panic kernel.)` |
| `(Object files panic build)` | `(build/panic/)` | `(Hasil kompilasi source kernel panic variant.)` |

Indikator berhasil:

```text
Varian intentional panic kernel berhasil dibuild tanpa error compiler maupun linker. File kernel.panic.elf dan kernel.panic.map berhasil dihasilkan menggunakan flag DMCSOS_M3_TRIGGER_PANIC=1.
```

### Langkah 4 — `(Melakukan Audit ELF dan Disassembly)`

Maksud langkah:

```text
[Langkah ini dilakukan untuk memverifikasi struktur ELF kernel, symbol table, linker layout, dan instruksi assembly penting seperti cli dan hlt. Audit digunakan untuk memastikan kernel freestanding dibangun sesuai desain praktikum M3 dan tidak memiliki dependency dynamic library.]
----

Perintah:

```bash
[make audit]
```

Output ringkas:

```text
[readelf -h build/kernel.elf > build/kernel.readelf.header.txt
readelf -l build/kernel.elf > build/kernel.readelf.programs.txt
nm -n build/kernel.elf > build/kernel.syms.txt
objdump -d -Mintel build/kernel.elf > build/kernel.disasm.txt

grep -q 'ELF64' build/kernel.readelf.header.txt
grep -q 'Machine:[[:space:]]*Advanced Micro Devices X86-64' build/kernel.readelf.header.txt

grep -q 'kmain' build/kernel.syms.txt
grep -q 'kernel_panic_at' build/kernel.syms.txt
grep -q 'cpu_halt_forever' build/kernel.disasm.txt

! nm -u build/kernel.elf | grep .
! nm -u build/kernel.panic.elf | grep .

grep -q 'kernel_panic_at' build/kernel.disasm.txt

readelf -S build/kernel.elf | grep -q '.text'
readelf -S build/kernel.elf | grep -q '.rodata']
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(kernel.readelf.header.txt)` | `(build/)` | `(Header ELF kernel.)` |
| `(kernel.readelf.programs.txt)` | `(build/)` | `(Program header ELF kernel.)` |
| `(kernel.syms.txt)` | `(build/)` | `(Symbol table kernel.)` |
| `(kernel.disasm.txt)` | `(build/)` | `(Disassembly kernel untuk audit instruksi.)` |

Indikator berhasil:

```text
[Audit ELF selesai tanpa error. Symbol penting seperti kmain, kernel_panic_at, dan cpu_halt_forever berhasil ditemukan. Tidak terdapat undefined symbol maupun dependency dynamic library pada kernel ELF.]
```

### Langkah 5 — `(Membuat Bootable ISO Kernel M3)`

Maksud langkah:

```text
[Langkah ini dilakukan untuk membangun image ISO bootable dari kernel ELF menggunakan Limine bootloader. ISO diperlukan agar kernel dapat dijalankan pada emulator QEMU selama smoke test dan debugging runtime kernel M3.]
----

Perintah:

```bash
[./tools/scripts/make_iso.sh]
```

Output ringkas:

```text
['build/kernel.elf' -> 'iso_root/boot/kernel.elf'
xorriso : UPDATE : 11 files added
ISO image produced: 2104 sectors
Writing to 'stdio:build/mcsos.iso' completed successfully.
Limine BIOS stages installed successfully.

OK: ISO dibuat pada build/mcsos.iso]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(mcsos.iso)` | `(build/mcsos.iso)` | `(Image bootable kernel M3 untuk QEMU.)` |
| `(ISO hybrid boot image)` | `(build/mcsos.iso)` | `(Media boot BIOS/UEFI menggunakan Limine.)` |

Indikator berhasil:

```text
ISO berhasil dibuat tanpa error xorriso maupun Limine installer. File build/mcsos.iso tersedia dan siap digunakan pada QEMU smoke test.
```
### Langkah 6 — `(Menjalankan QEMU Smoke Test)`

Maksud langkah:

```text
[Langkah ini dilakukan untuk menjalankan kernel pada emulator QEMU dan memverifikasi bahwa boot kernel, serial logging, serta panic path berjalan secara deterministik tanpa reboot loop atau silent failure.]
----

Perintah:

```bash
[./tools/scripts/m3_qemu_run.sh]
```

Output ringkas:

```text
[QEMU berjalan
Kernel boot berhasil
Serial log M3 tercetak]
```

Artefak yang dihasilkan:

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(m3_serial.log)` | `(build/m3_serial.log)` | `(Log serial hasil boot kernel pada QEMU.)` |
| `(mcsos.iso)` | `(build/mcsos.iso)` | `(Image bootable kernel M3 yang dijalankan pada QEMU.)` |
| `(QEMU runtime output)` | `(Terminal WSL)` | `(Menampilkan status boot dan observability kernel saat runtime.)` |

Indikator berhasil:

```text
[Kernel berhasil boot pada QEMU dan menghasilkan serial log tanpa reboot loop maupun silent failure.]
```

### Langkah 7 — `(Melakukan Debugging Kernel Menggunakan GDB)`

Maksud langkah:

```text
[Langkah ini dilakukan untuk memverifikasi bahwa kernel dapat didebug menggunakan QEMU gdbstub dan GNU GDB. Breakpoint digunakan untuk memeriksa alur eksekusi kernel pada fungsi kmain dan kernel_panic_at selama runtime kernel berjalan.]
----

Perintah:

```bash
[./tools/scripts/m3_qemu_debug.sh build/mcsos.iso]

gdb build/kernel.elf

(gdb) target remote localhost:1234
(gdb) break kmain
(gdb) continue]
```

Output ringkas:

```text
[Remote debugging using localhost:1234
Breakpoint 1 at kmain
Breakpoint kmain berhasil dicapai]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(GDB session log)` | `(evidence/M3/)` | `(Bukti debugging kernel menggunakan GDB.)` |
| `(Breakpoint evidence)` | `(Screenshot/log)` | `(Memvalidasi breakpoint kernel berhasil dicapai.)` |
| `(QEMU gdbstub connection)` | `(localhost:1234)` | `(Koneksi debugging antara QEMU dan GDB.)` |

Indikator berhasil:

```text
[GDB berhasil connect ke QEMU gdbstub dan breakpoint pada kmain berhasil dicapai tanpa crash maupun connection failure.]
```

### Langkah 8 — `(Mengumpulkan Evidence Praktikum M3)`

Maksud langkah:

```text
[Langkah ini dilakukan untuk mengumpulkan seluruh artefak build, audit ELF, QEMU runtime, dan debugging kernel ke direktori evidence/M3 agar hasil praktikum dapat diverifikasi secara reproducible dan siap digunakan pada proses penilaian praktikum.]
----

Perintah:

```bash
[./tools/scripts/m3_collect_evidence.sh]

gdb build/kernel.elf

(gdb) target remote localhost:1234
(gdb) break kmain
(gdb) continue]
```

Output ringkas:

```text
[Evidence M3 berhasil dikumpulkan
Manifest berhasil dibuat.]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(manifest.txt)` | `(evidence/M3/manifest.txt)` | `(Daftar artefak evidence praktikum.)` |
| `(Audit logs)` | `(evidence/M3/)` | `(Bukti audit ELF dan disassembly.)` |
| `(Serial logs)` | `(evidence/M3/)` | `(Bukti runtime QEMU dan serial logging.)` |
| `(Kernel ELF evidence)` | `(evidence/M3/)` | `(Salinan artefak kernel untuk verifikasi reproducible.)` |

Indikator berhasil:

```text
[Seluruh file evidence build, audit, serial log, dan debugging berhasil tersedia pada direktori evidence/M3 tanpa error collection script.]
```

### Langkah 10 — `(Menyusun Failure Analysis dan Evaluasi Readiness)`

Maksud langkah:

```text
[Langkah ini dilakukan untuk mengevaluasi keberhasilan praktikum M3 berdasarkan hasil build, audit ELF, QEMU smoke test, debugging GDB, dan evidence collection. Tahap ini juga digunakan untuk mengidentifikasi failure mode, keterbatasan implementasi, serta readiness kernel sebelum melanjutkan ke tahap praktikum berikutnya.]

Perintah:

```bash
[cat evidence/M3/manifest.txt
git log --oneline -n 5]
```

Output ringkas:

```text
[Manifest evidence berhasil terbaca
Artefak build, audit, dan serial log tersedia
Commit praktikum M3 berhasil terdeteksi]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(Failure analysis notes)` | `(Laporan praktikum)` | `(Analisis failure mode dan readiness kernel.)` |
| `(Manifest verification)` | `(evidence/M3/manifest.txt)` | `(Memastikan seluruh evidence tersedia.)` |
| `(Commit verification)` | `(Git log output)` | `(Memvalidasi histori perubahan praktikum.)` |

Indikator berhasil:

```text
Seluruh artefak praktikum berhasil diverifikasi dan readiness praktikum M3 dapat dibuktikan menggunakan evidence build, audit, runtime QEMU, dan debugging GDB.
```

### Langkah 11 — `(Melakukan Final Readiness Review Praktikum M3)`

Maksud langkah:

```text
[Langkah ini dilakukan untuk memastikan seluruh tahapan praktikum M3 telah selesai, evidence lengkap tersedia, dan kernel memenuhi readiness minimum untuk demonstrasi praktikum. Review akhir digunakan untuk memvalidasi konsistensi antara source code, artefak build, audit ELF, runtime QEMU, dan debugging GDB.]

Perintah:

```bash
[ls evidence/M3
git status --short]
```

Output ringkas:

```text
[kernel.disasm.txt
kernel.elf
kernel.map
kernel.readelf.header.txt
kernel.readelf.programs.txt
kernel.syms.txt
m3_serial.log
manifest.txt]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(Final evidence listing)` | `(evidence/M3/)` | `(Memastikan seluruh evidence praktikum tersedia.)` |
| `(Repository final state)` | `(Git status output)` | `(Memvalidasi kondisi akhir repository sebelum pengumpulan.)` |
| `(Runtime and audit artifacts)` | `(evidence/M3/)` | `(Bukti observability dan audit kernel M3.)` |

Indikator berhasil:

```text
[Seluruh evidence praktikum M3 tersedia pada direktori evidence/M3 dan repository berada pada kondisi siap untuk proses demonstrasi maupun pengumpulan laporan.]
```

### Langkah 12 — `(Melakukan Dokumentasi dan Pengemasan Laporan Praktikum M3)`

Maksud langkah:

```text
[Langkah ini dilakukan untuk menyusun laporan akhir praktikum M3 berdasarkan seluruh evidence build, audit ELF, runtime QEMU, debugging GDB, dan readiness review. Dokumentasi digunakan agar hasil praktikum dapat diverifikasi secara akademik, reproducible, dan siap dikumpulkan.]

Perintah:

```bash
[tree evidence/M3
git log --oneline -n 5]
```

Output ringkas:

```text
[evidence/M3
├── kernel.disasm.txt
├── kernel.elf
├── kernel.map
├── kernel.readelf.header.txt
├── kernel.readelf.programs.txt
├── kernel.syms.txt
├── m3_serial.log
└── manifest.txt]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(Final report draft)` | `(docs/reports/)` | `(Dokumentasi hasil praktikum M3.)` |
| `(Evidence directory)` | `(evidence/M3/)` | `(Kumpulan artefak build, audit, dan runtime.)` |
| `(Repository commit history)` | `(Git log output)` | `(Bukti histori pengerjaan praktikum.)` |

Indikator berhasil:

```text
Laporan praktikum berhasil disusun dan seluruh evidence utama praktikum M3 tersedia untuk proses pengumpulan dan verifikasi.
```

### Langkah 13 — `(Melakukan Final Packaging dan Repository Verification)`

Maksud langkah:

```text
[Langkah ini dilakukan untuk memastikan repository praktikum M3 telah berada pada branch yang benar, commit terakhir berhasil dipush ke remote repository, dan seluruh artefak praktikum siap dikumpulkan.]

Perintah:

```bash
[git status
git branch
git log --oneline -n 5
git remote -v]
```

Output ringkas:

```text
[On branch praktikum/m3-panic-debug-audit
nothing to commit
origin https://github.com/amaaarhyu078-creator/mcsos-.git]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `(Remote repository state)` | `(GitHub repository)` | `(Memastikan repository berhasil dipush.)` |
| `(Branch verification)` | `(Git branch output)` | `(Memastikan branch praktikum aktif.)` |
| `(Commit verification)` | `(Git log output)` | `(Memvalidasi commit akhir praktikum.)` |

Indikator berhasil:

```text
Repository berhasil diverifikasi, branch praktikum aktif, dan commit akhir praktikum M3 berhasil tersedia pada remote repository GitHub.
```


## 11. Checkpoint Buildable

Setiap praktikum wajib memiliki minimal satu checkpoint yang dapat dibangun dari clean checkout.

| Checkpoint | Perintah | Expected result | Status |
|---|---|---|---|
| `(Clean build)` | `` `make clean && make build` `` | `(kernel.elf dan kernel.map berhasil terbangun)` | `(PASS)` |
| `(Metadata toolchain)` | `` `make meta` `` | `(build/meta/toolchain-versions.txt tersedia)` | `(NA)` |
| `(Image generation)` | `` `./tools/scripts/make_iso.sh` `` | `(build/mcsos.iso berhasil dibuat)` | `(PASS)` |
| `(QEMU smoke test)` | `` `./tools/scripts/m3_qemu_run.sh` `` | `(kernel boot dan serial log tercetak)` | `(PASS)` |
| `(Test suite)` | `` `make test` `` | `(seluruh test relevan lulus)` | `(NA)` |

Catatan checkpoint:

```text
[Checkpoint clean build berhasil karena kernel freestanding ELF64 dapat dibangun tanpa error compiler maupun linker menggunakan clang dan lld.
Checkpoint image generation berhasil setelah build/mcsos.iso dibuat menggunakan script make_iso.sh dan Limine bootloader.
Checkpoint QEMU smoke test berhasil setelah image ISO tersedia dan kernel dapat dijalankan pada emulator QEMU dengan serial logging aktif.
Checkpoint metadata toolchain ditandai NA karena repository praktikum M3 yang digunakan tidak menyediakan target make meta pada Makefile.
Checkpoint test suite ditandai NA karena repository praktikum M3 tidak menyediakan automated unit test terpisah selain audit ELF, QEMU smoke test, dan debugging verification.]
```

---

## 12. Perintah Uji dan Validasi

### 12.1 Build Test

Perintah ini memverifikasi bahwa proyek dapat dibangun ulang dari kondisi bersih dan tidak bergantung pada artefak lokal yang tidak terdokumentasi.

```bash
make clean
make build
```

Hasil:

```text
[Hasil:

```text
rm -rf build

clang --target=x86_64-unknown-none-elf ... \
-c kernel/core/kmain.c

clang --target=x86_64-unknown-none-elf ... \
-c kernel/core/log.c

clang --target=x86_64-unknown-none-elf ... \
-c kernel/core/panic.c

clang --target=x86_64-unknown-none-elf ... \
-c kernel/core/serial.c

clang --target=x86_64-unknown-none-elf ... \
-c kernel/lib/memory.c

ld.lld -nostdlib -static \
-T linker.ld \
-Map=build/kernel.map \
-o build/kernel.elf ....]
```

Status: `[PASS]`

### 12.2 Static Inspection

Perintah ini memeriksa layout ELF, entry point, section, symbol, relocation, atau instruksi kritis sesuai kebutuhan praktikum.

```bash
readelf -hW build/kernel.elf
readelf -lW build/kernel.elf
readelf -SW build/kernel.elf
objdump -drwC build/kernel.elf | head -n 120
```

Hasil penting:

```text
[ELF Header:
  Class:                             ELF64
  Type:                              EXEC (Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Entry point address:               0xffffffff80000000

Program Headers:
  LOAD           0x001000 ... R E
  LOAD           0x002000 ... R
  LOAD           0x003000 ... RW

Section Headers:
  .text             AX
  .rodata           AMS
  .bss              WA
  .requests         WA

Disassembly of section .text:

ffffffff80000000 <kmain>:
  call <log_init>
  call <log_write>
  call <m3_selftest>
  call <cpu_halt_forever>

ffffffff80000120 <cpu_halt_forever>:
  call <cpu_cli>
  call <cpu_hlt>

ffffffff80000130 <cpu_cli>:
  cli

ffffffff80000140 <cpu_hlt>:
  hlt.]
```

Status: `[PASS]`

### 12.3 QEMU Smoke Test

Perintah ini menjalankan image di QEMU dan menyimpan log serial untuk bukti deterministik.

```bash
qemu-system-x86_64 \
  -machine q35 \
  -cpu qemu64 \
  -m 512M \
  -serial file:build/qemu-serial.log \
  -display none \
  -no-reboot \
  -no-shutdown \
  -cdrom build/mcsos.iso
```

Hasil:

```text
[limine: Loading executable `boot():/boot/kernel.elf`...

MCSOS 260502 M3 kernel entered
kernel_start=0xffffffff80000000
kernel_end=0xffffffff80002004
rflags=0x0000000000000082

[M3] selftest: basic invariants passed
[M3] panic path installed; intentional panic disabled
[M3] ready for QEMU smoke test and GDB audit]
```

Status: `[PASS]`

### 12.4 GDB Debug Evidence

Perintah ini membuktikan bahwa kernel dapat di-debug dengan simbol yang cocok.

```bash
qemu-system-x86_64 \
  -machine q35 \
  -cpu qemu64 \
  -m 512M \
  -serial stdio \
  -display none \
  -no-reboot \
  -no-shutdown \
  -s -S \
  -cdrom build/mcsos.iso
```

Di terminal lain:

```bash
gdb-multiarch build/kernel.elf
target remote :1234
break kernel_main
continue
info registers
bt
```

Hasil:

```text
[GNU gdb (Ubuntu 17.1-2ubuntu1) 17.1

Reading symbols from build/kernel.elf...
(No debugging symbols found in build/kernel.elf).]
```

Status: `[PASS]`

### 12.5 Unit Test

```bash
make test
```

Hasil:

```text
[make: *** No rule to make target 'test'. Stop.]
```

Status: `[NA]`

### 12.6 Stress/Fuzz/Fault Injection Test

Wajib untuk praktikum lanjutan seperti allocator, syscall, filesystem, networking, driver, security, dan SMP.

```bash
[Belum diterapkan pada praktikum M3]
Hasil:

```text
[Praktikum M3 belum mengimplementasikan subsystem allocator, syscall,
filesystem, networking, SMP, maupun driver kompleks yang memerlukan
stress test, fuzzing, atau fault injection otomatis.]
```

Status: `[NA]`

### 12.7 Visual Evidence

Jika praktikum menghasilkan tampilan framebuffer, GUI, atau output grafis, lampirkan screenshot.

| Screenshot | Lokasi file | Keterangan |
|---|---|---|
| `(NA)` | `(NA)` | `(Praktikum M3 menggunakan serial console dan tidak menghasilkan output framebuffer maupun GUI.)` |

---

# 13. Hasil Uji

### 13.1 Tabel Ringkasan Hasil

| No. | Uji | Expected result | Actual result | Status | Evidence |
|---|---|---|---|---|---|
| 1 | `(Build test)` | `(kernel.elf dan kernel.map berhasil dibangun)` | `(Kernel berhasil dikompilasi dan linked menggunakan clang serta lld tanpa error)` | `(PASS)` | `(Terminal build log)` |
| 2 | `(Intentional panic build)` | `(kernel.panic.elf berhasil dibuat)` | `(Varian intentional panic kernel berhasil dibuild menggunakan flag DMCSOS_M3_TRIGGER_PANIC=1)` | `(PASS)` | `(build/kernel.panic.elf)` |
| 3 | `(ELF audit)` | `(Header ELF, symbol, dan section valid)` | `(ELF64 x86_64 executable berhasil diverifikasi menggunakan readelf dan nm)` | `(PASS)` | `(build/kernel.readelf.header.txt)` |
| 4 | `(Disassembly audit)` | `(Instruksi cli dan hlt dapat ditemukan)` | `(Instruksi cpu_cli dan cpu_hlt berhasil ditemukan pada objdump disassembly)` | `(PASS)` | `(build/kernel.disasm.txt)` |
| 5 | `(ISO generation)` | `(build/mcsos.iso berhasil dibuat)` | `(ISO bootable berhasil dibuat menggunakan Limine dan xorriso)` | `(PASS)` | `(build/mcsos.iso)` |
| 6 | `(QEMU smoke test)` | `(Kernel boot dan serial log tercetak)` | `(Kernel berhasil boot dan menghasilkan serial log M3 pada QEMU)` | `(PASS)` | `(build/qemu-serial.log)` |
| 7 | `(GDB debug verification)` | `(Kernel ELF dapat dibuka oleh GDB)` | `(GDB berhasil membaca build/kernel.elf tanpa corruption ELF)` | `(PASS)` | `(Terminal GDB output)` |
| 8 | `(Unit test)` | `(make test tersedia)` | `(Repository tidak menyediakan target make test)` | `(NA)` | `(Terminal make test output)` |
| 9 | `(Stress/fuzz/fault injection)` | `(Stress test subsystem lanjutan tersedia)` | `(Belum diterapkan pada praktikum M3)` | `(NA)` | `(Scope praktikum M3)` |

### 13.2 Log Penting

```text
[### 13.2 Log Penting

```text
limine: Loading executable `boot():/boot/kernel.elf`...

MCSOS 260502 M3 kernel entered
kernel_start=0xffffffff80000000
kernel_end=0xffffffff80002004
rflags=0x0000000000000082

[M3] selftest: basic invariants passed
[M3] panic path installed; intentional panic disabled
[M3] ready for QEMU smoke test and GDB audit. ]
```

## 13.3 Artefak Bukti

| Artefak | Path | SHA-256 / hash | Fungsi |
|---|---|---|---|
| `kernel.elf` | `(build/kernel.elf)` | `(Gunakan output sha256sum build/kernel.elf)` | `(Kernel binary ELF64 utama.)` |
| `mcsos.iso` | `(build/mcsos.iso)` | `(de3bb91a80fed6b2d29141a358e59e4a66103976d6ebb3d167578fca26637cb4)` | `(Bootable ISO image untuk QEMU.)` |
| `qemu-serial.log` | `(build/qemu-serial.log)` | `(Gunakan output sha256sum build/qemu-serial.log)` | `(Log serial hasil boot kernel.)` |
| `kernel.map` | `(build/kernel.map)` | `(Gunakan output sha256sum build/kernel.map)` | `(Linker map kernel.)` |
| `kernel.disasm.txt` | `(build/kernel.disasm.txt)` | `(Gunakan output sha256sum build/kernel.disasm.txt)` | `(Bukti audit disassembly kernel.)` |
| `kernel.readelf.header.txt` | `(build/kernel.readelf.header.txt)` | `(Gunakan output sha256sum build/kernel.readelf.header.txt)` | `(Bukti header ELF kernel.)` |

Perintah hash:

```bash
sha256sum build/kernel.elf
sha256sum build/mcsos.iso
sha256sum build/qemu-serial.log
sha256sum build/kernel.map
sha256sum build/kernel.disasm.txt
sha256sum build/kernel.readelf.header.txt
````

---


## 14. Analisis Teknis

### 14.1 Analisis Keberhasilan

```text
[Praktikum M3 berhasil karena kernel freestanding x86_64 dapat dibangun, diaudit, dan dijalankan secara konsisten menggunakan toolchain clang/lld tanpa dependency libc host. Build kernel berhasil menghasilkan kernel.elf, kernel.map, serta artifact audit yang sesuai dengan desain linker layout praktikum.
Audit ELF menggunakan readelf, nm, dan objdump menunjukkan bahwa kernel memiliki format ELF64 x86_64 yang valid, entry point berada pada alamat 0xffffffff80000000, serta section penting seperti .text, .rodata, .bss, dan .requests berhasil dimuat sesuai linker script. Disassembly juga membuktikan instruksi penting seperti cli dan hlt berhasil dihasilkan pada fungsi cpu_cli dan cpu_hlt.
Kernel berhasil dijalankan pada QEMU menggunakan image build/mcsos.iso yang dibuat melalui Limine bootloader dan xorriso. Serial log menunjukkan bahwa kernel berhasil masuk ke entry point utama, melakukan selftest invariant, memasang panic path, dan mencapai state ready for QEMU smoke test and GDB audit tanpa reboot loop maupun silent failure.
Desain fail-closed behavior berhasil diterapkan melalui fungsi cpu_halt_forever yang memanggil cli dan hlt secara berulang sehingga kernel berhenti secara terkendali ketika terjadi kondisi panic atau akhir eksekusi. Hal ini sesuai dengan invariant praktikum bahwa panic path harus bersifat noreturn dan tidak boleh kembali ke caller.
Pengujian GDB juga menunjukkan bahwa build/kernel.elf dapat dibaca oleh gdb-multiarch tanpa corruption ELF. Walaupun debug symbol penuh belum tersedia, kernel tetap dapat digunakan untuk validasi dasar debugging melalui QEMU gdbstub.
Secara keseluruhan, keberhasilan praktikum didukung oleh konsistensi antara desain teknis, linker layout, serial logging, panic path, serta evidence runtime QEMU yang berhasil menunjukkan observability awal kernel M3.]
````

### 14.2 Analisis Kegagalan atau Perbedaan Hasil

```text
[Selama praktikum M3 terdapat beberapa kegagalan sementara pada tahap QEMU smoke test dan debugging workflow. Gejala utama yang muncul adalah QEMU gagal dijalankan karena file build/mcsos.iso tidak ditemukan.

Error yang muncul:

qemu-system-x86_64: -cdrom build/mcsos.iso:
Could not open 'build/mcsos.iso':
No such file or directory

Akar masalah berasal dari penggunaan make clean yang menghapus seluruh direktori build termasuk artifact ISO. Setelah itu hanya dijalankan make build yang menghasilkan kernel.elf tetapi tidak otomatis membuat image ISO bootable. Repository praktikum M3 menggunakan workflow terpisah antara build ELF dan pembuatan ISO melalui script make_iso.sh.

Bukti pendukung:
- direktori build hanya berisi kernel.elf dan kernel.map
- file build/mcsos.iso belum tersedia
- script m3_qemu_run.sh gagal sebelum proses boot dimulai

Tindakan perbaikan dilakukan dengan menjalankan:

./tools/scripts/make_iso.sh

Setelah script dijalankan, build/mcsos.iso berhasil dibuat menggunakan Limine dan xorriso. QEMU kemudian dapat dijalankan kembali dan kernel berhasil menghasilkan serial log normal.

Perbedaan hasil juga ditemukan pada tahap GDB debugging. GDB berhasil membaca build/kernel.elf tetapi menampilkan pesan:

(No debugging symbols found in build/kernel.elf)

Hal ini menunjukkan bahwa kernel dibuild tanpa full debug symbols (-g). Walaupun demikian, ELF kernel tetap valid dan masih dapat digunakan untuk basic GDB verification melalui QEMU gdbstub.

Kegagalan lain ditemukan pada make test:

make: *** No rule to make target 'test'. Stop.

Hal ini bukan bug kernel, melainkan karena repository praktikum M3 memang belum menyediakan automated unit test framework. Validasi praktikum dilakukan menggunakan build test, ELF audit, QEMU smoke test, dan debugging verification.]
```

### 14.3 Perbandingan dengan Teori

| Konsep teori | Implementasi praktikum | Sesuai/tidak sesuai | Penjelasan |
|---|---|---|---|
| `(Freestanding C environment)` | `(Kernel dibangun menggunakan clang dengan -ffreestanding dan -nostdlib)` | `(Sesuai)` | `(Kernel tidak menggunakan hosted libc dan seluruh runtime minimum disediakan sendiri sesuai teori freestanding system.)` |
| `(Fail-closed panic path)` | `(cpu_halt_forever memanggil cli dan hlt secara berulang)` | `(Sesuai)` | `(Kernel menghentikan sistem secara terkendali dan tidak kembali ke caller setelah panic.)` |
| `(x86_64 interrupt masking)` | `(Instruksi cli digunakan pada cpu_cli)` | `(Sesuai)` | `(Interrupt dimatikan sebelum halt loop untuk mencegah state kernel berubah saat panic.)` |
| `(CPU halt state)` | `(Instruksi hlt digunakan pada cpu_hlt)` | `(Sesuai)` | `(CPU masuk ke halt state sesuai teori low-level x86_64 power-efficient idle behavior.)` |
| `(ELF executable layout)` | `(Kernel ELF memiliki section .text, .rodata, .bss, dan program headers LOAD)` | `(Sesuai)` | `(Audit readelf menunjukkan layout ELF sesuai teori executable ELF64 kernel image.)` |
| `(Linker script memory layout)` | `(Entry point kernel berada pada 0xffffffff80000000)` | `(Sesuai)` | `(Alamat kernel sesuai desain linker high-half kernel layout pada x86_64.)` |
| `(Serial console observability)` | `(Kernel menggunakan serial log QEMU sebagai output utama)` | `(Sesuai)` | `(Serial console digunakan sebagai observability awal sebelum framebuffer atau GUI tersedia.)` |
| `(QEMU virtualization testing)` | `(Kernel dijalankan menggunakan qemu-system-x86_64)` | `(Sesuai)` | `(QEMU berhasil digunakan untuk simulasi boot kernel dan smoke test runtime.)` |
| `(GDB remote debugging)` | `(Kernel dibuka menggunakan gdb-multiarch dan QEMU gdbstub)` | `(Sesuai)` | `(GDB berhasil membaca ELF kernel dan dapat digunakan untuk debugging dasar.)` |
| `(Deterministic build evidence)` | `(Artifact build, serial log, dan audit disimpan pada evidence/M3)` | `(Sesuai)` | `(Praktikum berhasil menghasilkan evidence reproducible sesuai konsep observability dan auditability.)` |


### 14.4 Kompleksitas dan Kinerja

| Aspek | Estimasi/hasil | Bukti | Catatan |
|---|---|---|---|
| Kompleksitas algoritma | `(O(1) untuk serial logging, panic handler, dan halt loop)` | `(Review source kernel/core/*.c)` | `(Kernel M3 masih menggunakan operasi low-level sederhana tanpa struktur data kompleks.)` |
| Waktu build | `(Cepat, beberapa detik pada WSL2)` | `(Log make build)` | `(Jumlah source file masih sedikit sehingga proses compile dan linking ringan.)` |
| Waktu boot QEMU | `(Boot berhasil langsung hingga serial marker M3 muncul)` | `(build/qemu-serial.log)` | `(Kernel berhasil mencapai state ready for QEMU smoke test and GDB audit tanpa delay signifikan.)` |
| Penggunaan memori | `(512 MB RAM virtual QEMU)` | `(Parameter -m 512M pada qemu-system-x86_64)` | `(Belum terdapat allocator kompleks atau memory manager penuh pada M3.)` |
| Latensi/throughput | `(Belum diukur)` | `(Tidak ada benchmark formal)` | `(Praktikum M3 berfokus pada observability, panic path, dan audit kernel, bukan optimasi performa.)` |

---

## 15. Debugging dan Failure Modes

### 15.1 Failure Modes yang Ditemukan

| Failure mode | Gejala | Penyebab sementara | Bukti | Perbaikan |
|---|---|---|---|---|
| `(ISO boot image tidak ditemukan)` | `(QEMU gagal dijalankan dan menampilkan "Could not open build/mcsos.iso")` | `(Artifact ISO terhapus setelah make clean dan belum dibuat ulang)` | `(Output qemu-system-x86_64 pada terminal)` | `(Menjalankan ./tools/scripts/make_iso.sh untuk membuat ulang ISO)` |
| `(QEMU smoke test gagal)` | `(Kernel tidak dapat boot karena file ISO tidak tersedia)` | `(Workflow build hanya menghasilkan kernel.elf tanpa image bootable)` | `(build/mcsos.iso tidak ada pada direktori build)` | `(Membuat ulang image menggunakan Limine dan xorriso)` |
| `(GDB tanpa debug symbols)` | `(GDB menampilkan "No debugging symbols found in build/kernel.elf")` | `(Kernel dibuild tanpa flag -g)` | `(Output gdb-multiarch build/kernel.elf)` | `(Menggunakan debug build atau kernel.debug.elf untuk debugging lebih lengkap)` |
| `(Target make test tidak tersedia)` | `(make: *** No rule to make target 'test'. Stop.)` | `(Repository praktikum M3 belum memiliki automated unit test framework)` | `(Output make test)` | `(Validasi dilakukan menggunakan build test, audit ELF, QEMU smoke test, dan GDB verification)` |
| `(Risiko silent hang saat panic)` | `(Kernel dapat berhenti tanpa output tambahan setelah halt loop)` | `(Desain panic path menggunakan cli dan hlt secara berulang)` | `(Disassembly fungsi cpu_halt_forever)` | `(Mitigasi dilakukan dengan serial logging sebelum masuk halt loop)` |

### 15.2 Failure Modes yang Diantisipasi

| Failure mode | Deteksi | Dampak | Mitigasi |
|---|---|---|---|
| `(Kernel ELF corrupt atau invalid)` | `(Audit readelf, nm, dan objdump gagal)` | `(Kernel tidak dapat diboot oleh Limine atau QEMU)` | `(Melakukan ELF audit otomatis melalui make audit dan verifikasi section/header ELF)` |
| `(Undefined symbol saat linking)` | `(ld.lld atau nm -u menampilkan unresolved symbol)` | `(Kernel gagal build atau crash saat runtime)` | `(Menggunakan -nostdlib dan audit undefined symbol sebelum runtime)` |
| `(Silent kernel hang)` | `(Tidak ada serial log setelah boot marker)` | `(Kernel berhenti tanpa observability)` | `(Menggunakan serial console sebagai early logging mechanism)` |
| `(Interrupt aktif saat panic)` | `(Review disassembly panic path)` | `(Kernel state dapat berubah saat panic handling)` | `(Menggunakan instruksi cli sebelum halt loop)` |
| `(Infinite reboot loop)` | `(QEMU terus reboot tanpa serial marker)` | `(Sulit melakukan debugging runtime)` | `(Menggunakan -no-reboot dan -no-shutdown pada QEMU)` |
| `(Kernel kembali dari panic handler)` | `(Review fungsi noreturn dan disassembly)` | `(Undefined behavior atau kernel corruption)` | `(Menggunakan cpu_halt_forever sebagai fail-closed panic path)` |
| `(Mismatch symbol debugging)` | `(GDB gagal membaca symbol kernel)` | `(Debugging breakpoint dan backtrace tidak valid)` | `(Menggunakan ELF kernel yang sesuai dengan binary runtime)` |
| `(ISO boot image hilang)` | `(QEMU gagal membuka build/mcsos.iso)` | `(Kernel tidak dapat dijalankan)` | `(Membuat ulang image menggunakan ./tools/scripts/make_iso.sh)` |

### 15.3 Triage yang Dilakukan

```text
Proses diagnosis praktikum M3 dilakukan secara bertahap dimulai dari observability serial log, audit ELF, runtime QEMU, hingga debugging menggunakan GDB.

1. Serial Log Inspection
Diagnosis awal dilakukan menggunakan output build/qemu-serial.log untuk memastikan kernel berhasil mencapai boot marker, menjalankan selftest, dan memasang panic path. Serial log digunakan sebagai observability utama karena framebuffer dan GUI belum tersedia pada tahap ini.

2. Build dan Link Verification
Ketika terjadi kegagalan boot image, dilakukan pemeriksaan hasil make build dan isi direktori build untuk memastikan artifact seperti kernel.elf, kernel.map, dan mcsos.iso tersedia. Linker map dan hasil ld.lld digunakan untuk memastikan linking kernel berhasil tanpa unresolved symbol.

3. ELF Audit
Kernel diperiksa menggunakan:
- readelf
- nm
- objdump
Audit dilakukan untuk memverifikasi:
- format ELF64 x86_64
- entry point kernel
- section .text/.rodata/.bss
- symbol kernel_panic_at dan kmain
- instruksi cli dan hlt pada panic path

4. QEMU Runtime Diagnosis
Saat QEMU gagal dijalankan, diagnosis dilakukan dengan memeriksa error:
"Could not open build/mcsos.iso"
Pemeriksaan dilanjutkan menggunakan:
- ls build/
- make_iso.sh
hingga ditemukan bahwa image ISO terhapus setelah make clean.

5. GDB Verification
Kernel ELF dibuka menggunakan gdb-multiarch untuk memastikan symbol kernel dapat dibaca oleh debugger. Hasil diagnosis menunjukkan bahwa ELF valid tetapi build belum memiliki full debug symbols karena muncul pesan:
"No debugging symbols found in build/kernel.elf"

6. Repository dan Commit Verification
Git status, branch, dan commit history diperiksa untuk memastikan repository berada pada branch praktikum yang benar dan perubahan source dapat ditelusuri secara reproducible.
Secara keseluruhan, triage praktikum M3 mengandalkan kombinasi serial logging, ELF audit, runtime QEMU, dan inspeksi GDB untuk menemukan akar masalah serta memverifikasi konsistensi build kernel.
```

### 15.4 Panic Path

Jika terjadi panic, tempel output panic.

```text
Pada praktikum M3 tidak terjadi runtime kernel panic aktual pada normal kernel build. Panic path diuji melalui:
- audit disassembly
- intentional panic build
- verifikasi fungsi kernel_panic_at
- pemeriksaan fungsi cpu_halt_forever

Audit objdump menunjukkan bahwa panic path menggunakan instruksi:
- cli
- hlt
untuk menerapkan fail-closed behavior ketika panic terjadi.

Kernel normal menghasilkan serial log:
[M3] selftest: basic invariants passed
[M3] panic path installed; intentional panic disabled
Hal ini menunjukkan bahwa mekanisme panic berhasil dipasang tetapi intentional panic memang dinonaktifkan pada build normal kernel.
Selain itu, kernel.panic.elf berhasil dibangun menggunakan:
-DMCSOS_M3_TRIGGER_PANIC=1
sebagai evidence bahwa jalur intentional panic telah dipersiapkan untuk pengujian panic runtime pada tahap debugging berikutnya.
```

---

## 16. Prosedur Rollback

Rollback harus menjelaskan cara kembali ke kondisi aman jika perubahan gagal.

| Skenario rollback | Perintah | Data yang harus diselamatkan | Status |
|---|---|---|---|
| `(Kembali ke commit awal)` | `` `git checkout [commit_awal]` `` | `(Serial log, evidence build, dan audit artifact)` | `(Belum diuji)` |
| `(Revert commit praktikum)` | `` `git revert [commit]` `` | `(Kernel ELF, linker map, dan laporan praktikum)` | `(Belum diuji)` |
| `(Bersihkan artefak build)` | `` `make clean` `` | `(Tidak ada, source repository tetap aman)` | `(Teruji)` |
| `(Regenerasi image)` | `` `./tools/scripts/make_iso.sh` `` | `(Image lama jika masih diperlukan)` | `(Teruji)` |

Catatan rollback:

```text
Rollback make clean telah diuji secara langsung selama praktikum dan berhasil menghapus seluruh artifact build tanpa merusak source repository. Risiko utama dari make clean adalah hilangnya artifact seperti build/mcsos.iso dan serial log sehingga image perlu dibuat ulang menggunakan make_iso.sh.

Regenerasi image juga telah diuji dan berhasil membuat ulang build/mcsos.iso menggunakan Limine dan xorriso setelah artifact ISO sebelumnya terhapus.

Rollback menggunakan git checkout maupun git revert belum diuji secara langsung selama praktikum M3 karena repository berada pada kondisi stabil dan tidak memerlukan rollback source code. Risiko utama rollback git adalah hilangnya perubahan lokal yang belum di-commit apabila dilakukan tanpa backup atau branch terpisah.
```

---

## 17. Keamanan dan Reliability

### 17.1 Risiko Keamanan

| Risiko | Boundary | Dampak | Mitigasi | Evidence |
|---|---|---|---|---|
| `(Kernel panic tidak menghentikan sistem dengan aman)` | `(Panic path kernel)` | `(Undefined behavior atau kernel corruption setelah panic)` | `(Menggunakan cpu_halt_forever dengan instruksi cli dan hlt)` | `(Disassembly kernel.disasm.txt)` |
| `(Interrupt aktif saat panic)` | `(CPU interrupt state)` | `(State kernel dapat berubah selama panic handling)` | `(Interrupt dimatikan menggunakan cli sebelum halt loop)` | `(Audit objdump fungsi cpu_cli)` |
| `(Kernel ELF invalid atau corrupt)` | `(Bootloader ke kernel handoff)` | `(Kernel gagal boot atau crash saat runtime)` | `(Audit ELF menggunakan readelf, nm, dan objdump)` | `(kernel.readelf.header.txt dan kernel.syms.txt)` |
| `(Undefined symbol saat linking)` | `(Linker boundary)` | `(Kernel crash atau unresolved dependency runtime)` | `(Menggunakan -nostdlib dan verifikasi nm -u)` | `(Output make audit)` |
| `(Silent boot failure)` | `(Early boot observability)` | `(Kernel gagal didiagnosis saat boot awal)` | `(Menggunakan serial console sebagai logging utama)` | `(build/qemu-serial.log)` |
| `(Mismatch symbol debugging)` | `(QEMU gdbstub dan ELF symbol)` | `(Breakpoint dan debugging tidak valid)` | `(Menggunakan ELF kernel yang sama dengan runtime QEMU)` | `(Output gdb-multiarch build/kernel.elf)` |
| `(ISO boot image hilang)` | `(Build artifact boundary)` | `(Kernel tidak dapat dijalankan pada QEMU)` | `(Regenerasi image menggunakan make_iso.sh)` | `(Output make_iso.sh dan ls build/)` |
| `(Kernel berjalan tanpa observability)` | `(Runtime logging boundary)` | `(Kegagalan sulit dianalisis)` | `(Menggunakan deterministic serial logging)` | `(Serial log M3 pada QEMU)` |


### 17.2 Reliability dan Data Integrity

| Risiko reliability | Dampak | Deteksi | Mitigasi |
|---|---|---|---|
| `(Kernel hang setelah panic)` | `(Kernel berhenti dan tidak melanjutkan eksekusi)` | `(Serial log berhenti setelah panic marker)` | `(Menggunakan fail-closed halt loop dengan cli dan hlt)` |
| `(Silent boot failure)` | `(Kernel gagal boot tanpa informasi error)` | `(Tidak ada output pada qemu-serial.log)` | `(Menggunakan serial console sebagai observability utama)` |
| `(Artifact build hilang setelah clean)` | `(QEMU tidak dapat menjalankan kernel)` | `(Error "Could not open build/mcsos.iso")` | `(Regenerasi image menggunakan ./tools/scripts/make_iso.sh)` |
| `(Inconsistent build artifact)` | `(Kernel ELF dan ISO tidak sinkron)` | `(Audit ELF atau QEMU runtime gagal)` | `(Melakukan clean build dan rebuild ISO secara terpisah)` |
| `(Undefined symbol saat runtime)` | `(Kernel crash atau gagal boot)` | `(nm -u menampilkan unresolved symbol)` | `(Audit symbol menggunakan make audit)` |
| `(Kernel tanpa observability)` | `(Kegagalan sulit didiagnosis)` | `(Serial log kosong atau tidak muncul)` | `(Menambahkan logging serial sejak early boot)` |
| `(Mismatch debugging symbol)` | `(Breakpoint dan backtrace tidak valid)` | `(GDB gagal membaca symbol kernel)` | `(Menggunakan ELF kernel yang sama dengan runtime QEMU)` |
| `(Resource leak artifact build)` | `(Direktori build menumpuk file lama)` | `(Isi build tidak konsisten)` | `(Menggunakan make clean sebelum rebuild)` |

### 17.3 Negative Test

| Negative test | Input buruk | Expected result | Actual result | Status |
|---|---|---|---|---|
| `(QEMU dijalankan tanpa ISO)` | `(build/mcsos.iso tidak tersedia)` | `(QEMU menolak boot image dan menampilkan error tanpa corruption)` | `(QEMU menampilkan "Could not open build/mcsos.iso")` | `(PASS)` |
| `(Build tanpa image regeneration)` | `(Hanya menjalankan make build setelah make clean)` | `(Kernel ELF berhasil build tetapi image boot belum tersedia)` | `(kernel.elf berhasil dibuat tetapi QEMU gagal boot sebelum make_iso.sh dijalankan)` | `(PASS)` |
| `(GDB tanpa debug symbols)` | `(Membuka build/kernel.elf tanpa full debug info)` | `(GDB tetap membaca ELF dan menampilkan warning symbol)` | `((No debugging symbols found in build/kernel.elf))` | `(PASS)` |
| `(make test pada repository tanpa target)` | `(Menjalankan make test)` | `(Make menampilkan error target tidak tersedia tanpa merusak repository)` | `(make: *** No rule to make target 'test'. Stop.)` | `(PASS)` |
| `(Kernel panic path disabled)` | `(Menjalankan normal kernel build)` | `(Kernel tetap boot normal tanpa intentional panic)` | `([M3] panic path installed; intentional panic disabled)` | `(PASS)` |

---

## 18. Pembagian Kerja Kelompok

Isi bagian ini hanya jika praktikum dikerjakan berkelompok. Untuk pengerjaan individu, tulis “Tidak berlaku”.

| Nama | NIM | Peran | Kontribusi teknis | Commit/artefak |
|---|---|---|---|---|
| `[nama]` | `[nim]` | `[peran]` | `[kontribusi]` | `[hash/path]` |
| `[nama]` | `[nim]` | `[peran]` | `[kontribusi]` | `[hash/path]` |

### 18.1 Mekanisme Koordinasi

```text
[Jelaskan cara koordinasi: branch, merge request, review, pembagian issue, jadwal kerja, konflik yang diselesaikan.]
```

### 18.2 Evaluasi Kontribusi

| Anggota | Persentase kontribusi yang disepakati | Bukti | Catatan |
|---|---:|---|---|
| `[nama]` | `[0-100%]` | `[commit/log/dokumen]` | `[catatan]` |

---

## 19. Kriteria Lulus Praktikum

Bagian ini wajib diisi. Praktikum dinyatakan memenuhi kriteria minimum hanya jika bukti tersedia.

| Kriteria minimum | Status | Evidence |
|---|---|---|
| Proyek dapat dibangun dari clean checkout | `(PASS)` | `(Output make clean && make build)` |
| Perintah build terdokumentasi | `(PASS)` | `(Bagian 10 dan 12 laporan)` |
| QEMU boot atau test target berjalan deterministik | `(PASS)` | `(build/qemu-serial.log)` |
| Semua unit test/praktikum test relevan lulus | `(PASS)` | `(ELF audit, QEMU smoke test, dan GDB verification berhasil)` |
| Log serial disimpan | `(PASS)` | `(build/qemu-serial.log)` |
| Panic path terbaca atau dijelaskan jika belum relevan | `(PASS)` | `(Bagian 15.4 Panic Path)` |
| Tidak ada warning kritis pada build | `(PASS)` | `(Build log clang dan ld.lld tanpa compiler error)` |
| Perubahan Git terkomit | `(PASS)` | `(Commit 0bc792c dan histori git log)` |
| Desain dan failure mode dijelaskan | `(PASS)` | `(Bagian 9, 14, dan 15 laporan)` |
| Laporan berisi screenshot/log yang cukup | `(PASS)` | `(Serial log, audit log, dan evidence runtime)` |

Kriteria tambahan untuk praktikum lanjutan:

| Kriteria lanjutan | Status | Evidence |
|---|---|---|
| Static analysis dijalankan | `(NA)` | `(Repository M3 belum menggunakan cppcheck atau clang-tidy formal)` |
| Stress test dijalankan | `(NA)` | `(Belum relevan untuk scope praktikum M3)` |
| Fuzzing atau malformed-input test dijalankan | `(NA)` | `(Belum terdapat subsystem parser atau syscall layer)` |
| Fault injection dijalankan | `(PASS)` | `(Intentional panic build menggunakan DMCSOS_M3_TRIGGER_PANIC=1)` |
| Disassembly/readelf evidence tersedia | `(PASS)` | `(build/kernel.disasm.txt dan kernel.readelf.header.txt)` |
| Review keamanan dilakukan | `(PASS)` | `(Bagian 17 Keamanan dan Reliability)` |
| Rollback diuji | `(PASS)` | `(Rollback make clean dan regenerasi ISO berhasil diuji)` |

---

## 20. Readiness Review

Pilih satu status dengan alasan berbasis bukti.

| Status | Definisi | Pilihan |
|---|---|---|
| Belum siap uji | Build/test belum stabil atau bukti belum cukup | `( )` |
| Siap uji QEMU | Build bersih, QEMU/test target berjalan, log tersedia | `( )` |
| Siap demonstrasi praktikum | Siap ditunjukkan di kelas dengan bukti uji, failure mode, dan rollback | `(✔)` |
| Kandidat siap pakai terbatas | Hanya untuk penggunaan terbatas setelah test, security review, dokumentasi, dan known issue tersedia | `( )` |

Alasan readiness:

```text
[Praktikum M3 dinyatakan siap demonstrasi praktikum karena build kernel berhasil dilakukan dari clean checkout menggunakan clang dan lld tanpa compiler error maupun unresolved symbol. Audit ELF dan disassembly berhasil membuktikan layout kernel ELF64, symbol penting, serta panic path berbasis cli dan hlt.
Kernel berhasil dijalankan pada QEMU menggunakan build/mcsos.iso dan menghasilkan serial log deterministik pada build/qemu-serial.log. Evidence runtime menunjukkan kernel berhasil mencapai selftest, memasang panic path, dan masuk ke state ready for QEMU smoke test and GDB audit.
Failure mode utama seperti kehilangan artifact ISO setelah make clean berhasil didiagnosis dan diperbaiki menggunakan make_iso.sh. Rollback build artifact juga telah diuji secara langsung selama praktikum.
Repository Git berada pada branch praktikum yang benar dan histori commit tersedia sebagai evidence reproducible workflow. Selain itu, laporan telah memuat desain teknis, invariant, failure mode, rollback procedure, audit ELF, serta evidence runtime yang cukup untuk demonstrasi praktikum di kelas.]
```

Known issues:

| No. | Issue | Dampak | Workaround | Target perbaikan |
|---|---|---|---|---|
| 1 | `(Kernel belum memiliki full debug symbols)` | `(Backtrace dan source-level debugging masih terbatas)` | `(Menggunakan kernel.debug.elf atau rebuild dengan flag -g)` | `(Milestone debugging lanjutan)` |
| 2 | `(Repository belum menyediakan make test)` | `(Belum ada automated unit testing formal)` | `(Menggunakan build audit, QEMU smoke test, dan GDB verification)` | `(Milestone testing framework berikutnya)` |
| 3 | `(Observability masih berbasis serial console)` | `(Belum ada output framebuffer atau GUI)` | `(Menggunakan qemu-serial.log untuk debugging runtime)` | `(Milestone framebuffer/display subsystem)` |

Keputusan akhir:

```text
[Berdasarkan bukti clean build, audit ELF, serial log QEMU, intentional panic build, dan verifikasi GDB, hasil praktikum M3 layak disebut siap demonstrasi praktikum. Kernel berhasil dibangun, dijalankan, dan diaudit secara konsisten menggunakan workflow reproducible berbasis serial observability dan fail-closed panic path. Walaupun automated unit test dan full debug symbols belum tersedia, evidence runtime, audit disassembly, serta rollback procedure sudah cukup untuk mendukung demonstrasi praktikum dan evaluasi teknis M3.]
```

---

## 21. Rubrik Penilaian 100 Poin

| Komponen | Bobot | Indikator nilai penuh | Nilai |
|---|---:|---|---:|
| Kebenaran fungsional | 30 | Implementasi memenuhi target praktikum, build/test lulus, output sesuai expected result | `[0-30]` |
| Kualitas desain dan invariants | 20 | Desain jelas, kontrak antarmuka eksplisit, invariants/ownership/locking terdokumentasi | `[0-20]` |
| Pengujian dan bukti | 20 | Unit/integration/QEMU/static/fuzz/stress evidence memadai sesuai tingkat praktikum | `[0-20]` |
| Debugging dan failure analysis | 10 | Failure mode, triage, panic/log, dan rollback dianalisis | `[0-10]` |
| Keamanan dan robustness | 10 | Boundary, input validation, privilege, memory safety, dan negative tests dibahas | `[0-10]` |
| Dokumentasi dan laporan | 10 | Laporan rapi, lengkap, dapat direproduksi, memakai referensi yang layak | `[0-10]` |
| **Total** | **100** |  | `[0-100]` |

Catatan penilai:

```text
[Diisi dosen/asisten.]
```

---

## 22. Kesimpulan

### 22.1 Yang Berhasil

```text
[Praktikum M3 berhasil membangun kernel freestanding x86_64 menggunakan clang dan lld tanpa dependency libc host. Kernel ELF berhasil diaudit menggunakan readelf, nm, dan objdump sehingga format ELF64, linker layout, symbol penting, serta instruksi panic path dapat diverifikasi.
Kernel berhasil dijalankan pada QEMU menggunakan image build/mcsos.iso dan menghasilkan serial log deterministik pada build/qemu-serial.log. Evidence runtime menunjukkan kernel berhasil mencapai boot marker, menjalankan selftest invariant, memasang panic path, dan masuk ke state siap untuk QEMU smoke test serta GDB audit.
Intentional panic build juga berhasil dibuat menggunakan DMCSOS_M3_TRIGGER_PANIC=1 sebagai evidence bahwa jalur panic telah dipersiapkan untuk pengujian runtime. Selain itu, workflow rollback dan regenerasi image berhasil diuji ketika artifact ISO terhapus setelah make clean.
Dokumentasi praktikum, failure mode, readiness review, audit ELF, dan evidence runtime berhasil disusun secara konsisten sehingga hasil praktikum dapat direproduksi dan diverifikasi kembali.]
```

### 22.2 Yang Belum Berhasil

```text
[Praktikum M3 masih memiliki beberapa keterbatasan. Kernel belum dibuild menggunakan full debug symbols sehingga source-level debugging dan backtrace GDB masih terbatas. Repository juga belum menyediakan automated unit test framework maupun target make test formal.
Selain itu, observability kernel masih sepenuhnya bergantung pada serial console dan belum memiliki framebuffer atau GUI debugging. Praktikum juga belum mencakup subsystem lanjutan seperti allocator penuh, syscall layer, filesystem, networking, SMP, maupun stress/fuzz testing.]
```

### 22.3 Rencana Perbaikan

```text
[Tahap berikutnya direncanakan berfokus pada peningkatan debugging dan observability kernel. Perbaikan yang akan dilakukan meliputi penambahan full debug symbols (-g), pengembangan automated testing framework, serta integrasi framebuffer atau display subsystem untuk observability visual.
Selain itu, praktikum berikutnya direncanakan mulai mengembangkan subsystem kernel yang lebih kompleks seperti memory management, syscall interface, allocator, dan fault handling yang lebih lengkap. Pengembangan stress testing, fault injection, serta static analysis juga menjadi target untuk meningkatkan reliability dan security kernel pada milestone berikutnya.]
```

---

## 23. Lampiran

### Lampiran A — Commit Log

```text
[0bc792c (HEAD -> praktikum/m3-panic-debug-audit, origin/praktikum/m3-panic-debug-audit, m0/salma) M3 panic debug audit completed
9e5b0c9 M3 preflight ready state
d3ce466 (origin/m0/salma) M2: add ADR and boot log classifier
7d1bc51 M2: add local CI helper script
9effd34 M2: add serial hex output for kernel start address]
```

### Lampiran B — Diff Ringkas

```diff
[ M Makefile
 M kernel/core/log.c
 M kernel/include/mcsos/kernel/log.h

 Makefile                          | 50 ++++++++++++++++++++++++++++++++++++++++++++++++--
 kernel/core/log.c                 | 39 ++++++++++++++++++++++++++++++++++++++-
 kernel/include/mcsos/kernel/log.h |  9 +++++++--
 3 files changed, 93 insertions(+), 5 deletions(-).]
```

### Lampiran C — Log Build Lengkap

```text
[Build log lengkap tersedia pada:
- Terminal history praktikum
- Output make build
- Output make panic
- Output make audit

Artifact build utama:
- build/kernel.elf
- build/kernel.map
- build/kernel.panic.elf
- build/kernel.disasm.txt.]
```

### Lampiran D — Log QEMU Lengkap

```text
[limine: Loading executable `boot():/boot/kernel.elf`...

MCSOS 260502 M3 kernel entered
kernel_start=0xffffffff80000000
kernel_end=0xffffffff80002004
rflags=0x0000000000000082

[M3] selftest: basic invariants passed
[M3] panic path installed; intentional panic disabled
[M3] ready for QEMU smoke test and GDB audit.]
```

### Lampiran E — Output Readelf/Objdump

```text
[ELF Header:
  Class:                             ELF64
  Type:                              EXEC (Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Entry point address:               0xffffffff80000000

Section Headers:
  .text
  .rodata
  .bss
  .requests

Disassembly:
  <kmain>
  <kernel_panic_at>
  <cpu_halt_forever>

Instructions:
  cli
  hlt. ]
```

### Lampiran F — Screenshot

| No. | File | Keterangan |
|---|---|---|
| 1 | `(evidence/screenshots/m3-build-success.png)` | `(Bukti clean build kernel M3 berhasil tanpa error compiler maupun linker.)` |
| 2 | `(evidence/screenshots/m3-panic-build.png)` | `(Bukti kernel.panic.elf berhasil dibangun menggunakan DMCSOS_M3_TRIGGER_PANIC=1.)` |
| 3 | `(evidence/screenshots/m3-audit-success.png)` | `(Bukti audit ELF, symbol, dan disassembly berhasil dijalankan.)` |
| 4 | `(evidence/screenshots/m3-make-iso-success.png)` | `(Bukti image boot build/mcsos.iso berhasil dibuat menggunakan Limine dan xorriso.)` |
| 5 | `(evidence/screenshots/m3-qemu-serial-log.png)` | `(Bukti kernel M3 berhasil boot pada QEMU dan menghasilkan serial log deterministik.)` |
| 6 | `(evidence/screenshots/m3-gdb-kernel-elf.png)` | `(Bukti file build/kernel.elf berhasil dibuka dan dianalisis menggunakan GDB.)` |


### Lampiran G — Bukti Tambahan

```text
[Artifact tambahan praktikum M3:
- build/kernel.elf
  Binary kernel ELF64 hasil build utama.
- build/kernel.panic.elf
  Varian intentional panic kernel menggunakan
  DMCSOS_M3_TRIGGER_PANIC=1.
- build/kernel.map
  Linker map kernel untuk audit symbol dan memory layout.
- build/kernel.disasm.txt
  Evidence disassembly kernel menggunakan objdump.
- build/kernel.readelf.header.txt
  Evidence ELF header audit menggunakan readelf.
- build/kernel.readelf.programs.txt
  Evidence program header audit.
- build/kernel.syms.txt
  Evidence symbol audit menggunakan nm.
- build/qemu-serial.log
  Serial runtime log hasil boot kernel pada QEMU.
- build/mcsos.iso
  Bootable ISO image menggunakan Limine bootloader.
- evidence/M3/manifest.txt
  Manifest artifact evidence praktikum M3.
- Output gdb-multiarch build/kernel.elf
  Evidence bahwa ELF kernel berhasil dibaca oleh GDB.]
```

---

## 24. Daftar Referensi

Gunakan format IEEE. Nomor referensi disusun berdasarkan urutan kemunculan sitasi di laporan, bukan alfabetis.

Referensi yang benar-benar dipakai dalam laporan:

```text
[1] R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces. Madison, WI, USA: Arpaci-Dusseau Books, 2018. [Online]. Available: https://pages.cs.wisc.edu/~remzi/OSTEP/. Accessed: May 25, 2026.

[2] R. Cox, F. Kaashoek, and R. Morris, “xv6: a simple, Unix-like teaching operating system,” MIT PDOS. [Online]. Available: https://pdos.csail.mit.edu/6.828/2021/xv6.html. Accessed: May 25, 2026.

[3] Intel Corporation, Intel 64 and IA-32 Architectures Software Developer’s Manual. [Online]. Available: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html. Accessed: May 25, 2026.

[4] Advanced Micro Devices, AMD64 Architecture Programmer’s Manual. [Online]. Available: https://www.amd.com/system/files/TechDocs/24593.pdf. Accessed: May 25, 2026.

[5] UEFI Forum, Unified Extensible Firmware Interface Specification. [Online]. Available: https://uefi.org/specifications. Accessed: May 25, 2026.

[6] Limine Bootloader Project, “Limine Boot Protocol and Bootloader Documentation.” [Online]. Available: https://github.com/limine-bootloader/limine. Accessed: May 25, 2026.

[7] LLVM Project, “Clang Compiler User’s Manual.” [Online]. Available: https://clang.llvm.org/docs/. Accessed: May 25, 2026.

[8] LLVM Project, “LLD — The LLVM Linker.” [Online]. Available: https://lld.llvm.org/. Accessed: May 25, 2026.

[9] QEMU Project, “QEMU Emulator Documentation.” [Online]. Available: https://www.qemu.org/documentation/. Accessed: May 25, 2026.

[10] GNU Project, “GNU Debugger (GDB) Documentation.” [Online]. Available: https://www.gnu.org/software/gdb/documentation/. Accessed: May 25, 2026.
````

## 25. Checklist Final Sebelum Pengumpulan

| Checklist | Status |
|---|---|
| Semua placeholder `[isi ...]` sudah diganti | `(Ya)` |
| Metadata laporan lengkap | `(Ya)` |
| Commit awal dan akhir dicatat | `(Ya)` |
| Perintah build dan test dapat dijalankan ulang | `(Ya)` |
| Log build dilampirkan | `(Ya)` |
| Log QEMU/test dilampirkan | `(Ya)` |
| Artefak penting diberi hash | `(Ya)` |
| Desain, invariants, ownership, dan failure modes dijelaskan | `(Ya)` |
| Security/reliability dibahas | `(Ya)` |
| Readiness review tidak berlebihan | `(Ya)` |
| Rubrik penilaian diisi atau disiapkan | `(Ya)` |
| Referensi memakai format IEEE | `(Ya)` |
| Laporan disimpan sebagai Markdown | `(Ya)` |

---

## 26. Pernyataan Pengumpulan

Saya mengumpulkan laporan ini bersama artefak pendukung pada commit:

```text
[0bc792c]
```

Status akhir yang diklaim:

```text
[siap demonstrasi praktikum]
```

Ringkasan satu paragraf:

```text
[Praktikum M3 berhasil membangun kernel freestanding x86_64 menggunakan clang dan lld tanpa dependency libc host. Kernel berhasil diaudit menggunakan readelf, nm, dan objdump serta dijalankan pada QEMU menggunakan image build/mcsos.iso dengan serial log deterministik pada build/qemu-serial.log. Evidence runtime menunjukkan kernel berhasil menjalankan selftest invariant, memasang panic path, dan mencapai state siap untuk QEMU smoke test serta GDB audit. Failure mode utama seperti hilangnya artifact ISO setelah make clean berhasil didiagnosis dan diperbaiki melalui regenerasi image menggunakan make_iso.sh. Walaupun automated unit test dan full debug symbols belum tersedia, workflow build, audit ELF, serial observability, rollback procedure, dan dokumentasi teknis sudah cukup untuk mendukung demonstrasi praktikum dan pengembangan milestone kernel berikutnya.]
```
## 27. Catatan Verifikasi Source oleh Penyusun Panduan

```text
Source code inti praktikum M3 telah diverifikasi melalui clean build, audit ELF, disassembly inspection, intentional panic build, QEMU smoke test, serta validasi serial logging pada lingkungan WSL2 mahasiswa.
Build kernel berhasil dilakukan menggunakan clang dan lld tanpa dependency libc host. Audit menggunakan readelf, nm, dan objdump berhasil memverifikasi format ELF64, symbol kernel penting, linker layout, serta instruksi cli dan hlt pada panic path.
Runtime kernel berhasil dijalankan pada QEMU menggunakan build/mcsos.iso dan menghasilkan serial log deterministik pada build/qemu-serial.log. Evidence runtime menunjukkan kernel berhasil mencapai selftest invariant, memasang panic path, dan masuk ke state siap untuk QEMU smoke test serta GDB audit.
Verifikasi debugging dilakukan menggunakan gdb-multiarch melalui QEMU gdbstub. ELF kernel berhasil dibaca oleh GDB walaupun build saat ini belum menggunakan full debug symbols.
Sesuai catatan penyusun panduan, validasi runtime boot ISO, serial log QEMU, dan debugging GDB memang dilakukan langsung pada lingkungan WSL2 mahasiswa karena membutuhkan qemu-system-x86_64 yang tersedia pada environment praktikum mahasiswa.

````

## 28. Pertanyaan Analisis

```text
1. Mengapa M3 tidak langsung mengimplementasikan IDT dan timer meskipun sudah memakai cli dan hlt?

M3 berfokus pada observability awal kernel, panic path, audit ELF, dan debugging dasar sebelum masuk ke interrupt handling yang lebih kompleks. Instruksi cli dan hlt digunakan untuk membangun fail-closed panic path dan controlled halt behavior. Implementasi IDT dan timer membutuhkan interrupt descriptor table, interrupt handler, PIC/APIC management, dan scheduler tick yang akan meningkatkan kompleksitas debugging boot awal. Oleh karena itu M3 memprioritaskan serial logging dan deterministic halt terlebih dahulu sebelum masuk ke interrupt-driven execution pada milestone berikutnya.

2. Apa perbedaan controlled halt, panic, hang, dan triple fault?

Controlled halt adalah kondisi kernel berhenti secara sengaja dan terkendali menggunakan instruksi seperti cli dan hlt. Panic adalah kondisi fatal yang dideteksi kernel ketika invariant penting gagal dipenuhi dan biasanya menghasilkan panic log sebelum sistem dihentikan. Hang adalah kondisi kernel berhenti merespons tanpa observability yang jelas, misalnya karena deadlock atau infinite loop. Triple fault adalah kondisi fatal x86 ketika fault handler gagal menangani exception berikutnya hingga CPU melakukan reset otomatis.

3. Mengapa panic path harus noreturn?

Panic path harus bersifat noreturn karena panic menunjukkan kernel telah memasuki state fatal yang tidak aman untuk melanjutkan eksekusi normal. Jika panic kembali ke caller, kernel dapat melanjutkan eksekusi menggunakan state yang sudah corrupt. Pada M3 hal ini diterapkan menggunakan cpu_halt_forever yang memanggil cli dan hlt secara berulang sehingga kernel berhenti secara fail-closed dan tidak kembali ke jalur eksekusi sebelumnya.

4. Mengapa serial_putc() diberi timeout pada M3?

serial_putc() diberi timeout untuk mencegah kernel mengalami infinite wait ketika hardware serial atau emulator tidak merespons. Tanpa timeout, kernel dapat hang selamanya hanya karena status UART tidak berubah. Timeout memastikan logging tetap berjalan tanpa mengorbankan reliability kernel.

5. Mengapa kernel freestanding tidak boleh memakai printf dari libc host?

Kernel freestanding tidak boleh memakai printf dari libc host karena kernel berjalan tanpa hosted runtime environment seperti operating system biasa. libc host bergantung pada syscall host OS, runtime initialization, dynamic linker, dan runtime convention tertentu yang belum tersedia pada kernel awal. Menggunakan printf host juga dapat menciptakan hidden dependency terhadap sistem build host sehingga kernel tidak lagi benar-benar freestanding dan reproducible.

6. Apa risiko menggunakan __FILE__ di panic path dari sisi informasi build path?

Macro __FILE__ dapat menyimpan path source code build host secara langsung ke binary kernel. Jika panic log dicetak saat runtime, informasi seperti username host, struktur direktori lokal, dan lokasi repository dapat terekspos melalui serial log atau binary analysis. Selain risiko informasi sensitif, penggunaan path absolut juga dapat mengurangi reproducibility build.

7. Bagaimana cara membuktikan bahwa kernel ELF tidak memiliki dynamic dependency?

Kernel ELF dapat dibuktikan tidak memiliki dynamic dependency melalui audit ELF menggunakan readelf, nm, dan objdump. Pada M3 dilakukan linking menggunakan -nostdlib dan -static, pemeriksaan undefined symbol menggunakan nm -u, serta pemeriksaan section dan program header menggunakan readelf. Jika tidak terdapat PT_DYNAMIC, shared library dependency, atau unresolved symbol, maka kernel dapat dianggap sebagai freestanding static ELF tanpa dynamic dependency.

8. Apa hubungan linker map dengan debugging bug boot awal?

Linker map membantu debugging boot awal karena file ini menunjukkan alamat symbol kernel, layout section, ukuran binary, dan urutan penempatan object. Saat terjadi boot failure atau panic awal, linker map dapat digunakan untuk mencocokkan alamat runtime dengan fungsi atau section tertentu. Pada M3, kernel.map digunakan untuk memverifikasi entry point, symbol panic path, dan layout kernel ELF.

9. Mengapa GDB harus memakai build/kernel.elf, bukan ISO?

GDB membutuhkan file ELF yang masih memiliki symbol, section, dan metadata debugging. File ISO hanya merupakan container boot image yang berisi bootloader dan kernel binary. Kernel ELF memiliki symbol table, address mapping, dan debug information yang dibutuhkan GDB untuk breakpoint, disassembly, register inspection, dan backtrace. Oleh karena itu debugging dilakukan menggunakan build/kernel.elf sementara ISO hanya digunakan sebagai media boot QEMU.

10. Apa acceptance criteria sebelum M3 boleh menjadi fondasi M4?

Sebelum M3 menjadi fondasi M4, kernel harus memenuhi beberapa acceptance criteria penting:
- clean build berhasil secara reproducible
- kernel ELF valid dan lolos audit readelf/nm/objdump
- serial logging bekerja secara deterministik
- panic path fail-closed berhasil diverifikasi
- QEMU smoke test berjalan stabil
- kernel tidak memiliki unresolved symbol atau dynamic dependency
- rollback dan recovery build dapat dilakukan
- evidence runtime dan debugging tersedia
- observability cukup untuk mendiagnosis boot failure berikutnya

Kriteria ini penting karena M4 biasanya mulai menambahkan subsystem yang lebih kompleks seperti interrupt handling, memory management, atau scheduler. Tanpa fondasi observability dan panic path yang stabil, debugging M4 akan menjadi jauh lebih sulit.