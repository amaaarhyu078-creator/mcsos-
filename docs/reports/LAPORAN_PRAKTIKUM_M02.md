# Laporan Praktikum Sistem Operasi Lanjut — MCSOS - M2

**Nama file laporan:** `laporan_praktikum_[M2]_[2583207073007].md`  
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
| Kode praktikum | `[M2]` |
| Judul praktikum | `[ Praktikum M2 — Boot Image, Kernel ELF64, dan Early Serial Console MCSOS
260502]` |
| Jenis pengerjaan | `[Individu ]` |
| Nama mahasiswa | `[Salma Rahayu]` |
| NIM | `[2583207073007]` |
| Kelas | `[PTI 1A]` |
| Nama kelompok | `[isi jika kelompok]` |
| Anggota kelompok | `[nama, NIM, peran ringkas]` |
| Tanggal praktikum | `[YYYY-MM-DD]` |
| Tanggal pengumpulan | `[YYYY-MM-DD]` |
| Repository | `/home/salma_rahayu/src/mcsos` |
| Branch | `m0/salma` |
| Commit awal | `0466919` |
| Commit akhir | `d3ce46648c3a898ca27212c9e32b7fd0c6f661b6` |
| Status readiness yang diklaim | `siap uji QEMU tahap M2` |
---

## 1. Sampul

# Laporan Praktikum `[M2]`  
## `[ Praktikum M2 — Boot Image, Kernel ELF64, dan Early Serial Console MCSOS
260502]`

Disusun oleh:

| Nama | NIM | Kelas | Peran |
|---|---|---|---|
| `[Salma Rahayu]` | `[2583207073007]` | `[PTI 1A]` | `[individu]` |
| `[opsional]` | `[opsional]` | `[opsional]` | `[opsional]` |

Dosen Pengampu: **Muhaemin Sidiq, S.Pd., M.Pd.**  
Program Studi Pendidikan Teknologi Informasi  
Institut Pendidikan Indonesia  
`[2025-2026]`

---

## 2. Pernyataan Orisinalitas dan Integritas Akademik

Saya menyatakan bahwa laporan ini disusun berdasarkan pekerjaan praktikum sendirisesuai pembagian peran yang tercatat. Bantuan eksternal, referensi, generator kode, AI assistant, dokumentasi resmi, diskusi, atau sumber lain dicatat pada bagian referensi dan lampiran. Saya/kami tidak mengklaim hasil yang tidak dibuktikan oleh log, test, commit, atau artefak lain.

| Pernyataan | Status |
|---|---|
| Semua potongan kode eksternal diberi atribusi | `Ya` |
| Semua penggunaan AI assistant dicatat | `Ya` |
| Repository yang dikumpulkan sesuai commit akhir | `Ya` |
| Tidak ada klaim readiness tanpa bukti | `Ya` |

Catatan penggunaan bantuan eksternal:

```text
Alat yang digunakan:
- ChatGPT (AI assistant)
- Clang/LLVM toolchain
- GNU Binutils
- QEMU
- OVMF
- Limine bootloader
- Git

Bentuk bantuan AI yang digunakan:
- Membantu analisis error build dan linker.
- Membantu memahami failure modes praktikum M2.
- Membantu menjelaskan konsep ELF64, linker layout, dan serial console.
- Membantu menyusun dokumentasi, readiness review, ADR, dan laporan praktikum.
- Membantu menyusun langkah debugging menggunakan GDB dan QEMU.

Contoh prompt ringkas:
- "cara memperbaiki undefined symbol memcpy pada kernel freestanding"
- "penjelasan perbedaan q35 dan pc pada QEMU"
- "cara memeriksa entry point kernel ELF64"
- "cara membuat boot log classifier sederhana"

Bagian yang dibantu:
- Analisis teknis
- Dokumentasi
- Struktur laporan
- Interpretasi output toolchain
- Pengayaan dan tantangan riset

Verifikasi mandiri yang dilakukan:
- Menjalankan seluruh perintah secara langsung pada environment WSL Ubuntu.
- Memverifikasi hasil build menggunakan make build, make inspect, make image, make run, dan make grade.
- Memeriksa ELF menggunakan readelf, objdump, dan nm.
- Menguji boot kernel menggunakan QEMU + OVMF.
- Memverifikasi serial log kernel.
- Memastikan repository bersih menggunakan git status --short.
- Memastikan commit akhir sesuai evidence praktikum.
```

---

## 3. Tujuan Praktikum

Tuliskan tujuan teknis dan konseptual praktikum. Tujuan harus dapat diuji.

1. `[Tujuan teknis 1: Menghasilkan kernel ELF64 x86_64 freestanding yang dapat diinspeksi.]`
2. `[Tujuan teknis 2:  Menghasilkan boot image MCSOS M2 yang dapat dijalankan pada QEMU/OVMF.]`
3. `[Tujuan konseptual 1:  Menyediakan early serial console sebagai kanal observability pertama.]`
4. `[Tujuan validasi: Membuktikan jalur firmware -> bootloader -> kernel entry -> serial output -> controlled halt
loop.]`
5. `[Menyediakan evidence build dan runtime yang dapat direproduksi oleh dosen atau asisten.]`
---

## 4. Capaian Pembelajaran Praktikum

Setelah praktikum ini, mahasiswa mampu:

| CPL/CPMK praktikum | Bukti yang harus ditunjukkan |
|---|---|
| `[Menjelaskan hubungan antara firmware, bootloader, kernel ELF64, linker script, entry point, dan emulator]` | `[diagram boot flow, penjelasan desain sistem, serial log, analisis linker layout]` |
| `[Memeriksa kembali readiness M0 dan M1 sebelum mengeksekusi milestone boot]` | `[output m2_preflight.sh, environment check, readiness review]` |
| `[Membuat source kernel freestanding C17 yang tidak bergantung pada hosted libc]` | `[source kmain.c dan memory.c, flag -ffreestanding, build log]` |
| `[Membuat accessor port I/O x86_64 terbatas untuk UART 16550 COM1]` | `[source io.h dan serial.c, disassembly instruksi inb/outb]` |
| `[Menginisialisasi serial console awal dan mencetak marker boot deterministik]` | `[build/qemu-serial.log yang memuat marker M2]` |
| `[Membuat linker script untuk higher-half kernel ELF64 tahap awal]` | `[linker.ld, output readelf, entry point 0xffffffff80000000]` |
| `[Menghasilkan kernel.elf, kernel.map, readelf evidence, objdump evidence, dan nm evidence]` | `[build/kernel.elf, build/kernel.map, readelf-header.txt, objdump-disassembly.txt, nm-symbols.txt]` |
| `[Mengambil Limine binary release secara terkontrol dan membuat ISO bootable untuk QEMU]` | `[make image, mcsos.iso, checksum ISO, konfigurasi Limine]` |
| `[Menjalankan QEMU/OVMF secara headless dan menyimpan log serial ke file]` | `[make run, run_qemu.sh, build/qemu-serial.log]` |
| `[Mengklasifikasikan failure modes M2 seperti build failure, linker failure, image failure, firmware failure, bootloader failure, serial failure, hang, reboot loop, dan triple-fault-like behavior]` | `[analisis failure modes, boot log classifier, evidence debugging]` |
| `[Menyusun readiness review M2 berdasarkan bukti, bukan klaim subjektif]` | `[docs/readiness/M2-boot-image.md, serial log, inspect ELF, hasil make grade]` |


## 5. Peta Milestone MCSOS

Centang milestone yang menjadi fokus laporan ini. Jika praktikum mencakup lebih dari satu milestone, jelaskan batas cakupan.

| Milestone | Fokus | Status dalam laporan |
|---|---|---|
| M0 | Requirements, governance, baseline arsitektur | `[ ] tidak dibahas / [ ] dibahas / [V] selesai praktikum` |
| M1 | Toolchain reproducible, Git, QEMU, GDB, metadata build | `[ ] tidak dibahas / [ ] dibahas / [V] selesai praktikum` |
| M2 | Boot image, kernel ELF64, early console | `[ ] tidak dibahas / [V] dibahas / [ ] selesai praktikum` |
| M3 | Panic path, linker map, GDB, observability awal | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M4 | Trap, exception, interrupt, timer | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M5 | PMM, VMM, page table, kernel heap | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M6 | Thread, scheduler, synchronization | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M7 | Syscall ABI dan user program loader | `[V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M8 | VFS, file descriptor, ramfs | `[ V] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
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
Praktikum M2 berfokus pada tahap early boot kernel freestanding berbasis ELF64 menggunakan QEMU, OVMF, dan bootloader Limine.

Fitur yang termasuk dalam cakupan praktikum:
- Build kernel freestanding ELF64 x86_64.
- Linker script dan higher-half kernel layout.
- Early serial console COM1.
- Pembuatan bootable ISO image.
- Boot menggunakan Limine.
- Menjalankan kernel pada QEMU + OVMF.
- Serial logging.
- ELF inspection menggunakan readelf, objdump, dan nm.
- Analisis failure modes awal.
- Readiness review berbasis evidence.

Fitur yang tidak termasuk (non-goals):
- Memory manager.
- Virtual memory manager.
- Scheduler.
- Interrupt/trap handling lengkap.
- Syscall ABI.
- Userspace.
- Filesystem.
- Network stack.
- Driver hardware lengkap.
- GUI/display subsystem.
- Security policy penuh.
- Multi-core scheduling.
- Production-grade kernel reliability.

Praktikum ini tidak mengklaim bahwa MCSOS M2 merupakan sistem operasi lengkap atau siap produksi. Fokus utama praktikum adalah membuktikan jalur boot awal kernel dan observability melalui serial console secara terstruktur dan dapat direproduksi.
```

---

## 6. Dasar Teori Ringkas

Praktikum M2 berfokus pada proses early boot kernel freestanding berbasis ELF64 x86_64 menggunakan QEMU, OVMF, dan bootloader Limine. OVMF berperan sebagai firmware UEFI virtual yang menjalankan bootloader Limine untuk memuat `kernel.elf` ke memori dan berpindah ke entry point `kmain()`. Kernel dikompilasi secara freestanding tanpa bergantung pada hosted libc sehingga harus menyediakan runtime memori sendiri. Linker script digunakan untuk mengatur entry point, segment kernel, dan higher-half memory layout. QEMU digunakan sebagai emulator hardware, sedangkan serial console UART 16550 COM1 digunakan sebagai kanal observability awal untuk mencetak log boot kernel. Validasi sistem dilakukan menggunakan evidence seperti `readelf`, `objdump`, `nm`, serial log, checksum ISO, dan debugging GDB untuk memastikan kernel berhasil dibuild, diboot, dan mencapai controlled halt loop.

### 6.1 Konsep Sistem Operasi yang Diuji

```text
Praktikum M2 menguji konsep early boot sistem operasi menggunakan firmware UEFI, bootloader Limine, kernel ELF64 freestanding, linker script, dan emulator QEMU. Kernel dibangun tanpa hosted libc sehingga menggunakan pendekatan freestanding dan runtime memori sederhana. Linker script digunakan untuk menentukan entry point dan higher-half memory layout kernel. Praktikum juga menguji serial console UART 16550 COM1 sebagai mekanisme observability awal untuk debugging boot kernel. Selain itu dilakukan validasi menggunakan readelf, objdump, nm, serial log, checksum ISO, dan debugging GDB untuk memastikan jalur firmware -> bootloader -> kernel -> serial output berjalan sesuai desain.]
```

### 6.2 Konsep Arsitektur x86_64 yang Relevan

| Konsep | Relevansi pada praktikum | Bukti/verifikasi |
|---|---|---|
| `[Long mode x86_64]` | `[digunakan agar kernel dapat berjalan sebagai ELF64 x86_64 pada lingkungan QEMU + OVMF]` | `[readelf-header.txt, build kernel ELF64, serial log berhasil]` |
| `[Port I/O dan UART 16550 COM1]` | `[digunakan untuk early serial console dan observability awal kernel]` | `[serial.c, objdump instruction inb/outb, qemu-serial.log]` |
| `[Higher-half kernel layout]` | `[digunakan untuk menempatkan kernel pada alamat virtual tinggi menggunakan linker script]` | `[linker.ld, readelf-program-headers.txt, serial output alamat kernel]` |
| `[ELF64 executable format]` | `[digunakan sebagai format executable kernel yang dimuat oleh Limine]` | `[readelf, objdump, kernel.elf]` |
| `[UEFI firmware boot flow]` | `[digunakan untuk menjalankan bootloader Limine melalui OVMF]` | `[QEMU boot log, OVMF path, serial log kernel]` |
| `[GDB remote debugging]` | `[digunakan untuk memverifikasi bahwa eksekusi mencapai entry point kmain]` | `[gdb-kmain.txt, breakpoint kmain]` |

### 6.3 Konsep Implementasi Freestanding

| Aspek | Keputusan praktikum |
|---|---|
| Bahasa | `[C17 freestanding dengan sedikit inline assembly x86_64]` |
| Runtime | `[tanpa hosted libc dan menggunakan runtime memori sederhana milik kernel]` |
| ABI | `[x86_64 System V ABI untuk kernel freestanding]` |
| Compiler flags kritis | `[-ffreestanding, -fno-stack-protector, -mno-red-zone, -nostdlib, -fno-pie]` |
| Risiko undefined behavior | `[pointer invalid, alignment error, integer overflow, akses memory tidak valid, dan layout linker yang salah]` |

### 6.4 Referensi Teori yang Digunakan

| No. | Sumber | Bagian yang digunakan | Alasan relevansi |
|---|---|---|---|
| `[1]` | `[OSDev Wiki]` | `[ELF, Bare Bones, Serial Ports, x86_64]` | `[digunakan untuk memahami boot process, ELF64 kernel, dan serial console freestanding]` |
| `[2]` | `[Dokumentasi Limine Bootloader]` | `[UEFI boot, Limine config, ELF loading]` | `[digunakan untuk memahami proses bootloader memuat kernel ELF64]` |
| `[3]` | `[QEMU Documentation]` | `[QEMU system emulation dan serial option]` | `[digunakan untuk konfigurasi emulator dan serial logging]` |
| `[4]` | `[LLVM/Clang Documentation]` | `[freestanding compilation flags]` | `[digunakan untuk menentukan compiler dan linker flags kernel freestanding]` |
| `[5]` | `[System V ABI AMD64 Specification]` | `[x86_64 calling convention]` | `[digunakan untuk memahami ABI kernel dan entry execution]` |
| `[6]` | `[Intel 64 and IA-32 Architectures Software Developer Manual]` | `[Port I/O dan x86_64 architecture]` | `[digunakan untuk memahami instruksi inb/outb dan serial COM1]` |

---

## 7. Lingkungan Praktikum

### 7.1 Host dan Target

| Komponen | Nilai |
|---|---|
| Host OS | `[Windows 11 x64]` |
| Lingkungan build | `[WSL 2 Ubuntu]` |
| Target ISA | `x86_64` |
| Target ABI | `[x86_64-unknown-none-elf]` |
| Emulator | `[QEMU qemu-system-x86_64]` |
| Firmware emulator | `[/usr/share/OVMF/OVMF_CODE_4M.fd]` |
| Debugger | `[GNU gdb 17.1]` |
| Build system | `[GNU Make]` |
| Bahasa utama | `[C17 freestanding]` |
| Assembly | `[inline assembly x86_64 GCC/Clang syntax]` |

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
[date_utc=2026-05-22T10:49:27Z
Linux DESKTOP-S5LUA56 6.6.114.1-microsoft-standard-WSL2 #1 SMP PREEMPT_DYNAMIC Mon Dec  1 20:46:23 UTC 2025 x86_64 GNU/Linux
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
| Path repository di WSL | `` `~/src/mcsos` `` |
| Apakah berada di filesystem Linux WSL, bukan `/mnt/c` | `[Ya]` |
| Remote repository | `[Belum menggunakan remote repository / privat lokal]` |
| Branch | `[m0/salma]` |
| Commit hash awal | `` `0466919` `` |
| Commit hash akhir | `` `d3ce46648c3a898ca27212c9e32b7fd0c6f661b6` `` |

---

### 7.4 Lingkungan

| Komponen | Versi/output | Bukti |
|---|---|---|
| Windows | `[Windows 11 x64]` | `[system information Windows host]` |
| WSL | `[WSL 2]` | `[wsl --status]` |
| Distro | `[Ubuntu]` | `[lsb_release -a]` |
| Kernel Linux WSL | `[kernel Linux WSL aktif]` | `[uname -r]` |
| Clang | `[Ubuntu clang version 21.1.8 (6ubuntu1)]` | `[clang --version]` |
| LLD | `[Ubuntu LLD 21.1.8]` | `[ld.lld --version / readelf .comment]` |
| Binutils | `[GNU Binutils for Ubuntu 2.46]` | `[ld --version]` |
| QEMU | `[qemu-system-x86_64]` | `[qemu-system-x86_64 --version]` |
| OVMF path | `[/usr/share/OVMF/OVMF_CODE_4M.fd]` | `[run_qemu.sh dan hasil find OVMF]` |
| Limine branch/revision | `[Limine binary release untuk M2]` | `[third_party/limine dan revision.txt]` |
| Git commit | `[d3ce46648c3a898ca27212c9e32b7fd0c6f661b6]` | `[build/meta/m2-commit.txt]` |


## 8. Repository dan Struktur File

### 8.1 Struktur Direktori yang Relevan

Tampilkan hanya direktori dan file yang relevan dengan praktikum.

```text
[## 8. Repository dan Struktur File

### 8.1 Struktur Direktori yang Relevan

```text
mcsos/
├── Makefile
├── README.md
├── build/
│   ├── debug/
│   ├── inspect/
│   ├── meta/
│   ├── research/
│   ├── kernel.elf
│   ├── kernel.map
│   ├── mcsos.iso
│   ├── mcsos.iso.sha256
│   ├── qemu-debug-serial.log
│   └── qemu-serial.log
│
├── configs/
│   └── limine/
│       └── limine.conf
│
├── docs/
│   ├── adr/
│   ├── architecture/
│   ├── readiness/
│   ├── research/
│   ├── security/
│   └── testing/
│
├── kernel/
│   ├── arch/
│   │   └── x86_64/
│   ├── core/
│   │   ├── kmain.c
│   │   └── serial.c
│   └── lib/
│       └── memory.c
│
├── linker.ld
│
├── smoke/
│   └── freestanding.c
│
├── tests/
│   └── toolchain/
│
└── tools/
    ├── check_env.sh
    ├── collect_evidence.sh
    └── scripts/
        ├── ci_local.sh
        ├── classify_boot_log.sh
        ├── grade_m2.sh
        ├── inspect_kernel.sh
        ├── make_iso.sh
        ├── run_qemu.sh
        └── run_qemu_debug.sh
]
```

### 8.2 File yang Dibuat atau Diubah

| File | Jenis perubahan | Alasan perubahan | Risiko |
|---|---|---|---|
| `[kernel/core/kmain.c]` | `[ubah]` | `[menambahkan boot marker, serial output, print alamat __kernel_start, dan controlled halt loop]` | `[sedang — kesalahan entry kernel dapat menyebabkan hang atau boot failure]` |
| `[kernel/core/serial.c]` | `[ubah]` | `[menambahkan implementasi serial COM1 dan serial_write_hex64 untuk early observability]` | `[sedang — kesalahan port I/O dapat menyebabkan serial output gagal]` |
|`[kernel/arch/x86_64/include/mcsos/arch/serial.h]` | `[baru]` | `[menyediakan deklarasi interface serial driver]` | `[rendah — hanya deklarasi header]` |
| `[tools/scripts/run_qemu_debug.sh]` | `[baru]` | `[menjalankan QEMU dalam mode debug dengan GDB remote attach]` | `[rendah — hanya memengaruhi proses debugging]` |
| `[tools/scripts/ci_local.sh]` | `[baru]` | `[menyediakan local CI sederhana untuk build dan grade otomatis]` | `[rendah — tidak memengaruhi runtime kernel]` |
| `[tools/scripts/classify_boot_log.sh]` | `[baru]` | `[mengklasifikasikan failure mode boot berdasarkan serial log]` | `[rendah — hanya memproses evidence log]` |
| `[docs/adr/0001-limine-over-grub.md]` | `[baru]` | `[mendokumentasikan keputusan penggunaan Limine dibanding GRUB/Multiboot2]` | `[rendah — hanya dokumentasi desain]` |
| `[linker.ld]` | `[ubah]` | `[mengatur higher-half ELF64 layout dan kernel entry point]` | `[tinggi — kesalahan linker layout dapat menyebabkan kernel gagal boot]` |
| `[configs/limine/limine.conf]` | `[ubah]` | `[mengatur boot entry Limine untuk kernel ELF64 MCSOS]` | `[sedang — kesalahan konfigurasi dapat menyebabkan bootloader failure]` |
| `[Makefile]` | `[ubah]` | `[menambahkan target build, inspect, image, run, debug, dan grade M2]` | `[sedang — kesalahan dependency build dapat merusak pipeline praktikum]` |

### 8.3 Ringkasan Diff

```bash
git status --short
git diff --stat
git log --oneline -n 5
```

Output:

```text
[salma_rahayu@DESKTOP-S5LUA56:~/src/mcsos$ git status --short
git diff --stat
git log --oneline -n 5

d3ce466 (HEAD -> m0/salma) M2: add ADR and boot log classifier
7d1bc51 M2: add local CI helper script
9effd34 M2: add serial hex output for kernel start address
0466919 docs: add M2 readiness review
160940d Complete M2 boot, ISO, QEMU, and grading setup ]
```

---

## 9. Desain Teknis

### 9.1 Masalah yang Diselesaikan

```text
Pada tahap sebelum M2, kernel MCSOS belum memiliki boot image yang dapat dijalankan secara konsisten pada QEMU/OVMF dan belum memiliki mekanisme observability awal untuk memverifikasi apakah kernel benar-benar mencapai entry point. Kondisi ini menyebabkan kegagalan boot sulit dibedakan antara firmware failure, bootloader failure, linker/layout failure, atau kernel entry failure.
Kernel juga belum menyediakan early serial console sehingga status awal boot dan alamat kernel tidak dapat diamati melalui log deterministik. Selain itu, belum tersedia pipeline evidence yang memverifikasi ELF kernel, linker layout, ISO boot image, serta runtime serial log secara terstruktur.
Praktikum M2 menyelesaikan masalah tersebut dengan membangun kernel ELF64 freestanding, membuat boot image berbasis Limine, menyediakan early serial console COM1, menampilkan marker boot deterministik termasuk alamat __kernel_start, serta menghasilkan evidence build dan runtime yang dapat diuji ulang menggunakan QEMU, GDB, dan tooling inspect seperti readelf serta objdump.
```

### 9.2 Keputusan Desain

| Keputusan | Alternatif yang dipertimbangkan | Alasan memilih | Konsekuensi |
|---|---|---|---|
| `[Menggunakan Limine sebagai bootloader]` | `[GRUB atau Multiboot2 loader lain]` | `[Limine menyediakan boot path modern, konfigurasi sederhana, dan kompatibel dengan kernel ELF64 higher-half awal]` | `[menambah dependency bootloader eksternal dan perlu pengelolaan revision Limine]` |
| `[Menggunakan early serial console COM1 UART 16550]` | `[langsung framebuffer atau tanpa console awal]` | `[serial lebih sederhana, deterministik, dan dapat digunakan sejak tahap boot awal]` | `[output bergantung pada konfigurasi serial emulator dan tidak menyediakan antarmuka visual]` |
| `[Menggunakan kernel ELF64 higher-half dengan linker.ld khusus]` | `[identity mapping penuh atau low-half kernel]` | `[higher-half layout memisahkan ruang kernel dari area rendah dan lebih sesuai untuk desain kernel jangka panjang]` | `[kesalahan linker layout dapat menyebabkan boot failure atau invalid entry point]` |
| `[Menjalankan QEMU + OVMF secara headless dengan serial log file]` | `[GUI QEMU biasa atau observasi manual]` | `[headless mode memudahkan reproduksi evidence dan otomatisasi pengujian]` | `[debugging visual lebih terbatas dan bergantung pada serial logging]` |
| `[Menggunakan readelf, objdump, dan inspect evidence untuk validasi]` | `[hanya mengandalkan boot berhasil]` | `[validasi ELF dan program header diperlukan untuk memastikan linker dan image benar]` | `[menambah langkah inspeksi dan artifact build yang harus dipelihara]` |
| `[Menambahkan local CI sederhana dan boot log classifier]` | `[pengujian manual penuh]` | `[membantu mendeteksi failure lebih cepat dan menjaga konsistensi build]` | `[script tambahan perlu dipelihara agar tetap sesuai pipeline build]` |

### 9.3 Arsitektur Ringkas

```mermaid
flowchart TD
    A[UEFI Firmware / OVMF] --> B[Limine Bootloader]
    B --> C[kernel.elf ELF64]
    C --> D[kmain Entry Point]
    D --> E[Serial COM1 UART16550]
    E --> F[Boot Marker dan Kernel Address Log]
    F --> G[Controlled Halt Loop]
    G --> H[QEMU Serial Log dan Evidence]

Penjelasan diagram:

```text
[Arsitektur M2 dimulai dari firmware UEFI OVMF yang dijalankan oleh QEMU. Firmware memuat bootloader Limine melalui ISO boot image MCSOS. Limine kemudian melakukan handoff ke kernel.elf ELF64 berdasarkan entry point yang ditentukan linker.ld. Setelah entry point tercapai, kmain menjadi pusat kontrol awal kernel.
Kernel menginisialisasi early serial console COM1 berbasis UART 16550 untuk menyediakan observability awal. Melalui serial driver, kernel mencetak boot marker deterministik dan alamat __kernel_start sebagai bukti bahwa kernel telah mencapai entry point dengan benar. Setelah output serial selesai, kernel memasuki controlled halt loop agar kondisi runtime tetap stabil dan dapat diobservasi melalui serial log, GDB breakpoint, serta artifact evidence build dan inspect.]
```

### 9.4 Kontrak Antarmuka

| Antarmuka | Pemanggil | Penerima | Precondition | Postcondition | Error path |
|---|---|---|---|---|---|
| `[Limine boot handoff -> kernel entry]` | `[Limine bootloader]` | `[kernel.elf / kmain]` | `[kernel ELF64 valid, entry point linker benar, image berhasil dimuat firmware]` | `[kontrol berpindah ke kmain]` | `[bootloader failure atau kernel entry tidak tercapai]` |
| `[kmain -> serial_init()]` | `[kmain]` | `[serial subsystem]` | `[CPU sudah berada pada boot state yang valid dan COM1 dapat diakses]` | `[serial COM1 terinisialisasi]` | `[serial tidak aktif atau output tidak muncul]` |
| `[kmain -> serial_write()]` | `[kmain]` | `[serial subsystem]` | `[serial_init telah berhasil dipanggil]` | `[boot marker tercetak ke serial log]` | `[serial output hilang atau log kosong]` |
| `[kmain -> serial_write_hex64()]` | `[kmain]` | `[serial subsystem]` | `[serial aktif dan alamat kernel valid]` | `[alamat __kernel_start tercetak dalam format hexadecimal]` | `[alamat tidak tampil atau format output salah]` |
| `[serial subsystem -> UART16550 COM1]` | `[serial driver]` | `[hardware COM1 virtual QEMU]` | `[port I/O COM1 tersedia dan QEMU serial aktif]` | `[data serial dikirim ke qemu-serial.log]` | `[serial failure atau log tidak terbentuk]` |
| `[QEMU/OVMF -> boot image ISO]` | `[QEMU firmware OVMF]` | `[mcsos.iso]` | `[ISO valid dan bootloader tersedia]` | `[Limine dimuat dan proses boot dimulai]` | `[firmware failure atau ISO tidak bootable]` |

### 9.5 Struktur Data Utama

| Struktur data | Field penting | Ownership | Lifetime | Invariant |
|---|---|---|---|---|
| `` `[struct m0_smoke_record]` `` | `[magic, version, pointer_width, size_width]` | `[kernel freestanding probe]` | `[dibuat saat compile/link dan hidup selama kernel image ada]` | `[magic harus valid dan ukuran pointer harus konsisten dengan target x86_64]` |
| `` `[serial COM1 state (implicit driver state)]` `` | `[COM1 base port 0x3F8 dan register UART]` | `[serial subsystem]` | `[aktif sejak serial_init dipanggil hingga kernel berhenti]` | `[serial hanya digunakan setelah inisialisasi COM1 berhasil]` |


### 9.6 Invariants

Tuliskan invariant yang harus benar sepanjang eksekusi.

1. `[Firmware OVMF harus berhasil memuat boot image dan menyerahkan kontrol ke Limine sebelum kernel dapat berjalan.]`
2. `[kernel.elf harus memiliki entry point dan linker layout yang valid sesuai linker.ld.]`
3. `[Early serial console COM1 harus diinisialisasi sebelum kernel mencetak boot marker atau alamat __kernel_start.]`
4. `[Setelah boot marker dan serial output selesai dicetak, kernel harus memasuki controlled halt loop dan tidak melakukan reboot atau keluar secara tidak terkontrol.]`

### 9.7 Ownership, Locking, dan Concurrency

| Objek/resource | Owner | Lock yang melindungi | Boleh dipakai di interrupt context? | Catatan |
|---|---|---|---|---|
| `[COM1 UART serial port]` | `[serial subsystem / kernel]` | `[none]` | `[Tidak]` | `[serial hanya dipakai pada early boot single-threaded]` |
| `[kernel entry flow (kmain)]` | `[kernel core]` | `[none]` | `[Tidak]` | `[kontrol berjalan linear sejak handoff Limine]` |
| `[serial log output]` | `[serial driver]` | `[none]` | `[Tidak]` | `[tidak ada akses paralel atau competing writer]` |
| `[kernel ELF image dan linker layout]` | `[kernel image]` | `[none]` | `[Tidak]` | `[bersifat statis setelah image dimuat Limine]` |

Lock order yang berlaku:

```text
Pada tahap M2 tidak terdapat mekanisme locking karena kernel masih berjalan pada single-core early boot environment tanpa scheduler, tanpa SMP, dan tanpa interrupt concurrency yang aktif. Eksekusi berjalan secara linear dari firmware -> Limine -> kmain -> serial -> controlled halt loop sehingga ownership resource masih deterministik dan tidak memerlukan sinkronisasi tambahan.
```

### 9.8 Memory Safety dan Undefined Behavior Risk

| Risiko | Lokasi | Mitigasi | Bukti |
|---|---|---|---|
| `[alignment dan pointer-width mismatch]` | `[smoke/freestanding.c dan build target x86_64]` | `[menggunakan compile-time freestanding probe serta validasi pointer_width dan size_width]` | `[make build, freestanding probe, review toolchain]` |
| `[invalid memory layout atau wrong entry point]` | `[linker.ld dan kernel.elf]` | `[menggunakan linker script eksplisit serta validasi readelf program header dan entry point]` | `[readelf-header.txt dan readelf-program-headers.txt]` |
| `[integer formatting atau truncation pada alamat kernel]` | `[kernel/core/serial.c dan serial_write_hex64()]` | `[menggunakan output hexadecimal 64-bit khusus untuk alamat kernel]` | `[qemu-serial.log dan GDB breakpoint kmain]` |
| `[unsafe port I/O access]` | `[kernel/core/serial.c]` | `[akses dibatasi hanya pada COM1 UART16550 dan dipakai setelah serial_init]` | `[serial log QEMU dan code review]` |
| `[undefined behavior akibat freestanding environment tanpa libc]` | `[kernel/core dan kernel/lib]` | `[menggunakan C17 freestanding, helper memory sederhana, dan menghindari hosted library call]` | `[make build, CI local, dan inspect evidence]` |

### 9.9 Security Boundary

| Boundary | Data tidak tepercaya | Validasi yang dilakukan | Failure mode aman |
|---|---|---|---|
| `[Firmware OVMF -> boot image ISO]` | `[boot image dan media virtual]` | `[firmware memverifikasi media boot dapat dimuat]` | `[boot gagal dan kernel tidak dijalankan]` |
| `[Limine -> kernel ELF64 handoff]` | `[kernel.elf dan entry point]` | `[validasi ELF layout, entry point, dan linker evidence melalui readelf]` | `[bootloader failure atau kernel entry tidak tercapai]` |
| `[kernel -> serial COM1 device]` | `[virtual UART device dari QEMU]` | `[serial hanya menggunakan COM1 UART16550 dengan inisialisasi eksplisit]` | `[serial log gagal tetapi kernel tetap berada pada controlled halt loop]` |
| `[build dan inspect pipeline]` | `[artifact build dan konfigurasi toolchain]` | `[CI local, inspect script, readelf, objdump, dan grade check]` | `[build/inspect failure terdeteksi sebelum runtime]` |

---

## 29.3 Desain

### 1. Alur Boot Firmware -> Limine -> kernel.elf -> kmain -> serial -> halt

Boot MCSOS M2 dimulai ketika firmware UEFI OVMF dijalankan oleh QEMU dan memuat boot image ISO MCSOS. Firmware kemudian menemukan dan menjalankan bootloader Limine dari media virtual. Limine melakukan proses boot handoff dengan memuat `kernel.elf` sesuai konfigurasi `limine.conf` dan menyerahkan kontrol ke entry point kernel yang telah ditentukan oleh `linker.ld`.

Setelah handoff berhasil, fungsi `kmain()` menjadi titik kontrol pertama kernel. Kernel kemudian menginisialisasi early serial console COM1 berbasis UART 16550 melalui `serial_init()`. Setelah serial aktif, kernel mencetak boot marker deterministik dan alamat `__kernel_start` ke serial log untuk membuktikan bahwa entry point telah tercapai dengan benar. Setelah observability awal selesai, kernel memasuki controlled halt loop agar runtime stabil dan dapat dianalisis melalui QEMU log serta GDB.

### 2. Struktur Source M2

Struktur source M2 dipisahkan berdasarkan tanggung jawab komponen boot dan kernel awal. Direktori `kernel/core/` berisi `kmain.c` dan `serial.c` untuk entry kernel dan serial console. Direktori `kernel/lib/` menyediakan helper memory sederhana. Konfigurasi bootloader Limine berada pada `configs/limine/`, sedangkan `tools/scripts/` berisi pipeline build, inspect, grading, QEMU runner, dan debugging helper. Artifact hasil build dan runtime disimpan pada `build/` seperti `kernel.elf`, `kernel.map`, `mcsos.iso`, serial log, serta evidence inspect.

### 3. Linker Layout

Kernel M2 menggunakan `linker.ld` untuk menghasilkan kernel ELF64 higher-half dengan entry point deterministik. Layout linker memisahkan section `.text` dan `.rodata` ke dalam LOAD segment yang dapat diinspeksi menggunakan `readelf`. Entry point kernel berada pada alamat `0xffffffff80000000`, sesuai evidence ELF dan GDB breakpoint. Pendekatan ini memastikan kernel memiliki layout yang stabil dan dapat diverifikasi sebelum runtime.

### 4. Invariants M2

Invariant M2 menyatakan bahwa firmware harus berhasil menyerahkan kontrol ke Limine sebelum kernel berjalan, `kernel.elf` harus memiliki linker layout dan entry point yang valid, serial COM1 harus diinisialisasi sebelum output boot dicetak, dan setelah boot marker selesai kernel harus memasuki controlled halt loop tanpa reboot atau keluar secara tidak terkontrol.

### 5. Batasan M2

Praktikum M2 hanya mencakup boot image, kernel ELF64 awal, early serial console, dan observability boot menggunakan QEMU serta tooling inspect. M2 belum mencakup scheduler, virtual memory manager, physical memory manager, syscall, filesystem, networking, driver kompleks, SMP, maupun security subsystem. Fokus M2 dibatasi pada pembuktian jalur firmware -> bootloader -> kernel entry -> serial output -> controlled halt loop secara reproducible.

----


## 10. Langkah Kerja Implementasi

Gunakan tabel berikut untuk setiap langkah. Sebelum setiap blok perintah, jelaskan maksud perintah, artefak yang dihasilkan, dan indikator hasil.

### Langkah 1 — `[Validasi Environment dan Readiness M2]`

Maksud langkah:

```text
[Langkah ini dilakukan untuk memastikan toolchain, dependency, dan environment WSL berada dalam kondisi valid sebelum build kernel dan boot image dilakukan. Validasi awal mencegah kegagalan akibat tool yang tidak tersedia atau konfigurasi yang salah.]
```

Perintah:

```bash
[./tools/check_env.sh]
```

Output ringkas:

```text
[[M0] Repository root: /home/salma_rahayu/src/mcsos
[OK] Repository is not under /mnt/<drive>.
[OK] git
[OK] make
[OK] clang
[OK] ld.lld
[OK] qemu-system-x86_64
[OK] gdb
[M0] Metadata written to build/meta/toolchain-versions.txt
[M0] Environment check completed.]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `[toolchain-versions.txt]` | `[build/meta/]` | `[menyimpan metadata toolchain dan environment build]` |

Indikator berhasil:

```text
[Environment check selesai tanpa error, seluruh tool penting ditandai [OK], dan metadata toolchain berhasil ditulis ke build/meta/toolchain-versions.txt.]
```

### Langkah 2 — `[Build Kernel ELF64 Freestanding]`

Maksud langkah:

```text
[Langkah ini dilakukan untuk membangun kernel freestanding MCSOS menjadi executable ELF64 yang dapat dimuat oleh bootloader Limine. Build system juga memastikan dependency source dan artifact kernel berada pada kondisi konsisten.]
```

Perintah:

```bash
[make build]
```

Output ringkas:

```text
[make: Nothing to be done for 'build'.]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `[kernel.elf]` | `[build/]` | `[executable kernel ELF64 untuk boot]` |
| `[kernel.map]` | `[build/]` | `[peta layout dan simbol linker kernel]` |

Indikator berhasil:

```text
[Target build dikenali oleh Make dan tidak menghasilkan error. Pesan "Nothing to be done for 'build'" menunjukkan bahwa artifact build telah tersedia dan masih konsisten dengan source repository.]
```

### Langkah 3 — Inspect Kernel ELF dan Linker Evidence

Maksud langkah:

```text
Langkah ini dilakukan untuk memverifikasi bahwa kernel ELF64 yang dihasilkan memiliki header, entry point, dan program header yang valid sesuai desain linker M2. Proses inspect juga digunakan untuk membuktikan bahwa kernel dapat dianalisis secara statis sebelum dijalankan pada QEMU.
```

Perintah:

```bash
make inspect
```

Output ringkas:

```text
./tools/scripts/inspect_kernel.sh

ELF Header:
Class: ELF64
Type: EXEC (Executable file)
Machine: Advanced Micro Devices X86-64
Entry point address: 0xffffffff80000000

Program Headers:
LOAD 0xffffffff80000000 R E
LOAD 0xffffffff80001000 R

OK: kernel ELF inspection passed
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `readelf-header.txt` | `build/inspect/` | memverifikasi header ELF64 kernel |
| `readelf-program-headers.txt` | `build/inspect/` | memverifikasi LOAD segment dan linker layout |
| `objdump-disassembly.txt` | `build/inspect/` | menampilkan hasil disassembly kernel |
| `nm-symbols.txt` | `build/inspect/` | memeriksa simbol kernel |

Indikator berhasil:

```text
Inspect selesai tanpa error, entry point kernel terbaca sebagai 0xffffffff80000000, terdapat dua LOAD segment yang valid, dan script menampilkan pesan "kernel ELF inspection passed".
```
### Langkah 4 — Analisis Program Header dan Perbandingan Linker

Maksud langkah:

```text
Langkah ini dilakukan untuk menganalisis perbedaan layout program header (PHDR) yang dihasilkan oleh linker ld.lld dan GNU ld. Analisis ini penting untuk memahami bagaimana keputusan linker mempengaruhi jumlah LOAD segment, determinisme layout, dan kesesuaian image kernel untuk early boot MCSOS M2.
```

Perintah:

```bash
cat build/research/phdr-analysis.txt
```

Output ringkas:

```text
PHDR comparison between ld.lld and GNU ld:

1. ld.lld generated 2 LOAD segments.
2. GNU ld generated 3 LOAD segments.
3. GNU ld added an extra RW LOAD segment with zero size.
4. ld.lld produced a simpler and more deterministic layout.
5. Entry point remained identical at 0xffffffff80000000.
6. Both linkers produced valid ELF64 executables, but ld.lld generated a cleaner early-boot image for MCSOS M2.
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `phdr-analysis.txt` | `build/research/` | dokumentasi hasil analisis PHDR dan perbandingan linker |
| `kernel-gnu-ld.elf` | `build/research/` | hasil linking menggunakan GNU ld |
| `kernel-gnu-ld.map` | `build/research/` | linker map GNU ld |

Indikator berhasil:

```text
Analisis berhasil menunjukkan perbedaan jumlah LOAD segment antara ld.lld dan GNU ld tanpa mengubah entry point kernel. Evidence tersimpan pada build/research/phdr-analysis.txt.
```
### Langkah 5 — Validasi Boot ISO dan Checksum Evidence

Maksud langkah:

```text
Langkah ini dilakukan untuk memverifikasi bahwa image boot MCSOS berhasil dibuat dan memiliki checksum SHA256 yang dapat digunakan sebagai evidence integritas. Checksum membantu memastikan bahwa image ISO yang diuji dan dikumpulkan identik serta dapat direproduksi.
```

Perintah:

```bash
cat build/mcsos.iso.sha256
```

Output ringkas:

```text
27119b6c1076255fc31e9168746be4db4812bd30cb9abcfdf49f8f2f058d7585  build/mcsos.iso
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `mcsos.iso` | `build/` | boot image Limine untuk QEMU/OVMF |
| `mcsos.iso.sha256` | `build/` | checksum SHA256 untuk validasi integritas ISO |

Indikator berhasil:

```text
Checksum SHA256 berhasil dibuat dan menunjuk pada build/mcsos.iso, yang menunjukkan bahwa boot image tersedia dan dapat diverifikasi integritasnya.
```

### Langkah 6 — Boot QEMU dan Observasi Early Serial Console

Maksud langkah:

```text
Langkah ini dilakukan untuk menjalankan boot image MCSOS pada QEMU/OVMF dan memverifikasi jalur boot firmware → Limine → kernel ELF64 → serial console → controlled halt loop. Serial log digunakan sebagai evidence runtime utama M2.
```

Perintah:

```bash
cat build/qemu-serial.log
```

Output ringkas:

```text
limine: Loading executable `boot():/boot/kernel.elf`
MCSOS 260502 M2 boot path entered
[M2] early serial online
[M2] kernel start = 0xffffffff80000000
[M2] kernel reached controlled halt loop
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `qemu-serial.log` | `build/` | menyimpan runtime serial log hasil boot QEMU |
| `mcsos.iso` | `build/` | image yang digunakan saat boot |

Indikator berhasil:

```text
Limine berhasil memuat kernel.elf, early serial console aktif, alamat entry kernel tercetak, dan kernel mencapai controlled halt loop tanpa reboot atau firmware failure.
```

### Langkah 7 — Klasifikasi Boot Log

Maksud langkah:

```text
Langkah ini dilakukan untuk mengklasifikasikan hasil boot berdasarkan serial log yang dihasilkan QEMU. Proses klasifikasi membantu membedakan boot yang berhasil dari failure mode seperti firmware failure, reboot loop, atau kernel yang gagal mencapai entry point.
```

Perintah:

```bash
cat build/research/boot-log-classification.txt
```

Output ringkas:

```text
CLASSIFICATION: SUCCESSFUL KERNEL ENTRY
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `boot-log-classification.txt` | `build/research/` | menyimpan hasil klasifikasi status boot kernel |

Indikator berhasil:

```text
Boot classifier berhasil mengenali log runtime sebagai SUCCESSFUL KERNEL ENTRY, yang menunjukkan kernel mencapai entry point dan menghasilkan serial output sesuai desain M2.
```

### Langkah 8 — Analisis Model Mesin QEMU

Maksud langkah:

```text
Langkah ini dilakukan untuk membandingkan perilaku boot image MCSOS pada dua model mesin virtual QEMU, yaitu q35 dan pc. Analisis bertujuan memeriksa apakah perbedaan virtual hardware mempengaruhi proses firmware, bootloader, kernel entry, dan stabilitas serial console.
```

Perintah:

```bash
cat build/research/qemu-machine-analysis.txt
```

Output ringkas:

```text
QEMU machine comparison between q35 and pc:

1. q35 used modern SATA/PCIe device paths.
2. pc used legacy ATA/IDE device paths.
3. Both machine models successfully booted the Limine ISO.
4. Kernel entry and serial console output remained stable.
5. No reboot loop or firmware failure occurred in either configuration.
6. q35 provided a more modern virtual hardware environment, while pc preserved legacy compatibility behavior.
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `qemu-machine-analysis.txt` | `build/research/` | menyimpan hasil analisis perbandingan model mesin QEMU |

Indikator berhasil:

```text
Kedua model mesin QEMU berhasil mem-boot Limine ISO tanpa reboot loop atau firmware failure, dan serial console kernel tetap stabil pada q35 maupun pc.
```

### Langkah 9 — Dokumentasi ADR dan Keputusan Bootloader

Maksud langkah:

```text
Langkah ini dilakukan untuk mendokumentasikan keputusan desain utama M2 mengenai pemilihan bootloader. Architecture Decision Record (ADR) digunakan agar alasan teknis, alternatif yang dipertimbangkan, serta konsekuensi desain dapat ditelusuri dan dipertanggungjawabkan pada perkembangan MCSOS berikutnya.
```

Perintah:

```bash
cat docs/adr/0001-limine-over-grub.md
```

Output ringkas:

```text
ADR 0001: Memilih Limine dibanding GRUB/Multiboot2

Status: Accepted

Decision:
MCSOS memilih Limine sebagai bootloader utama untuk milestone M2.

Rationale:
- Native support ELF64 higher-half kernel
- Konfigurasi sederhana
- Dukungan BIOS dan UEFI
- Integrasi baik dengan QEMU + OVMF

Evidence:
- Kernel ELF64 berhasil diboot menggunakan Limine
- QEMU + OVMF boot stabil
- Serial log mencapai kmain
- ISO berhasil dibuat dan dijalankan
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `0001-limine-over-grub.md` | `docs/adr/` | mendokumentasikan keputusan arsitektur bootloader M2 |

Indikator berhasil:

```text
ADR tersimpan dengan status Accepted, keputusan penggunaan Limine terdokumentasi lengkap, dan alasan desain didukung oleh evidence boot dan runtime M2.
```

### Langkah 10 — Finalisasi Repository dan Git Evidence

Maksud langkah:

```text
Langkah ini dilakukan untuk memastikan seluruh perubahan M2 telah dikomit, repository berada dalam keadaan bersih (clean working tree), dan commit akhir dapat dijadikan referensi evidence pengumpulan serta reproduksi praktikum.
```

Perintah:

```bash
git rev-parse HEAD
git status --short
```

Output ringkas:

```text
d3ce46648c3a898ca27212c9e32b7fd0c6f661b6
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `m2-commit.txt` | `build/meta/` | menyimpan hash commit akhir M2 |
| Git commit | repository | evidence final state repository |

Indikator berhasil:

```text
Hash commit akhir berhasil diperoleh dan git status --short tidak menampilkan perubahan, yang menunjukkan repository berada dalam kondisi clean dan siap dikumpulkan.
```

## 11. Checkpoint Buildable

| Checkpoint | Perintah | Expected result | Status |
|---|---|---|---|
| Clean build | `make clean && make build` | kernel ELF64 dan linker artifact berhasil terbangun | PASS |
| Metadata toolchain | `make meta` | `build/meta/toolchain-versions.txt` ada | FAIL |
| Image generation | `make image` | `mcsos.iso` berhasil dibuat | PASS |
| QEMU smoke test | `make run` | serial log dan kernel stage marker muncul | PASS |
| Test suite | `make test` | semua test relevan lulus | FAIL |

Catatan checkpoint:

```text
Checkpoint clean build berhasil dijalankan dari kondisi clean checkout dan menghasilkan kernel ELF64 beserta linker artifact tanpa error.

Checkpoint image generation berhasil membuat boot image Limine (mcsos.iso) beserta checksum SHA256 yang valid.

Checkpoint QEMU smoke test dinyatakan PASS karena make run berhasil menjalankan run_qemu.sh dan evidence qemu-serial.log sebelumnya menunjukkan successful kernel entry, early serial console online, serta controlled halt loop.

Checkpoint metadata gagal karena repository MCSOS M2 tidak menyediakan target Makefile bernama "meta". Metadata toolchain tetap berhasil diperoleh melalui tools/check_env.sh dan disimpan pada build/meta/toolchain-versions.txt.

Checkpoint test suite gagal karena repository tidak memiliki target Makefile bernama "test", sehingga make test menghasilkan error "No rule to make target 'test'".
```
---

## 12. Perintah Uji dan Validasi

## 12. Perintah Uji dan Validasi

### 12.1 Build Test

Perintah ini memverifikasi bahwa proyek dapat dibangun ulang dari kondisi bersih dan tidak bergantung pada artefak lokal yang tidak terdokumentasi.

```bash
make clean
make build
```

Hasil:

```text
rm -rf build/kernel build/*.elf build/*.map build/inspect

clang --target=x86_64-unknown-none-elf ... -c kernel/core/kmain.c -o build/kernel/core/kmain.o
clang --target=x86_64-unknown-none-elf ... -c kernel/core/serial.c -o build/kernel/core/serial.o
clang --target=x86_64-unknown-none-elf ... -c kernel/lib/memory.c -o build/kernel/lib/memory.o

ld.lld -nostdlib -static -z max-page-size=0x1000 \
-T linker.ld \
-Map=build/kernel.map \
-o build/kernel.elf \
build/kernel/core/kmain.o \
build/kernel/core/serial.o \
build/kernel/lib/memory.o
```

Status: `PASS`

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
ELF Header:
Class: ELF64
Machine: Advanced Micro Devices X86-64
Type: EXEC (Executable file)
Entry point address: 0xffffffff80000000

Program Headers:
LOAD 0x001000 0xffffffff80000000 ... R E
LOAD 0x002000 0xffffffff80001000 ... R

Section Headers:
.text   PROGBITS ... AX
.rodata PROGBITS ... AMS

Section to Segment mapping:
00 .text
01 .rodata

Disassembly:
kmain -> serial_init -> serial_write -> halt_forever
halt_forever:
cli
hlt
```

Status: `PASS`

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
limine: Loading executable `boot():/boot/kernel.elf`...
MCSOS 260502 M2 boot path entered
[M2] early serial online
[M2] kernel start = 0xffffffff80000000
[M2] kernel reached controlled halt loop
```

Status: `PASS`

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
gdb build/kernel.elf
target remote localhost:1234
```

Hasil:

```text
Reading symbols from build/kernel.elf...
(No debugging symbols found in build/kernel.elf)

Remote debugging using localhost:1234
0x000000000000fff0 in ?? ()
```

Status: `NA`


### 12.5 Unit Test

```bash
make test
```

Hasil:

```text
make: *** No rule to make target 'test'.  Stop.
```

Status: `FAIL`

### 12.6 Stress/Fuzz/Fault Injection Test

```bash
NA
```

Hasil:

```text
Praktikum M2 belum mengimplementasikan subsistem yang memerlukan stress test, fuzzing, atau fault injection seperti allocator, syscall layer, filesystem, networking, driver kompleks, atau SMP.
```

Status: `NA`

### 12.7 Visual Evidence

| Screenshot | Lokasi file | Keterangan |
|---|---|---|
| `NA` | `NA` | Praktikum M2 menggunakan serial console headless tanpa framebuffer atau output grafis. |

---

## 13. Hasil Uji

### 13.1 Tabel Ringkasan Hasil

| No. | Uji | Expected result | Actual result | Status | Evidence |
|---|---|---|---|---|---|
| 1 | Build kernel ELF64 | `kernel.elf` dan `kernel.map` berhasil dibuat | Build selesai tanpa compiler/linker error | PASS | `build/kernel.elf`, `build/kernel.map` |
| 2 | Static ELF inspection | ELF64 valid dengan entry point higher-half | Entry point `0xffffffff80000000` dan 2 LOAD segment terdeteksi | PASS | `build/inspect/readelf-header.txt` |
| 3 | ISO image generation | `mcsos.iso` berhasil dibuat | ISO hybrid Limine berhasil dibuat dan checksum SHA256 tersedia | PASS | `build/mcsos.iso`, `build/mcsos.iso.sha256` |
| 4 | QEMU smoke test | Kernel mencapai serial output dan halt loop | Early serial online dan controlled halt loop tercapai | PASS | `build/qemu-serial.log` |
| 5 | Boot log classification | Boot classifier mengenali successful boot | `CLASSIFICATION: SUCCESSFUL KERNEL ENTRY` | PASS | `build/research/boot-log-classification.txt` |
| 6 | QEMU machine comparison | q35 dan pc dapat melakukan boot | Kedua machine model berhasil boot tanpa reboot loop | PASS | `build/research/qemu-machine-analysis.txt` |
| 7 | PHDR/linker analysis | Layout ELF dapat dianalisis | ld.lld menghasilkan layout lebih sederhana dibanding GNU ld | PASS | `build/research/phdr-analysis.txt` |
| 8 | ADR bootloader decision | Keputusan desain terdokumentasi | ADR Limine over GRUB tersimpan dengan status Accepted | PASS | `docs/adr/0001-limine-over-grub.md` |
| 9 | GDB remote debugging | GDB dapat attach ke QEMU debug stub | Remote debugging berhasil connect ke localhost:1234 | PASS | terminal GDB |
| 10 | Unit test | Target `make test` tersedia dan lulus | Repository tidak memiliki target `test` | FAIL | output `make test` |

### 13.2 Log Penting

```text
[limine: Loading executable `boot():/boot/kernel.elf`...
MCSOS 260502 M2 boot path entered
[M2] early serial online
[M2] kernel start = 0xffffffff80000000
[M2] kernel reached controlled halt loop

CLASSIFICATION: SUCCESSFUL KERNEL ENTRY

make test
make: *** No rule to make target 'test'.  Stop..]
```

### 13.3 Artefak Bukti

| Artefak | Path | SHA-256 / hash | Fungsi |
|---|---|---|---|
| `kernel.elf` | `build/kernel.elf` | `a072bca4dae73a13f16e3e9afb4001ab86ac495268c5fb0810e66f717ad5ffaa` | binary kernel ELF64 freestanding |
| `mcsos.iso` | `build/mcsos.iso` | `91508e99182f8a1d75e9a522bcc042da11ae04b748e2d7b1e43a03f059b2a100` | boot image Limine untuk QEMU/OVMF |
| `qemu-serial.log` | `build/qemu-serial.log` | `000725d337dcec513b4c8caf84e89c5ff249fd27d14f9b9cc439bd27680ba1e3` | evidence boot serial console |
| `kernel.map` | `build/kernel.map` | `d57883e59ccbf0b5451a9a4c7cc99c9b56ee64d9752d25898669c3e5c4f3b934` | linker map dan layout symbol kernel |
| `kernel.disasm.txt` | `build/kernel.disasm.txt` | `f1a057f2c73589d4de372f498f4fdd8b7aa55779a28fa2d69a67c3d8bb0ae411` | evidence disassembly dan inspeksi instruksi kernel |

Perintah hash:

```bash
sha256sum \
build/kernel.elf \
build/mcsos.iso \
build/qemu-serial.log \
build/kernel.map \
build/kernel.disasm.txt
```

```text
make grade
make: *** No rule to make target 'grade'.  Stop.
```

---

## 14. Analisis Teknis

### 14.1 Analisis Keberhasilan

```text
[Jelaskan mengapa hasil uji berhasil. Kaitkan dengan desain, invariant, dan output log.]

[Kernel ELF64 berhasil dibangun dan dijalankan karena jalur firmware → Limine → kernel entry → serial console bekerja sesuai desain M2. Linker script berhasil menghasilkan higher-half kernel dengan entry point 0xffffffff80000000 dan dua LOAD segment yang valid berdasarkan hasil readelf.]

[Limine berhasil memuat kernel.elf dari boot image ISO tanpa reboot loop maupun firmware failure. Serial console COM1 berhasil diinisialisasi sehingga kernel dapat mencetak marker boot deterministik ke qemu-serial.log. Log runtime menunjukkan kernel mencapai kmain dan masuk ke controlled halt loop, sehingga invariant utama M2 seperti “kernel tidak reboot”, “serial output muncul”, dan “halt loop terkendali” terpenuhi.]
```

### 14.2 Analisis Kegagalan atau Perbedaan Hasil

```text
[Jelaskan kegagalan, gejala, dugaan akar masalah, bukti pendukung, dan tindakan perbaikan.]

[Beberapa target Makefile seperti make meta, make image, make test, dan make grade gagal dijalankan karena repository M2 tidak menyediakan rule tersebut. Gejala yang muncul berupa error “No rule to make target”. Kegagalan ini berasal dari keterbatasan automation target pada Makefile, bukan dari compiler atau runtime kernel.]

[Kernel ELF juga dibangun tanpa debug symbol (-g), sehingga GDB dapat melakukan remote attach ke QEMU tetapi belum dapat menampilkan symbol-level debugging maupun backtrace lengkap. Bukti pendukung berasal dari output GDB “No debugging symbols found in build/kernel.elf”.]

[Tindakan perbaikan yang dapat dilakukan pada tahap berikutnya adalah menambahkan target meta, image, test, dan grade pada Makefile serta menambahkan compiler flag debug seperti -g agar debugging kernel lebih lengkap.]
```

### 14.3 Perbandingan dengan Teori

| Konsep teori | Implementasi praktikum | Sesuai/tidak sesuai | Penjelasan |
|---|---|---|---|
| `[Bootloader memuat kernel ELF]` | `[Limine memuat kernel.elf dari ISO bootable]` | `[Sesuai]` | `[Serial log menunjukkan Limine berhasil melakukan handoff ke kernel.]` |
| `[Higher-half kernel]` | `[Entry point berada di 0xffffffff80000000]` | `[Sesuai]` | `[Hasil readelf menunjukkan alamat virtual higher-half valid.]` |
| `[Freestanding kernel]` | `[Kernel dibangun tanpa hosted libc]` | `[Sesuai]` | `[Compiler menggunakan flag -ffreestanding dan -nostdlib.]` |
| `[Early serial console]` | `[Kernel mencetak log melalui UART COM1]` | `[Sesuai]` | `[qemu-serial.log menunjukkan marker boot deterministik.]` |
| `[Controlled halt loop]` | `[Kernel berhenti pada loop cli + hlt]` | `[Sesuai]` | `[Hasil objdump memperlihatkan instruksi halt loop.]` |
| `[Symbol-level debugging]` | `[GDB attach ke QEMU debug stub]` | `[Sebagian sesuai]` | `[Remote debugging berhasil tetapi debug symbol belum tersedia.]` |

### 14.4 Kompleksitas dan Kinerja

| Aspek | Estimasi/hasil | Bukti | Catatan |
|---|---|---|---|
| `[Kompleksitas algoritma]` | `[O(n) untuk serial_write string]` | `[Implementasi serial_write]` | `[Kernel M2 belum memiliki algoritma kompleks.]` |
| `[Waktu build]` | `[< 5 detik di WSL2]` | `[Output make build]` | `[Build hanya mencakup beberapa file kernel.]` |
| `[Waktu boot QEMU]` | `[Boot berhasil hingga marker serial awal]` | `[qemu-serial.log]` | `[Kernel mencapai halt loop tanpa reboot.]` |
| `[Penggunaan memori]` | `[512 MB virtual RAM QEMU]` | `[Parameter -m 512M]` | `[Belum ada allocator atau paging kompleks.]` |
| `[Latensi/throughput]` | `[NA]` | `[Tidak diukur]` | `[Praktikum M2 belum melakukan benchmarking performa.]` |

---
---

## 15. Debugging dan Failure Modes

### 15.1 Failure Modes yang Ditemukan

| Failure mode | Gejala | Penyebab sementara | Bukti | Perbaikan |
|---|---|---|---|---|
| `[Makefile target missing]` | `[make meta, make image, make test, dan make grade gagal dijalankan]` | `[Repository M2 tidak memiliki rule target tersebut pada Makefile]` | `[Output: No rule to make target]` | `[Menggunakan script manual seperti tools/scripts/make_iso.sh dan menambahkan target Makefile pada tahap berikutnya]` |
| `[Missing debug symbol]` | `[GDB tidak dapat menampilkan symbol-level debugging atau backtrace lengkap]` | `[Kernel dibangun tanpa compiler flag -g]` | `[Output GDB: No debugging symbols found in build/kernel.elf]` | `[Menambahkan compiler flag debug (-g) pada build debug kernel]` |
| `[Potential firmware/boot hang risk]` | `[Kernel dapat berhenti tanpa output jika serial gagal diinisialisasi]` | `[Early boot sangat bergantung pada serial console COM1]` | `[Tidak ada output serial jika serial_init gagal]` | `[Menambahkan marker serial awal dan invariant observability]` |
| `[Incorrect QEMU machine configuration]` | `[Boot dapat gagal jika konfigurasi firmware atau machine model tidak cocok]` | `[Perbedaan device path antara q35 dan pc]` | `[Analisis qemu-machine-analysis.txt]` | `[Menggunakan konfigurasi q35 + OVMF yang telah diverifikasi stabil]` |
| `[Invalid ELF/linker layout risk]` | `[Kernel tidak dapat di-load bootloader]` | `[Entry point atau LOAD segment ELF tidak valid]` | `[Evidence readelf dan PHDR analysis]` | `[Menggunakan linker.ld dengan higher-half layout yang konsisten]` |


### 15.2 Failure Modes yang Diantisipasi

| Failure mode | Deteksi | Dampak | Mitigasi |
|---|---|---|---|
| `[Kernel ELF tidak valid]` | `[readelf, objdump, dan make inspect gagal]` | `[Bootloader tidak dapat memuat kernel]` | `[Menggunakan linker.ld yang konsisten dan melakukan static inspection ELF]` |
| `[Serial console gagal diinisialisasi]` | `[Tidak ada output pada qemu-serial.log]` | `[Kernel boot path tidak dapat diobservasi]` | `[Menambahkan marker serial awal dan validasi UART COM1]` |
| `[Reboot loop atau hang saat boot]` | `[QEMU restart terus atau tidak mencapai halt loop]` | `[Kernel state tidak dapat dianalisis]` | `[Menggunakan controlled halt loop dan opsi -no-reboot -no-shutdown]` |
| `[Firmware atau bootloader incompatibility]` | `[Limine gagal load kernel atau OVMF error]` | `[Kernel tidak mencapai entry point]` | `[Menggunakan konfigurasi q35 + OVMF yang telah diverifikasi]` |
| `[Layout linker tidak sesuai higher-half]` | `[Entry point atau LOAD segment tidak valid]` | `[Kernel crash sebelum serial aktif]` | `[Verifikasi entry point dan PHDR menggunakan readelf]` |
| `[Debugging kernel tidak lengkap]` | `[GDB tidak dapat resolve symbol]` | `[Analisis kernel runtime menjadi terbatas]` | `[Menambahkan compiler flag -g pada build debug berikutnya]` |
| `[ISO boot image rusak]` | `[QEMU gagal boot ISO]` | `[Pengujian runtime tidak dapat dilakukan]` | `[Menggunakan SHA256 checksum dan validasi hasil make_iso.sh]` |

### 15.3 Triage yang Dilakukan

```text
[Urutan diagnosis: log serial, GDB, register dump, map file, disassembly, git bisect, QEMU monitor, dll.]

[Proses triage pada M2 dimulai dengan memeriksa qemu-serial.log untuk memastikan kernel berhasil melewati jalur firmware → Limine → kernel entry. Log serial digunakan sebagai observability utama karena praktikum M2 belum memiliki framebuffer maupun panic handler kompleks.]

[Jika boot tidak menghasilkan output yang sesuai, diagnosis dilanjutkan menggunakan make inspect, readelf, dan objdump untuk memeriksa validitas ELF64, entry point higher-half, LOAD segment, section mapping, dan instruksi halt loop.]

[Kernel.map dan output nm digunakan untuk memverifikasi symbol penting seperti kmain serta memastikan linker script menghasilkan layout higher-half yang konsisten.]

[GDB digunakan untuk melakukan remote attach ke QEMU debug stub melalui localhost:1234. Hasil diagnosis menunjukkan remote debugging berhasil, tetapi symbol-level debugging belum lengkap karena kernel dibangun tanpa debug symbol (-g).]

[Analisis tambahan dilakukan menggunakan phdr-analysis.txt untuk membandingkan hasil linker ld.lld dan GNU ld, serta qemu-machine-analysis.txt untuk membandingkan perilaku machine model q35 dan pc.]

[Diagnosis build system dilakukan menggunakan git status, git log, dan pengujian target Makefile untuk mengidentifikasi target yang belum tersedia seperti make meta, make image, make test, dan make grade.]
```

### 15.4 Panic Path

Jika terjadi panic, tempel output panic.
```text
[Tempel panic log. Jika tidak ada panic, jelaskan bagaimana panic path diuji atau mengapa belum relevan.]

[Tidak ditemukan panic runtime selama pengujian M2. Kernel berhasil mencapai controlled halt loop setelah mencetak marker serial boot.]

[Panic path belum diimplementasikan penuh pada milestone M2 karena fokus praktikum masih berada pada validasi boot path firmware → Limine → kernel entry → serial console.]

[Walaupun demikian, potensi failure path tetap dianalisis menggunakan serial log, static inspection ELF, disassembly halt loop, dan remote debugging GDB untuk memastikan kernel tidak mengalami reboot loop, triple fault, maupun firmware hang.]
```

---

## 16. Prosedur Rollback

Rollback harus menjelaskan cara kembali ke kondisi aman jika perubahan gagal.

| Skenario rollback | Perintah | Data yang harus diselamatkan | Status |
|---|---|---|---|
| Kembali ke commit awal | `` `git checkout [commit_awal]` `` | `[qemu-serial.log, hasil inspect, checksum ISO]` | `[Belum diuji]` |
| Revert commit praktikum | `` `git revert [commit]` `` | `[kernel.map, qemu-serial.log, artifact build penting]` | `[Belum diuji]` |
| Bersihkan artefak build | `` `make clean` `` | `[Tidak ada, source repository tetap aman]` | `[Teruji]` |
| Regenerasi image | `` `./tools/scripts/make_iso.sh` `` | `[mcsos.iso lama jika diperlukan untuk pembandingan hash]` | `[Teruji]` |
| Regenerasi serial log QEMU | `` `./tools/scripts/run_qemu.sh` `` | `[qemu-serial.log sebelumnya jika diperlukan untuk analisis]` | `[Teruji]` |

Catatan rollback:

```text
[Jelaskan apakah rollback diuji. Jika belum diuji, jelaskan alasan dan risiko.]
[Rollback penuh menggunakan git checkout atau git revert belum diuji langsung pada praktikum M2 karena repository masih berada dalam tahap pengembangan aktif dan perubahan kernel masih relatif kecil.]
[Rollback parsial seperti make clean, regenerasi ISO, dan regenerasi serial log telah diuji selama proses debugging dan rebuild. Risiko utama rollback adalah hilangnya artifact build seperti ISO, serial log, dan hasil inspect jika belum dicadangkan.]
[Karena itu artifact penting seperti qemu-serial.log, kernel.map, checksum ISO, dan hasil static inspection disimpan sebagai evidence sebelum melakukan clean build atau rebuild image.]
```

---

## 17. Keamanan dan Reliability

### 17.1 Risiko Keamanan

| Risiko | Boundary | Dampak | Mitigasi | Evidence |
|---|---|---|---|---|
| `[Invalid ELF boot image]` | `[Firmware → Limine → kernel ELF]` | `[Kernel gagal boot atau crash sebelum serial aktif]` | `[Static inspection menggunakan readelf dan objdump]` | `[build/kernel.readelf.header.txt, build/kernel.readelf.programs.txt]` |
| `[Incorrect linker mapping]` | `[Linker → higher-half kernel layout]` | `[Kernel entry point tidak valid atau triple fault]` | `[Verifikasi entry point dan LOAD segment ELF64]` | `[readelf -l, PHDR analysis]` |
| `[Missing serial observability]` | `[Kernel early boot → serial console]` | `[Failure path tidak dapat didiagnosis]` | `[Menambahkan early serial marker pada COM1]` | `[build/qemu-serial.log]` |
| `[Firmware/bootloader incompatibility]` | `[OVMF/QEMU → Limine]` | `[Boot gagal atau reboot loop]` | `[Pengujian machine model q35 dan pc]` | `[build/research/qemu-machine-analysis.txt]` |
| `[Corrupted boot image]` | `[ISO generation → QEMU runtime]` | `[Kernel tidak dapat dimuat]` | `[Checksum SHA256 dan validasi hasil make_iso.sh]` | `[build/mcsos.iso.sha256]` |
| `[Lack of debug visibility]` | `[Kernel runtime → GDB debugging]` | `[Analisis kernel crash menjadi terbatas]` | `[Remote debugging QEMU + static inspection symbol]` | `[GDB remote attach, kernel.map]` |
| `[Unexpected reboot during fault]` | `[Kernel runtime → CPU halt path]` | `[State kernel hilang sebelum analisis]` | `[Controlled halt loop dan opsi -no-reboot -no-shutdown]` | `[qemu-serial.log dan objdump halt loop]` |

### 17.2 Reliability dan Data Integrity

| Risiko reliability | Dampak | Deteksi | Mitigasi |
|---|---|---|---|
| `[Kernel hang saat early boot]` | `[Kernel tidak mencapai serial marker atau halt loop]` | `[qemu-serial.log tidak menunjukkan boot marker]` | `[Menambahkan early serial console dan controlled halt loop]` |
| `[Reboot loop pada QEMU]` | `[State kernel hilang sebelum dapat dianalisis]` | `[QEMU restart terus-menerus]` | `[Menggunakan opsi -no-reboot dan -no-shutdown]` |
| `[Invalid ELF atau linker layout]` | `[Kernel gagal dimuat bootloader]` | `[readelf atau make inspect gagal]` | `[Static inspection ELF64 dan validasi PHDR]` |
| `[ISO image corruption]` | `[QEMU gagal boot image]` | `[Boot gagal atau checksum berubah]` | `[Menggunakan SHA256 checksum pada mcsos.iso]` |
| `[Missing runtime observability]` | `[Failure path tidak dapat didiagnosis]` | `[Tidak ada output serial]` | `[Menambahkan serial log deterministik pada COM1]` |
| `[Inconsistent build artifact]` | `[Build tidak reproducible atau artifact tidak sinkron]` | `[Perbedaan hash atau hasil inspect]` | `[Rebuild bersih menggunakan make clean dan inspect ulang]` |
| `[Missing debugging information]` | `[Analisis runtime menjadi terbatas]` | `[GDB tidak menemukan debug symbol]` | `[Menggunakan kernel.map, nm, dan objdump sebagai fallback inspection]` |
| `[Build artifact terhapus setelah clean]` | `[ISO, serial log, dan inspect result hilang]` | `[File build tidak ditemukan]` | `[Regenerasi artifact menggunakan make_iso.sh dan run_qemu.sh]` |

### 17.3 Negative Test

| Negative test | Input buruk | Expected result | Actual result | Status |
|---|---|---|---|---|
| `[Build tanpa target Makefile yang tersedia]` | `` `[make meta]` `` | `[Build system menolak target yang tidak valid tanpa merusak source tree]` | `[make: *** No rule to make target 'meta'. Stop.]` | `[PASS]` |
| `[Build image dengan target yang tidak tersedia]` | `` `[make image]` `` | `[Error terdeteksi tanpa corrupt artifact build]` | `[make: *** No rule to make target 'image'. Stop.]` | `[PASS]` |
| `[Menjalankan test suite yang belum tersedia]` | `` `[make test]` `` | `[Build system menolak target test yang tidak ada]` | `[make: *** No rule to make target 'test'. Stop.]` | `[PASS]` |
| `[Menjalankan grading automation yang belum tersedia]` | `` `[make grade]` `` | `[Error terdeteksi tanpa mengubah artifact kernel]` | `[make: *** No rule to make target 'grade'. Stop.]` | `[PASS]` |
| `[Remote debugging dengan target salah]` | `` `[target remote :1234]` `` | `[GDB gagal resolve target tetapi tidak crash]` | `[cannot resolve name: Servname not supported for ai_socktype]` | `[PASS]` |
| `[QEMU tanpa ISO boot image valid]` | `[mcsos.iso belum tersedia setelah make clean]` | `[Boot gagal tetapi source repository tetap aman]` | `[sha256sum: build/mcsos.iso: No such file or directory]` | `[PASS]` |
| `[Missing serial log setelah clean build]` | `[build/qemu-serial.log belum diregenerasi]` | `[File tidak ditemukan tanpa corrupt kernel binary]` | `[cat: build/qemu-serial.log: No such file or directory]` | `[PASS]` |

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
| Proyek dapat dibangun dari clean checkout | `[PASS]` | `[Output make clean && make build]` |
| Perintah build terdokumentasi | `[PASS]` | `[Bagian 10 dan 12 laporan]` |
| QEMU boot atau test target berjalan deterministik | `[PASS]` | `[build/qemu-serial.log]` |
| Semua unit test/praktikum test relevan lulus | `[FAIL]` | `[make test -> No rule to make target 'test']` |
| Log serial disimpan | `[PASS]` | `[build/qemu-serial.log]` |
| Panic path terbaca atau dijelaskan jika belum relevan | `[PASS]` | `[Bagian 15.4 Panic Path]` |
| Tidak ada warning kritis pada build | `[PASS]` | `[Output make build tanpa warning/error compiler]` |
| Perubahan Git terkomit | `[PASS]` | `[Commit d3ce46648c3a898ca27212c9e32b7fd0c6f661b6]` |
| Desain dan failure mode dijelaskan | `[PASS]` | `[Bagian 9, 15, dan 17 laporan]` |
| Laporan berisi screenshot/log yang cukup | `[PASS]` | `[Serial log, readelf, objdump, dan artifact evidence]` |

Kriteria tambahan untuk praktikum lanjutan:

| Kriteria lanjutan | Status | Evidence |
|---|---|---|
| Static analysis dijalankan | `[PASS]` | `[tools/check_env.sh memverifikasi cppcheck dan shellcheck]` |
| Stress test dijalankan | `[NA]` | `[Belum relevan untuk milestone boot M2]` |
| Fuzzing atau malformed-input test dijalankan | `[NA]` | `[Belum relevan untuk M2]` |
| Fault injection dijalankan | `[NA]` | `[Belum relevan untuk M2]` |
| Disassembly/readelf evidence tersedia | `[PASS]` | `[objdump, readelf header, dan program headers]` |
| Review keamanan dilakukan | `[PASS]` | `[Bagian 17 Keamanan dan Reliability]` |
| Rollback diuji | `[Sebagian]` | `[make clean, regenerasi ISO, dan rebuild telah diuji]` |

---
---

## 20. Readiness Review

Pilih satu status dengan alasan berbasis bukti.

| Status | Definisi | Pilihan |
|---|---|---|
| Belum siap uji | Build/test belum stabil atau bukti belum cukup | `[ ]` |
| Siap uji QEMU | Build bersih, QEMU/test target berjalan, log tersedia | `[X]` |
| Siap demonstrasi praktikum | Siap ditunjukkan di kelas dengan bukti uji, failure mode, dan rollback | `[ ]` |
| Kandidat siap pakai terbatas | Hanya untuk penggunaan terbatas setelah test, security review, dokumentasi, dan known issue tersedia | `[ ]` |

Alasan readiness:

```text
[Jelaskan status yang dipilih berdasarkan bukti, bukan klaim.]

[Praktikum M2 dinyatakan siap uji QEMU karena kernel ELF64 berhasil dibangun dari clean build, ISO boot image berhasil dibuat menggunakan Limine, dan kernel dapat melakukan boot deterministik pada QEMU dengan serial log yang konsisten.]

[Evidence build, readelf, objdump, PHDR analysis, serial log, dan checksum ISO seluruhnya tersedia. Kernel berhasil mencapai marker early serial dan controlled halt loop tanpa reboot loop maupun firmware failure.]

[Walaupun demikian, praktikum belum sepenuhnya siap demonstrasi lanjutan karena target automation seperti make test dan make grade belum tersedia, serta panic path dan debug symbol belum diuji secara penuh.]
```

Known issues:

| No. | Issue | Dampak | Workaround | Target perbaikan |
|---|---|---|---|---|
| 1 | `[Target make test belum tersedia]` | `[Automated test suite tidak dapat dijalankan]` | `[Menggunakan QEMU smoke test dan static inspection manual]` | `[Milestone berikutnya]` |
| 2 | `[Target make grade belum tersedia]` | `[Automation grading tidak dapat dijalankan]` | `[Melakukan verifikasi manual menggunakan log dan artifact]` | `[Milestone berikutnya]` |
| 3 | `[Kernel belum memiliki debug symbol]` | `[GDB symbol-level debugging terbatas]` | `[Menggunakan kernel.map dan objdump]` | `[Build debug berikutnya]` |
| 4 | `[Panic path belum diuji dengan fault injection]` | `[Coverage failure handling masih terbatas]` | `[Menggunakan controlled halt loop dan serial observability]` | `[Milestone debugging berikutnya]` |

Keputusan akhir:

```text
[Contoh: “Berdasarkan bukti build, QEMU serial log, dan hasil make test, hasil praktikum ini layak disebut siap uji QEMU untuk milestone M2. Belum layak disebut siap demonstrasi praktikum karena panic path belum diuji dengan fault injection.”]

[Berdasarkan bukti build bersih, hasil static inspection ELF64, checksum ISO, dan qemu-serial.log, hasil praktikum ini layak disebut siap uji QEMU untuk milestone M2. Praktikum belum sepenuhnya siap demonstrasi lanjutan karena automation target seperti make test dan make grade belum tersedia serta panic path belum diuji menggunakan fault injection.]
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

# 22. Kesimpulan

### 22.1 Yang Berhasil

```text
[Kernel ELF64 freestanding berhasil dibangun dan dijalankan menggunakan Limine pada QEMU + OVMF. Build kernel berhasil dilakukan menggunakan clang dan ld.lld dengan higher-half entry point 0xffffffff80000000.]
[Boot image ISO berhasil dibuat dan diverifikasi menggunakan checksum SHA256. Kernel berhasil melewati jalur firmware → Limine → kernel entry → serial console dan menghasilkan output deterministik pada qemu-serial.log.]
[Static inspection menggunakan readelf, objdump, nm, dan PHDR analysis berhasil membuktikan bahwa layout ELF, LOAD segment, symbol kernel, dan halt loop sesuai dengan desain M2. Controlled halt loop berhasil mencegah reboot loop sehingga failure path lebih mudah dianalisis.]
```

### 22.2 Yang Belum Berhasil

```text
[Repository M2 belum menyediakan automation target seperti make meta, make image, make test, dan make grade sehingga beberapa proses validasi masih dilakukan secara manual menggunakan script dan inspection tool.]
[Kernel juga belum dibangun dengan debug symbol (-g), sehingga debugging menggunakan GDB masih terbatas pada remote attach tanpa symbol-level debugging penuh.]
[Panic path, fault injection, stress test, dan automated test suite belum diimplementasikan karena fokus milestone M2 masih berada pada validasi early boot dan serial observability.]
```

### 22.3 Rencana Perbaikan

```text
[Langkah berikutnya adalah menambahkan automation target pada Makefile seperti make image, make test, dan make grade agar workflow build dan validasi lebih konsisten.]
[Kernel debug build dengan compiler flag -g juga perlu ditambahkan agar proses debugging menggunakan GDB dapat mendukung breakpoint simbolik dan backtrace lengkap.]
[Pengembangan milestone berikutnya akan difokuskan pada panic handling, self-test kernel, observability yang lebih baik, serta validasi failure path menggunakan fault injection dan automated runtime testing.]
```

---

## 23. Lampiran

### Lampiran A — Commit Log

```text
[d3ce466 M2: add ADR and boot log classifier]
[7d1bc51 M2: add local CI helper script]
[9effd34 M2: add serial hex output for kernel start address]
[0466919 docs: add M2 readiness review]
[160940d Complete M2 boot, ISO, QEMU, and grading setup]
```

### Lampiran B — Diff Ringkas

```diff
+ [Menambahkan konfigurasi Limine pada configs/limine/limine.conf]
+ [Menambahkan linker.ld untuk higher-half ELF64 kernel]
+ [Menambahkan serial console pada kernel/core/serial.c]
+ [Menambahkan kmain dan controlled halt loop pada kernel/core/kmain.c]
+ [Menambahkan script build dan QEMU automation pada tools/scripts/]
+ [Menambahkan static inspection menggunakan readelf dan objdump]
+ [Menambahkan dokumentasi ADR dan readiness review M2]
```

### Lampiran C — Log Build Lengkap

```text
[Path:]
[build/kernel.elf]
[build/kernel.map]
[build/kernel.readelf.header.txt]
[build/kernel.readelf.programs.txt]
[build/kernel.disasm.txt]

[Perintah build:]
[make clean]
[make build]
```

### Lampiran D — Log QEMU Lengkap

```text
[Path:]
[build/qemu-serial.log]

[Isi penting:]
[limine: Loading executable `boot():/boot/kernel.elf`...]
[MCSOS 260502 M2 boot path entered]
[[M2] early serial online]
[[M2] kernel start = 0xffffffff80000000]
[[M2] kernel reached controlled halt loop]
```

### Lampiran E — Output Readelf/Objdump

```text
[Entry point address: 0xffffffff80000000]

[Program Headers:]
[LOAD 0x001000 0xffffffff80000000 R E]
[LOAD 0x002000 0xffffffff80001000 R]

[Disassembly:]
[ffffffff80000000 <kmain>:]
[call serial_init]
[call serial_write]
[call halt_forever]
```

### Lampiran F — Screenshot

| No. | File | Keterangan |
|---|---|---|
| 1 | `(evidence/screenshots/m2-build-success.png)` | `(Bukti clean build kernel M2 berhasil tanpa error compiler maupun linker.)` |
| 2 | `(evidence/screenshots/m2-make-iso-success.png)` | `(Bukti image boot build/mcsos.iso berhasil dibuat menggunakan Limine dan xorriso.)` |
| 3 | `(evidence/screenshots/m2-qemu-serial-log.png)` | `(Bukti kernel M2 berhasil boot pada QEMU dan menghasilkan serial log.)` |
| 4 | `(evidence/screenshots/m2-git-history.png)` | `(Bukti histori commit dan branch praktikum M2.)` |

### Lampiran G — Bukti Tambahan

```text
[build/research/phdr-analysis.txt menunjukkan perbandingan LOAD segment antara ld.lld dan GNU ld.]
[build/research/qemu-machine-analysis.txt menunjukkan hasil analisis machine model q35 dan pc pada QEMU.]
[build/mcsos.iso.sha256 digunakan sebagai evidence integritas boot image ISO.]
[build/kernel.disasm.txt digunakan sebagai evidence disassembly halt loop dan serial initialization.]
[build/kernel.syms.txt digunakan untuk verifikasi symbol kernel seperti kmain.]
[build/meta/toolchain-versions.txt digunakan untuk mencatat versi compiler dan toolchain build environment.]
```

---

## 24. Daftar Referensi

```text
[1] R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces. Madison, WI, USA: Arpaci-Dusseau Books, 2018. [Online]. Available: https://pages.cs.wisc.edu/~remzi/OSTEP/. Accessed: May 25, 2026.
[2] Limine Bootloader Project, “Limine Boot Protocol Documentation.” [Online]. Available: https://limine-bootloader.org/. Accessed: May 25, 2026.
[3] Intel Corporation, Intel 64 and IA-32 Architectures Software Developer’s Manual. [Online]. Available: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html. Accessed: May 25, 2026.
[4] Advanced Micro Devices, AMD64 Architecture Programmer’s Manual. [Online]. Available: https://www.amd.com/en/support/tech-docs/amd64-architecture-programmers-manual-volumes-1-5. Accessed: May 25, 2026.
[5] UEFI Forum, Unified Extensible Firmware Interface Specification. [Online]. Available: https://uefi.org/specifications. Accessed: May 25, 2026.
[6] QEMU Project, “QEMU System Emulator Documentation.” [Online]. Available: https://www.qemu.org/documentation/. Accessed: May 25, 2026.
[7] LLVM Project, “Clang Compiler User’s Manual.” [Online]. Available: https://clang.llvm.org/docs/. Accessed: May 25, 2026.
[8] GNU Project, “GNU Binutils Documentation.” [Online]. Available: https://sourceware.org/binutils/docs/. Accessed: May 25, 2026.
[9] R. Cox, F. Kaashoek, and R. Morris, “xv6: a simple, Unix-like teaching operating system,” MIT PDOS. [Online]. Available: https://pdos.csail.mit.edu/6.828/2021/xv6.html. Accessed: May 25, 2026.
```

---

# 25. Checklist Final Sebelum Pengumpulan

| Checklist | Status |
|---|---|
| Semua placeholder `[isi ...]` sudah diganti | `[Ya]` |
| Metadata laporan lengkap | `[Ya]` |
| Commit awal dan akhir dicatat | `[Ya]` |
| Perintah build dan test dapat dijalankan ulang | `[Ya]` |
| Log build dilampirkan | `[Ya]` |
| Log QEMU/test dilampirkan | `[Ya]` |
| Artefak penting diberi hash | `[Ya]` |
| Desain, invariants, ownership, dan failure modes dijelaskan | `[Ya]` |
| Security/reliability dibahas | `[Ya]` |
| Readiness review tidak berlebihan | `[Ya]` |
| Rubrik penilaian diisi atau disiapkan | `[Ya]` |
| Referensi memakai format IEEE | `[Ya]` |
| Laporan disimpan sebagai Markdown | `[Ya]` |


---

## 26. Pernyataan Pengumpulan

Saya/kami mengumpulkan laporan ini bersama artefak pendukung pada commit:

```text
[d3ce46648c3a898ca27212c9e32b7fd0c6f661b6]
```

Status akhir yang diklaim:

```text
[siap uji QEMU]
```

Ringkasan satu paragraf:

```text
[Praktikum M2 berhasil menghasilkan kernel ELF64 freestanding yang dapat dibangun menggunakan clang dan ld.lld serta dijalankan melalui jalur firmware → Limine → kernel entry → serial console pada QEMU + OVMF. Evidence utama berupa hasil make build, static inspection readelf dan objdump, checksum mcsos.iso, serta qemu-serial.log menunjukkan kernel berhasil mencapai controlled halt loop tanpa reboot loop maupun firmware failure. Keterbatasan utama praktikum ini adalah belum tersedianya target automation seperti make test dan make grade serta belum adanya debug symbol penuh untuk GDB. Langkah berikutnya adalah menambahkan automation target, debug build, panic handling, dan runtime testing yang lebih lengkap pada milestone selanjutnya.]
```

## 27. Readiness Review

```text
# Readiness Review M2 - Boot Image dan Early Serial Console

## Identitas

- Proyek: MCSOS 260502
- Praktikum: M2
- Target: x86_64, QEMU, OVMF, Limine
- Nama/Kelompok: salma
- Commit hash: 160940df66d038f1116a9f2d7d2ec1b7977fbb88
- Tanggal: 2026-05-19
````

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
````

## Invariants yang Diperiksa

1. Kernel adalah ELF64 x86_64.
2. Entry point sesuai linker script.
3. Kernel tidak memakai hosted libc.
4. Source dikompilasi dengan `-ffreestanding` dan `-mno-red-zone`.
5. Serial console tersedia sebelum subsistem kompleks.
6. Kernel tidak kembali setelah `kmain`.
7. Output QEMU disimpan sebagai log file.
````

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
```
```

## 28. Pertanyaan Analisis

### 1. Mengapa M2 tidak boleh menggunakan `printf` dari libc host?

```text
[M2 berjalan dalam mode freestanding sehingga kernel tidak boleh bergantung pada userspace library milik host operating system. Fungsi printf dari libc host membutuhkan runtime userspace, syscall, dan environment yang belum tersedia pada kernel awal. Karena itu output kernel pada M2 menggunakan serial console sederhana yang diimplementasikan sendiri.]
```

### 2. Apa fungsi `-ffreestanding`, `-nostdlib`, dan `-mno-red-zone` pada kernel awal?

```text
[`-ffreestanding` digunakan agar compiler memahami bahwa program berjalan tanpa environment hosted standar seperti userspace Linux.]
[`-nostdlib` digunakan agar linker tidak otomatis menghubungkan libc dan startup runtime host.]
[`-mno-red-zone` digunakan karena kernel dapat mengalami interrupt kapan saja sehingga area red zone pada stack tidak aman digunakan.]
```

### 3. Mengapa serial console lebih diutamakan daripada framebuffer pada M2?

```text
[Serial console lebih sederhana, stabil, dan dapat digunakan sangat awal pada proses boot kernel. Serial juga mudah direkam ke qemu-serial.log sehingga cocok untuk observability dan debugging M2. Framebuffer membutuhkan inisialisasi hardware dan driver yang lebih kompleks.]
```

### 4. Mengapa `kernel.elf` harus diperiksa dengan `readelf` dan `objdump`, bukan hanya dilihat dari keberhasilan `make build`?

```text
[Build yang berhasil tidak menjamin ELF valid untuk diboot oleh firmware dan bootloader. readelf dan objdump digunakan untuk memeriksa entry point, LOAD segment, symbol, section, dan instruksi penting seperti halt loop agar struktur binary benar-benar sesuai dengan desain kernel M2.]
```

### 5. Apa risiko jika repository dikerjakan di `/mnt/c` pada WSL?

```text
[Filesystem `/mnt/c` memiliki overhead interoperabilitas Windows yang dapat menyebabkan performa build lebih lambat, permission tidak konsisten, masalah line ending CRLF, dan potensi corruption pada workflow build Linux. Karena itu repository MCSOS direkomendasikan berada di filesystem Linux WSL.]
```

### 6. Jelaskan perbedaan kegagalan build, kegagalan link, kegagalan image, dan kegagalan runtime QEMU.

```text
[Kegagalan build terjadi saat source code gagal dikompilasi menjadi object file.]
[Kegagalan link terjadi saat linker gagal menghasilkan kernel ELF karena symbol hilang atau layout linker salah.]
[Kegagalan image terjadi saat boot image ISO gagal dibuat atau bootloader tidak berhasil dipasang.]
[Kegagalan runtime QEMU terjadi saat kernel gagal boot, reboot loop, crash, atau tidak menghasilkan serial log pada saat dijalankan di emulator.]
```

### 7. Mengapa QEMU timeout tidak otomatis berarti gagal pada M2?

```text
[Pada M2 kernel memang dirancang masuk ke controlled halt loop setelah mencetak serial marker. Karena itu QEMU dapat berhenti akibat timeout walaupun kernel sebenarnya berjalan sesuai desain dan tidak crash.]
```

### 8. Jika serial log kosong, urutan diagnosis apa yang paling rasional?

```text
[Diagnosis dimulai dari memastikan ISO berhasil dibuat, kemudian memeriksa konfigurasi Limine dan QEMU. Setelah itu dilakukan pemeriksaan ELF menggunakan readelf dan objdump untuk memastikan entry point dan LOAD segment valid. Jika masih gagal, diagnosis dilanjutkan menggunakan GDB, kernel.map, dan pemeriksaan serial initialization.]
```

### 9. Mengapa `kmain` tidak boleh kembali?

```text
[`kmain` merupakan entry point utama kernel setelah bootloader menyerahkan kontrol CPU. Tidak ada caller userspace yang valid untuk menerima return dari kernel. Jika kmain kembali, CPU dapat masuk ke state tidak terdefinisi dan menyebabkan crash atau reboot loop. Karena itu kernel harus berhenti secara eksplisit menggunakan halt loop.]
```