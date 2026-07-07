# Template Laporan Praktikum Sistem Operasi Lanjut — MCSOS

**Nama file laporan:** `laporan_praktikum_[M5]_[2583207073007].md`  
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
| Kode praktikum | `[M5]` |
| Judul praktikum | `M5 – Implementasi PIC, PIT, dan Timer Interrupt pada Kernel x86_64` |
| Jenis pengerjaan | `[Individu]` |
| Nama mahasiswa | `[Salma Rahayau]` |
| NIM | `[2583207073007]` |
| Kelas | `[PTI 1A]` |
| Nama kelompok | `[isi jika kelompok]` |
| Anggota kelompok | `[nama, NIM, peran ringkas]` |
| Tanggal praktikum | `[YYYY-MM-DD]` |
| Tanggal pengumpulan | `[YYYY-MM-DD]` |
| Repository | `[URL repo privat / path lokal]` |
| Repository | `https://github.com/amaaarhyu078-creator/mcsos-` |
| Branch | `praktikum/m5-timer-irq` |
| Commit awal | `9604535` |
| Commit akhir | `1f955e7` |
| Status readiness yang diklaim | `siap uji QEMU` |

---

## 1. Sampul

# Laporan Praktikum `[M4]`  
## `[M5 – Implementasi PIC, PIT, dan Timer Interrupt pada Kernel x86_64]`

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

Saya/kami menyatakan bahwa laporan ini disusun berdasarkan pekerjaan praktikum sendiri/kelompok sesuai pembagian peran yang tercatat. Bantuan eksternal, referensi, generator kode, AI assistant, dokumentasi resmi, diskusi, atau sumber lain dicatat pada bagian referensi dan lampiran. Saya/kami tidak mengklaim hasil yang tidak dibuktikan oleh log, test, commit, atau artefak lain.

| Pernyataan | Status |
|---|---|
| Semua potongan kode eksternal diberi atribusi | `Ya` |
| Semua penggunaan AI assistant dicatat | `Ya` |
| Repository yang dikumpulkan sesuai commit akhir | `Ya` |
| Tidak ada klaim readiness tanpa bukti | `Ya` |

Catatan penggunaan bantuan eksternal:

```text
[AI Assistant (ChatGPT) digunakan untuk membantu penjelasan konsep PIC, PIT, IRQ, IDT, debugging build, interpretasi log QEMU, analisis failure mode, serta penyusunan dokumentasi laporan praktikum. Dokumentasi resmi MCSOS, LLVM/Clang, GNU GDB, QEMU, dan referensi arsitektur x86_64 digunakan sebagai sumber pendukung.
Seluruh implementasi, build, audit ELF, pengujian QEMU, pengujian GDB, commit Git, tag Git, dan upload repository diverifikasi secara mandiri menggunakan artefak praktikum yang dihasilkan selama pengerjaan M5.]
```

---

## 3. Tujuan Praktikum

Tuliskan tujuan teknis dan konseptual praktikum. Tujuan harus dapat diuji.

1. `[Tujuan teknis 1: Menghasilkan jalur external interrupt awal yang terukur untuk MCSOS pada arsitektur x86_64.
2. `[Tujuan teknis 2: Menyediakan mekanisme remap legacy PIC ke vector `0x20..0x2F` serta membuka IRQ0 secara terkendali sebagai sumber interrupt timer awal.]`
3. `[Tujuan konseptual 1: Menghasilkan tick timer awal dari PIT channel 0 yang dapat diamati dan diverifikasi melalui serial log pada lingkungan QEMU.]`
4. `[Tujuan validasi: Mempertahankan panic path dan exception dispatcher dari M4 agar kegagalan runtime tetap dapat didiagnosis dengan jelas tanpa menghilangkan kemampuan debugging yang telah ada.]`
4. `[Menyediakan bukti implementasi melalui build log, audit ELF, audit symbol table, audit disassembly, pengujian GDB, dan QEMU smoke test sebagai dasar validasi fungsional sistem]`

---

## 4. Capaian Pembelajaran Praktikum

| CPL/CPMK praktikum | Bukti yang harus ditunjukkan |
|---|---|
| Mampu menjelaskan perbedaan exception CPU, software interrupt, dan external hardware interrupt serta hubungan ketiganya dalam mekanisme interrupt x86_64. | Analisis pada laporan, diagram alur interrupt, dan penjelasan dispatcher interrupt. |
| Mampu menjelaskan alasan remap IRQ legacy ke rentang vector `0x20..0x2F` agar tidak bertabrakan dengan exception CPU `0..31`, serta mengimplementasikan PIC master/slave menggunakan ICW1–ICW4, masking, unmasking IRQ0, dan EOI. | Source code PIC, symbol `pic_remap`, hasil audit symbol table, disassembly, dan log konfigurasi PIC pada QEMU. |
| Mampu mengimplementasikan akses port I/O freestanding menggunakan instruksi `inb` dan `outb` untuk komunikasi perangkat keras tingkat rendah pada kernel x86_64. | Source code port I/O, hasil audit disassembly yang menunjukkan instruksi `outb`, dan build log yang berhasil tanpa dependency host. |
| Mampu mengonfigurasi PIT channel 0 menggunakan command word `0x36` dan divisor yang dihitung dari frekuensi dasar `1.193.182 Hz` untuk menghasilkan interrupt timer periodik. | Source code PIT, symbol `pit_configure_hz`, log konfigurasi PIT, dan serial log timer periodik pada QEMU. |
| Mampu memperluas trap dispatcher sehingga IRQ0 diproses sebagai interrupt timer dan tidak diperlakukan sebagai fatal exception. | Source code dispatcher, symbol `x86_64_trap_dispatch`, symbol `timer_on_irq0`, dan log tick timer yang terus bertambah. |
| Mampu melakukan validasi implementasi menggunakan build reproducible, audit ELF, audit symbol, audit disassembly, pengujian QEMU, dan debugging GDB. | `make grade`, `readelf`, `nm`, `objdump`, log QEMU, screenshot runtime, dan breakpoint GDB. |
| Mampu melakukan analisis failure mode serta menyusun prosedur rollback ketika terjadi hang, interrupt storm, triple fault, atau kegagalan log serial. | Analisis failure mode pada laporan, prosedur rollback, dan dokumentasi langkah diagnosis. |

---

## 5. Peta Milestone MCSOS

Centang milestone yang menjadi fokus laporan ini. Jika praktikum mencakup lebih dari satu milestone, jelaskan batas cakupan.

| Milestone | Fokus | Status dalam laporan |
|---|---|---|
| M0 | Requirements, governance, baseline arsitektur | `[ ] tidak dibahas / [ ] dibahas / [V] selesai praktikum` |
| M1 | Toolchain reproducible, Git, QEMU, GDB, metadata build | `[ ] tidak dibahas / [ ] dibahas / [V] selesai praktikum` |
| M2 | Boot image, kernel ELF64, early console | `[ ] tidak dibahas / [ ] dibahas / [V] selesai praktikum` |
| M3 | Panic path, linker map, GDB, observability awal | `[ ] tidak dibahas / [ ] dibahas / [V] selesai praktikum` |
| M4 | Trap, exception, interrupt, timer | `[ ] tidak dibahas / [ ] dibahas / [V] selesai praktikum` |
| M5 | PMM, VMM, page table, kernel heap | `[ ] tidak dibahas / [V] dibahas / [ ] selesai praktikum` |
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
[Fitur yang termasuk dalam M5:
- Implementasi external interrupt awal pada MCSOS x86_64.
- Remap legacy PIC ke vector 0x20..0x2F.
- Konfigurasi PIT channel 0 sebagai sumber timer periodik.
- Aktivasi IRQ0 dan penanganannya melalui trap dispatcher.
- Pengiriman EOI setelah penanganan interrupt.
- Validasi menggunakan build audit, audit ELF, audit symbol, audit disassembly, QEMU, dan GDB.

Non-goals:
- Tidak mengimplementasikan scheduler preemptive final.
- Tidak mengganti legacy PIC/PIT dengan APIC, IOAPIC, HPET, atau LAPIC timer pada M5.
- Tidak mendukung SMP, user mode, syscall ABI, atau interrupt affinity.
- Tidak menyatakan sistem siap produksi atau siap hardware umum.]
```


## 6. Dasar Teori Ringkas

### 6.1 Interrupt pada Arsitektur x86_64
Interrupt merupakan mekanisme yang memungkinkan CPU menghentikan sementara alur eksekusi normal untuk menjalankan handler tertentu. Pada x86_64 terdapat tiga kategori utama, yaitu exception CPU, software interrupt, dan external hardware interrupt. Exception dihasilkan oleh CPU ketika terjadi kondisi khusus seperti divide-by-zero atau page fault, software interrupt dipicu oleh instruksi perangkat lunak seperti `int3`, sedangkan external hardware interrupt berasal dari perangkat keras melalui interrupt controller.

### 6.2 Interrupt Descriptor Table (IDT)
IDT adalah struktur data yang berisi alamat handler untuk setiap vector interrupt. Ketika interrupt atau exception terjadi, CPU menggunakan nomor vector untuk mencari entri yang sesuai pada IDT dan mengalihkan eksekusi ke handler tersebut. Pada M5, IDT diperluas hingga mencakup vector 0–47 sehingga mampu menangani exception CPU serta interrupt dari PIC yang telah diremap.

### 6.3 Programmable Interrupt Controller (PIC)
Legacy PIC terdiri dari PIC master dan PIC slave yang bertugas menerima sinyal interrupt dari perangkat keras dan meneruskannya ke CPU. Secara historis, IRQ PIC menggunakan vector yang bertabrakan dengan exception CPU sehingga perlu dilakukan remapping ke rentang `0x20–0x2F`. Pada M5, PIC diremap ke vector tersebut, seluruh IRQ dimask terlebih dahulu, kemudian hanya IRQ0 yang dibuka untuk timer.

### 6.4 Programmable Interval Timer (PIT)
PIT merupakan timer perangkat keras klasik pada arsitektur PC. Channel 0 PIT digunakan untuk menghasilkan interrupt periodik melalui IRQ0. Frekuensi timer ditentukan menggunakan divisor yang dihitung dari frekuensi dasar PIT sebesar `1.193.182 Hz`. Pada praktikum ini PIT dikonfigurasi pada frekuensi 100 Hz sehingga menghasilkan interrupt timer secara periodik.

### 6.5 Interrupt Service Routine dan Dispatcher
Interrupt Service Routine (ISR) adalah handler awal yang dijalankan ketika interrupt terjadi. ISR bertugas menyimpan konteks CPU dan meneruskan informasi interrupt ke dispatcher kernel. Dispatcher kemudian menentukan tindakan yang sesuai berdasarkan nomor vector. Pada M5, dispatcher dibedakan antara exception dan IRQ sehingga IRQ0 diproses sebagai timer interrupt dan tidak dianggap sebagai fatal exception.

### 6.6 End Of Interrupt (EOI)
Setelah interrupt selesai ditangani, kernel harus mengirimkan sinyal End Of Interrupt (EOI) ke PIC. Tanpa EOI, PIC akan menganggap interrupt masih sedang diproses sehingga interrupt berikutnya tidak akan dikirimkan ke CPU. Oleh karena itu setiap penanganan IRQ0 pada M5 diakhiri dengan pengiriman EOI.

### 6.7 Validasi Kernel Freestanding
Kernel freestanding tidak bergantung pada libc host sehingga seluruh fungsi yang diperlukan harus disediakan oleh kernel sendiri. Validasi dilakukan menggunakan build reproducible, audit ELF melalui `readelf`, audit symbol menggunakan `nm`, audit disassembly menggunakan `objdump`, serta pengujian runtime pada QEMU dan GDB untuk memastikan jalur interrupt bekerja sesuai rancangan.


### 6.1 Konsep Sistem Operasi yang Diuji

```text
[Konsep utama yang diuji pada praktikum M5 adalah mekanisme external interrupt pada sistem operasi x86_64. Praktikum berfokus pada bagaimana kernel menerima interrupt dari perangkat keras, mengalihkan kontrol eksekusi melalui Interrupt Descriptor Table (IDT), memproses interrupt menggunakan Interrupt Service Routine (ISR), dan meneruskannya ke trap dispatcher kernel.
Untuk mendukung mekanisme tersebut, digunakan legacy Programmable Interrupt Controller (PIC) sebagai interrupt controller yang diremap ke vector 0x20–0x2F agar tidak bertabrakan dengan exception CPU. Selain itu, Programmable Interval Timer (PIT) channel 0 dikonfigurasi sebagai sumber interrupt periodik melalui IRQ0 sehingga kernel memperoleh tick timer awal.
Praktikum juga menguji konsep trap frame sebagai struktur penyimpan konteks CPU saat interrupt terjadi, dispatcher interrupt untuk membedakan exception dan IRQ, serta mekanisme End Of Interrupt (EOI) yang diperlukan agar PIC dapat mengirim interrupt berikutnya.
Dari sisi rekayasa sistem, praktikum memvalidasi kernel freestanding melalui build reproducible, audit ELF, audit symbol, audit disassembly, pengujian QEMU, dan debugging menggunakan GDB untuk memastikan jalur interrupt bekerja sesuai rancangan.

```

### 6.2 Konsep Arsitektur x86_64 yang Relevan

| Konsep | Relevansi pada praktikum | Bukti/verifikasi |
|---|---|---|
| `Interrupt Descriptor Table (IDT)` | Digunakan untuk memetakan vector exception dan interrupt ke handler kernel. Pada M5 IDT diperluas hingga vector 0–47 untuk mendukung exception CPU dan IRQ dari PIC. | Serial log IDT loaded, symbol table, audit disassembly yang menunjukkan instruksi `lidt`. |
| `Interrupt dan Exception` | Menjadi dasar mekanisme perpindahan kontrol dari CPU ke handler kernel saat terjadi exception maupun external interrupt. | Trap dispatcher, log runtime QEMU, dan pengujian breakpoint GDB. |
| `Legacy PIC (Programmable Interrupt Controller)` | Digunakan untuk menerima interrupt perangkat keras dan meneruskannya ke CPU. PIC diremap ke rentang vector `0x20–0x2F` agar tidak bertabrakan dengan exception CPU. | Symbol `pic_remap`, log konfigurasi PIC, audit symbol table, dan runtime QEMU. |
| `PIT (Programmable Interval Timer)` | Digunakan sebagai sumber interrupt periodik melalui IRQ0 untuk menghasilkan tick timer awal kernel. | Symbol `pit_configure_hz`, log konfigurasi PIT, dan log tick periodik pada QEMU. |
| `Port I/O (inb/outb)` | Digunakan untuk berkomunikasi dengan PIC dan PIT melalui port I/O x86_64 tanpa menggunakan library host. | Audit disassembly yang menunjukkan instruksi `outb`, source code driver PIC dan PIT. |
| `Interrupt Service Routine (ISR)` | Menjadi entry point interrupt yang menyimpan konteks CPU dan meneruskan kontrol ke dispatcher kernel. | Symbol `isr_stub_32`, audit symbol table, dan audit disassembly. |
| `Trap Frame` | Digunakan untuk menyimpan register CPU saat interrupt atau exception terjadi sehingga konteks eksekusi dapat dipulihkan. | Struktur trap frame pada source code dan validasi melalui trap dispatcher. |
| `iretq` | Digunakan untuk mengembalikan eksekusi CPU dari handler interrupt ke konteks sebelumnya. | Audit disassembly yang menunjukkan instruksi `iretq`. |

### 6.3 Konsep Implementasi Freestanding

| Aspek | Keputusan praktikum |
|---|---|
| Bahasa | `[C17 freestanding dan assembly x86_64]` |
| Runtime | `[Tanpa hosted libc, tanpa runtime host, dan menggunakan inisialisasi kernel sendiri]` |
| ABI | `[x86_64 System V ABI untuk kernel freestanding]` |
| Compiler flags kritis | `[-ffreestanding, -fno-builtin, -fno-stack-protector, -mno-red-zone, -nostdlib, -fno-pic, -fno-pie]` |
| Risiko undefined behavior | `[Pointer tidak valid, alignment yang salah, trap frame yang tidak konsisten, kesalahan stack saat interrupt, integer overflow pada perhitungan divisor PIT, dan akses memori di luar area yang valid]` |


### 6.4 Referensi Teori yang Digunakan

| No. | Sumber | Bagian yang digunakan | Alasan relevansi |
|---|---|---|---|
| `[1]` | `[Intel® 64 and IA-32 Architectures Software Developer's Manual, Volume 3A]` | `[Chapter 6: Interrupt and Exception Handling]` | `[Menjelaskan mekanisme interrupt, exception, IDT, interrupt gate, dan instruksi iretq yang digunakan pada implementasi M5.]` |
| `[2]` | `[OSDev Wiki – Interrupt Descriptor Table (IDT)]` | `[Interrupt Descriptor Table]` | `[Digunakan untuk memahami struktur IDT, format descriptor, dan proses instalasi handler interrupt pada x86_64.]` |
| `[3]` | `[OSDev Wiki – 8259 PIC]` | `[8259 PIC]` | `[Menjelaskan proses remap PIC, masking IRQ, unmask IRQ0, dan pengiriman End Of Interrupt (EOI).]` |
| `[4]` | `[OSDev Wiki – Programmable Interval Timer]` | `[PIT Channel 0 dan Frequency Generation]` | `[Digunakan untuk memahami konfigurasi PIT, command word 0x36, divisor, dan pembangkitan timer periodik.]` |
| `[5]` | `[MCSOS Praktikum M5 Timer and External Interrupt Guide]` | `[Goals, Design Overview, Runtime Validation, Readiness Review]` | `[Menjadi acuan utama implementasi PIC, PIT, IRQ0, pengujian QEMU, audit ELF, dan kriteria kelulusan praktikum.]` |

---

## 7. Lingkungan Praktikum

### 7.1 Host dan Target

| Komponen | Nilai |
|---|---|
| Host OS | `[Windows 11 x64]` |
| Lingkungan build | `[WSL 2 Ubuntu Linux]` |
| Target ISA | `[x86_64]` |
| Target ABI | `[x86_64-unknown-none-elf]` |
| Emulator | `[QEMU versi ...]` |
| Firmware emulator | `[Tidak digunakan (boot melalui Limine BIOS/UEFI image)]` |
| Debugger | `[GNU GDB 17.1]` |
| Build system | `[GNU Make]` |
| Bahasa utama | `[C17 freestanding]` |
| Assembly | `[GNU Assembler (Clang Integrated Assembler) versi ...]` |

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
[date_utc=2026-05-29T22:33:29Z
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
| Path repository di WSL | `[~/src/mcsos]` |
| Apakah berada di filesystem Linux WSL, bukan /mnt/c | `[Ya]` |
| Remote repository | `[https://github.com/amaaarhyu078-creator/mcsos-.git]` |
| Branch | `[praktikum/m5-timer-irq]` |
| Commit hash awal | `[9604535]` |
| Commit hash akhir | `[1f955e7]` |

---

## 8. Repository dan Struktur File

### 8.1 Struktur Direktori yang Relevan

Tampilkan hanya direktori dan file yang relevan dengan praktikum.

```text
[mcsos/
├── Makefile
├── linker.ld
├── kernel/
│   ├── arch/x86_64/
│   │   ├── idt.c
│   │   ├── isr.S
│   │   ├── pic.c
│   │   ├── pit.c
│   │   └── include/
│   ├── core/
│   │   ├── kmain.c
│   │   ├── trap.c
│   │   ├── serial.c
│   │   ├── panic.c
│   │   └── log.c
│   ├── include/
│   │   ├── io.h
│   │   ├── pic.h
│   │   ├── pit.h
│   │   └── types.h
│   └── lib/
│       └── memory.c
├── configs/
│   └── limine/
│       └── limine.conf
├── iso_root/
│   ├── boot/
│   │   ├── kernel.elf
│   │   └── limine/
│   └── EFI/
├── build/
│   ├── mcsos-m5.elf
│   ├── mcsos-m5.map
│   ├── readelf-header.txt
│   ├── readelf-sections.txt
│   ├── readelf-program-headers.txt
│   ├── symbols.txt
│   ├── undefined.txt
│   └── disassembly.txt
├── evidence/
│   ├── m5-build.log
│   └── m5-qemu.log
└── tools/
    └── scripts/
]
```

### 8.2 File yang Dibuat atau Diubah

| File | Jenis perubahan | Alasan perubahan | Risiko |
|---|---|---|---|
| `[kernel/arch/x86_64/pic.c]` | `[Baru]` | `[Mengimplementasikan remap legacy PIC, masking IRQ, unmask IRQ0, dan pengiriman EOI untuk external interrupt.]` | `[Tinggi – Kesalahan konfigurasi PIC dapat menyebabkan interrupt tidak masuk, interrupt storm, atau hang sistem.]` |
| `[kernel/arch/x86_64/pit.c]` | `[Baru]` | `[Mengimplementasikan konfigurasi PIT channel 0 pada frekuensi 100 Hz sebagai sumber timer periodik.]` | `[Sedang – Kesalahan divisor atau konfigurasi PIT dapat menyebabkan timer tidak berjalan atau menghasilkan frekuensi yang salah.]` |
| `[kernel/arch/x86_64/isr.S]` | `[Ubah]` | `[Menambahkan ISR stub untuk IRQ 32–47 dan jalur masuk interrupt eksternal.]` | `[Tinggi – Kesalahan penyimpanan register atau return interrupt dapat menyebabkan kernel panic atau triple fault.]` |
| `[kernel/core/trap.c]` | `[Ubah]` | `[Memperluas dispatcher agar IRQ0 diproses sebagai timer interrupt dan tidak dianggap sebagai fatal exception.]` | `[Tinggi – Kesalahan dispatcher dapat menyebabkan interrupt diperlakukan sebagai exception fatal.]` |
| `[kernel/core/kmain.c]` | `[Ubah]` | `[Menambahkan inisialisasi PIC, PIT, aktivasi interrupt, dan validasi runtime timer.]` | `[Sedang – Kesalahan urutan inisialisasi dapat menyebabkan interrupt tidak berfungsi.]` |
| `[kernel/include/pic.h]` | `[Baru]` | `[Menyediakan deklarasi antarmuka PIC untuk digunakan oleh komponen kernel lain.]` | `[Rendah – Risiko terbatas pada ketidaksesuaian deklarasi fungsi.]` |
| `[kernel/include/pit.h]` | `[Baru]` | `[Menyediakan deklarasi antarmuka PIT dan timer interrupt.]` | `[Rendah – Risiko terbatas pada ketidaksesuaian deklarasi fungsi.]` |
| `[kernel/include/io.h]` | `[Baru/Ubah]` | `[Menyediakan abstraksi akses port I/O menggunakan instruksi inb dan outb.]` | `[Sedang – Kesalahan akses port dapat menyebabkan perangkat keras virtual tidak merespons.]` |
| `[kernel/include/types.h]` | `[Baru]` | `[Menyediakan definisi tipe data umum yang digunakan oleh modul PIC, PIT, dan interrupt.]` | `[Rendah – Risiko terbatas pada kompatibilitas tipe data.]` |

### 8.3 Ringkasan Diff

```bash
git status --short
git diff --stat
git log --oneline -n 5
```

Output:

```text
[git status --short
(tidak ada output; working tree clean)
git diff --stat
(tidak ada output; tidak ada perubahan yang belum di-commit)
git log --oneline -n 5
1f955e7 (HEAD -> praktikum/m5-timer-irq, tag: m5-enrichment-stable, origin/praktikum/m5-timer-irq) M5: add unexpected IRQ diagnostics
4a76ef9 M5: add common types header
9604535 (tag: m5-stable) M5: implement PIC, PIT, and IRQ0 timer interrupts
5d467ac M5: add IRQ 32-47 ISR stubs
82ccae6 (origin/m4-idt-exception-path, m4-idt-exception-path) M4 add enhanced trap diagnostics and fault-int3 target.]
```

---

### 9.1 Masalah yang Diselesaikan

```text
[Kernel M4 telah memiliki IDT dan exception handler dasar, namun belum mampu menerima dan memproses external hardware interrupt secara terkendali. Akibatnya, kernel belum dapat memperoleh sumber waktu periodik yang diperlukan untuk pengukuran waktu, penjadwalan, maupun aktivitas kernel yang bergantung pada timer.
Selain itu, IRQ legacy dari PIC secara historis menggunakan vector yang bertabrakan dengan exception CPU sehingga berpotensi menyebabkan ambiguitas diagnosis ketika interrupt terjadi. Kernel juga belum memiliki mekanisme konfigurasi PIC, pengiriman End Of Interrupt (EOI), maupun konfigurasi PIT sebagai sumber interrupt periodik.
Praktikum M5 menyelesaikan masalah tersebut dengan melakukan remap PIC ke rentang vector 0x20–0x2F, membuka IRQ0 secara terkendali, mengonfigurasi PIT channel 0 pada frekuensi 100 Hz, serta memperluas trap dispatcher agar mampu membedakan exception dan timer interrupt. Hasilnya, kernel dapat menghasilkan tick periodik yang dapat diamati melalui serial log dan digunakan sebagai fondasi untuk pengembangan subsistem waktu pada tahap berikutnya.]
```

### 9.2 Keputusan Desain

| Keputusan | Alternatif yang dipertimbangkan | Alasan memilih | Konsekuensi |
|---|---|---|---|
| `[Menggunakan legacy PIC yang diremap ke vector 0x20–0x2F.]` | `[Tetap menggunakan mapping bawaan PIC atau langsung menggunakan APIC/IOAPIC.]` | `[Remap menghindari konflik dengan exception CPU 0–31 dan lebih sederhana untuk tahap awal praktikum.]` | `[Kernel masih bergantung pada legacy PIC sehingga belum siap untuk sistem SMP atau interrupt controller modern.]` |
| `[Hanya membuka IRQ0 dan tetap mem-mask IRQ lainnya.]` | `[Membuka seluruh IRQ sejak awal.]` | `[Mengurangi kompleksitas debugging dan menerapkan prinsip fail-closed selama validasi jalur interrupt.]` | `[Perangkat lain belum dapat menghasilkan interrupt sampai handler yang sesuai tersedia.]` |
| `[Menggunakan PIT channel 0 dengan frekuensi 100 Hz.]` | `[Menggunakan frekuensi lain atau timer modern seperti HPET dan LAPIC timer.]` | `[100 Hz cukup untuk observasi tick periodik dan mudah diverifikasi melalui serial log.]` | `[Resolusi timer masih terbatas dan belum cocok untuk kebutuhan sistem operasi yang lebih kompleks.]` |
| `[Memisahkan penanganan exception dan IRQ pada trap dispatcher.]` | `[Menggunakan jalur penanganan yang sama untuk seluruh vector.]` | `[Mempermudah diagnosis kesalahan dan mencegah IRQ0 diperlakukan sebagai fatal exception.]` | `[Dispatcher menjadi sedikit lebih kompleks karena harus membedakan jenis interrupt.]` |
| `[Mengirim End Of Interrupt (EOI) setelah IRQ selesai diproses.]` | `[Tidak mengirim EOI atau mengirim EOI pada seluruh jalur interrupt tanpa seleksi.]` | `[PIC memerlukan EOI agar interrupt berikutnya dapat dikirim ke CPU.]` | `[Kesalahan implementasi EOI dapat menyebabkan interrupt berhenti atau interrupt storm.]` |
| `[Mempertahankan serial log sebagai mekanisme observasi utama.]` | `[Mengandalkan framebuffer atau debugging visual.]` | `[Serial log lebih sederhana, stabil, dan mudah dikumpulkan sebagai evidence otomatis.]` | `[Informasi yang ditampilkan terbatas pada output teks.]` |

### 9.3 Arsitektur Ringkas

```mermaid
flowchart TD
    A[PIT Channel 0<br/>100 Hz] --> B[IRQ0]
    B --> C[Legacy PIC]
    C --> D[Vector 32 (0x20)]
    D --> E[IDT]
    E --> F[ISR Stub 32]
    F --> G[x86_64_trap_dispatch]
    G --> H[timer_on_irq0]
    H --> I[ticks++]
    I --> J[Serial Log]
    H --> K[Send EOI]
    J --> L[QEMU Evidence]
```

Penjelasan diagram:

```text
[PIT channel 0 dikonfigurasi pada frekuensi 100 Hz dan secara periodik menghasilkan IRQ0. Interrupt tersebut diteruskan oleh legacy PIC yang telah diremap ke rentang vector 0x20–0x2F sehingga IRQ0 menggunakan vector 32 (0x20).
Ketika interrupt diterima CPU, IDT digunakan untuk menentukan handler yang sesuai. ISR stub 32 bertugas menyimpan konteks eksekusi dan meneruskan kontrol ke x86_64_trap_dispatch().
Dispatcher membedakan antara exception CPU dan external interrupt. Untuk vector 32, dispatcher memanggil timer_on_irq0() yang berfungsi memperbarui penghitung tick kernel. Nilai tick kemudian dicatat secara periodik ke serial log sehingga dapat diamati selama pengujian QEMU.
Setelah penanganan interrupt selesai, kernel mengirim End Of Interrupt (EOI) ke PIC agar interrupt berikutnya dapat diterima. Seluruh proses divalidasi melalui serial log QEMU, audit ELF, audit symbol, audit disassembly, dan debugging menggunakan GDB.]
```

### 9.3.1 Tabel Vector Interrupt

| Vector | Sumber | Handler | Keterangan |
|---|---|---|---|
| `[0–31]` | `[CPU Exception]` | `[Exception Dispatcher]` | `[Digunakan untuk fault, trap, dan exception CPU.]` |
| `[32]` | `[IRQ0 - PIT Timer]` | `[timer_on_irq0()]` | `[Menghasilkan tick periodik 100 Hz.]` |
| `[33–47]` | `[IRQ1–IRQ15 Legacy PIC]` | `[IRQ Dispatcher]` | `[Disediakan oleh ISR stub, namun tetap dimask pada baseline M5.]` |

Penjelasan:

```text
PIC legacy diremap ke rentang vector 32–47 (0x20–0x2F) untuk menghindari konflik dengan exception CPU pada vector 0–31. Pada M5 hanya IRQ0 yang dibuka dan digunakan sebagai sumber timer interrupt melalui PIT channel 0. IRQ lainnya tetap dimask sebagai bagian dari kebijakan fail-closed hingga driver dan handler yang sesuai tersedia.
```

### 9.4 Kontrak Antarmuka

| Antarmuka | Pemanggil | Penerima | Precondition | Postcondition | Error path |
|---|---|---|---|---|---|
| `[idt_init()]` | `[kmain()]` | `[Subsystem IDT]` | `[Struktur IDT telah dialokasikan dan descriptor dapat diisi.]` | `[IDT terpasang melalui instruksi lidt dan siap menerima interrupt.]` | `[Interrupt tidak dapat diproses sehingga kernel masuk panic atau hang.]` |
| `[pic_remap()]` | `[kmain()]` | `[Driver PIC]` | `[Port I/O dapat diakses dan interrupt belum diaktifkan.]` | `[PIC master/slave diremap ke vector 0x20–0x2F.]` | `[IRQ dapat bertabrakan dengan exception CPU dan menyebabkan diagnosis yang ambigu.]` |
| `[pit_configure_hz(100)]` | `[kmain()]` | `[Driver PIT]` | `[PIC telah dikonfigurasi dan akses port I/O tersedia.]` | `[PIT channel 0 menghasilkan interrupt periodik 100 Hz.]` | `[Timer tidak menghasilkan interrupt atau frekuensi menjadi tidak sesuai.]` |
| `[x86_64_trap_dispatch()]` | `[ISR Stub]` | `[Trap Dispatcher]` | `[Trap frame telah disiapkan oleh ISR dan vector interrupt valid.]` | `[Exception atau IRQ diteruskan ke handler yang sesuai.]` | `[Interrupt dapat diperlakukan sebagai exception fatal atau menyebabkan panic.]` |
| `[timer_on_irq0()]` | `[x86_64_trap_dispatch()]` | `[Subsystem Timer]` | `[Interrupt berasal dari vector 32 (IRQ0 timer).]` | `[Counter tick bertambah dan log periodik dapat dihasilkan.]` | `[Tick tidak bertambah sehingga sumber waktu kernel tidak berjalan.]` |
| `[pic_send_eoi()]` | `[timer_on_irq0()]` | `[Driver PIC]` | `[IRQ telah selesai diproses.]` | `[PIC siap menerima interrupt berikutnya.]` | `[Interrupt berikutnya tidak dikirim atau terjadi interrupt stall.]` |

### 9.5 Struktur Data Utama

| Struktur data | Field penting | Ownership | Lifetime | Invariant |
|---|---|---|---|---|
| `[struct idt_entry]` | `[offset_low, selector, ist, type_attr, offset_mid, offset_high]` | `[Subsystem IDT]` | `[Dibuat saat inisialisasi kernel dan tetap ada selama kernel berjalan]` | `[Setiap entri harus menunjuk ke ISR yang valid dan menggunakan descriptor interrupt gate yang benar]` |
| `[struct idt_ptr]` | `[limit, base]` | `[Subsystem IDT]` | `[Dibuat saat inisialisasi IDT dan digunakan oleh instruksi lidt]` | `[Base harus menunjuk ke tabel IDT yang valid dan limit harus sesuai ukuran IDT]` |
| `[struct trap_frame]` | `[vector, error_code, rip, cs, rflags, rsp, ss]` | `[Trap Dispatcher]` | `[Dibuat saat interrupt/exception masuk dan digunakan selama proses dispatch]` | `[Isi trap frame harus konsisten dengan konteks CPU yang disimpan oleh ISR]` |
| `[counter ticks]` | `[nilai tick timer]` | `[Subsystem Timer]` | `[Diinisialisasi saat boot dan bertambah selama kernel berjalan]` | `[Nilai tick hanya bertambah akibat IRQ0 dan tidak boleh berkurang]` |

### 9.6 Invariants

Tuliskan invariant yang harus benar sepanjang eksekusi.

1. `[IDT harus tetap valid selama kernel berjalan, dan setiap vector 0–47 harus menunjuk ke handler yang sah.]`
2. `[Interrupt handler tidak boleh melakukan operasi blocking, alokasi memori kompleks, atau aktivitas yang dapat menyebabkan deadlock.]`
3. `[IRQ0 harus selalu menggunakan vector 32 (0x20) hasil remap PIC dan tidak boleh bertabrakan dengan exception CPU 0–31.]`
4. `[Setiap IRQ yang berhasil diproses harus mengirim End Of Interrupt (EOI) ke PIC, dan counter tick hanya boleh bertambah akibat interrupt timer IRQ0.]`

### 9.7 Ownership, Locking, dan Concurrency

| Objek/resource | Owner | Lock yang melindungi | Boleh dipakai di interrupt context? | Catatan |
|---|---|---|---|---|
| `[IDT]` | `[Subsystem IDT]` | `[None]` | `[Ya]` | `[Diinisialisasi saat boot dan tidak diubah selama runtime M5.]` |
| `[PIC]` | `[Driver PIC]` | `[None]` | `[Ya]` | `[Diakses melalui port I/O untuk masking, unmasking, dan pengiriman EOI.]` |
| `[PIT]` | `[Driver PIT]` | `[None]` | `[Ya]` | `[Dikonfigurasi saat boot dan kemudian menghasilkan IRQ0 secara periodik.]` |
| `[Trap Frame]` | `[Trap Dispatcher]` | `[None]` | `[Ya]` | `[Bersifat sementara dan hanya digunakan selama penanganan interrupt.]` |
| `[Tick Counter]` | `[Subsystem Timer]` | `[None]` | `[Ya]` | `[Dimodifikasi oleh handler IRQ0 pada lingkungan single-core.]` |
| `[Serial Console]` | `[Subsystem Logging]` | `[None]` | `[Ya]` | `[Digunakan untuk menampilkan informasi tick dan diagnosis runtime.]` |

Lock order yang berlaku:

```text
[Tidak ada lock order pada M5 karena sistem masih berjalan dalam konfigurasi single-core dan belum memiliki scheduler preemptive maupun SMP. Konsistensi dijaga dengan urutan inisialisasi yang benar (IDT → PIC → PIT → STI), penggunaan interrupt handler yang singkat, serta kebijakan bahwa hanya IRQ0 yang dibuka sementara IRQ lain tetap dimask. Oleh karena itu, interrupt-disabled dan desain single-core dianggap cukup untuk tahap ini.]
```

### 9.8 Memory Safety dan Undefined Behavior Risk

| Risiko | Lokasi | Mitigasi | Bukti |
|---|---|---|---|
| `[Out-of-bounds access pada IDT]` | `[kernel/arch/x86_64/idt.c]` | `[IDT hanya diisi untuk vector yang telah ditentukan dan ukuran tabel dihitung melalui idt_limit.]` | `[Audit runtime menunjukkan IDT berhasil dimuat dan selftest IDT lulus.]` |
| `[Stack corruption akibat interrupt entry yang tidak konsisten]` | `[kernel/arch/x86_64/isr.S]` | `[ISR stub menggunakan format trap frame yang konsisten dan kembali menggunakan iretq.]` | `[Disassembly menunjukkan keberadaan iretq dan interrupt berhasil kembali ke kernel tanpa fault.]` |
| `[Alignment atau format trap frame tidak sesuai]` | `[kernel/core/trap.c dan kernel/arch/x86_64/isr.S]` | `[Trap frame memiliki tata letak yang konsisten antara assembly dan dispatcher.]` | `[IRQ0 berhasil diproses oleh x86_64_trap_dispatch tanpa panic atau triple fault.]` |
| `[Integer overflow atau divisor tidak valid saat konfigurasi PIT]` | `[kernel/arch/x86_64/pit.c]` | `[Frekuensi timer dibatasi dan divisor dihitung dari basis frekuensi PIT yang diketahui.]` | `[PIT berhasil menghasilkan tick periodik 100 Hz pada log QEMU.]` |
| `[Akses port I/O yang salah]` | `[kernel/include/io.h, kernel/arch/x86_64/pic.c, kernel/arch/x86_64/pit.c]` | `[Seluruh akses perangkat dilakukan melalui wrapper inb/outb yang terkontrol.]` | `[PIC remap berhasil, PIT aktif, dan interrupt timer berjalan.]` |
| `[Undefined behavior akibat penggunaan libc host atau runtime hosted]` | `[Seluruh kernel freestanding]` | `[Build menggunakan -ffreestanding, -nostdlib, -fno-builtin, dan target x86_64-unknown-none-elf.]` | `[Log build dan audit ELF menunjukkan kernel dibangun sebagai freestanding ELF64.]` |

### 9.9 Security Boundary

| Boundary | Data tidak tepercaya | Validasi yang dilakukan | Failure mode aman |
|---|---|---|---|
| `[Boot handoff dari bootloader ke kernel]` | `[Alamat struktur boot, informasi memori, dan parameter boot yang diberikan bootloader.]` | `[Kernel memeriksa keberadaan data yang diperlukan sebelum digunakan dan mempertahankan panic path untuk kondisi yang tidak valid.]` | `[Kernel melakukan panic dengan log diagnostik daripada melanjutkan eksekusi yang tidak aman.]` |
| `[Interrupt vector dari CPU dan PIC]` | `[Nomor vector interrupt yang diterima dispatcher.]` | `[Dispatcher memeriksa vector dan hanya menangani interrupt yang telah didaftarkan.]` | `[Vector yang tidak dikenal dicatat sebagai error atau memicu panic terkontrol.]` |
| `[Akses port I/O PIC dan PIT]` | `[Status perangkat dan respons perangkat keras virtual.]` | `[Akses dilakukan hanya melalui wrapper inb/outb yang terdefinisi dan menggunakan port yang telah diketahui.]` | `[Gangguan fungsi perangkat menghasilkan kegagalan timer atau log diagnostik, bukan korupsi memori.]` |
| `[IRQ eksternal]` | `[Sinyal interrupt dari perangkat keras virtual.]` | `[PIC diremap ke 0x20–0x2F dan hanya IRQ0 yang di-unmask pada baseline M5.]` | `[IRQ yang tidak diizinkan tetap dimask sehingga tidak dapat mengganggu kernel.]` |
| `[Trap frame yang diterima dispatcher]` | `[Nilai register dan konteks CPU saat interrupt terjadi.]` | `[ISR dan dispatcher menggunakan format trap frame yang konsisten.]` | `[Kesalahan terdeteksi melalui panic atau fault yang dapat didiagnosis melalui serial log.]` |

---

## 10. Langkah Kerja Implementasi

Gunakan tabel berikut untuk setiap langkah. Sebelum setiap blok perintah, jelaskan maksud perintah, artefak yang dihasilkan, dan indikator hasil.

### Langkah 1 — `[Implementasi Jalur Interrupt dan Timer]`

Maksud langkah:

```text
[Menambahkan dukungan external interrupt pada kernel dengan mengimplementasikan PIC, PIT, ISR stub IRQ 32–47, dan perluasan trap dispatcher agar dapat menangani IRQ0 timer.]
```

Perintah:

```bash
[git checkout praktikum/m5-timer-irq
git status.]
```

Output ringkas:

```text
[On branch praktikum/m5-timer-irq
working tree clean.]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `[pic.c]` | `[kernel/arch/x86_64/pic.c]` | `[Implementasi remap PIC dan EOI]` |
| `[pit.c]` | `[kernel/arch/x86_64/pit.c]` | `[Implementasi timer PIT 100 Hz]` |
| `[isr.S]` | `[kernel/arch/x86_64/isr.S]` | `[ISR stub untuk IRQ 32–47]` |
| `[trap.c]` | `[kernel/core/trap.c]` | `[Dispatcher interrupt dan exception]` |

Indikator berhasil:

```text
[Source code PIC, PIT, ISR, dan dispatcher berhasil terintegrasi tanpa konflik build.]
```
### Langkah 2 — `[Build dan Audit Artefak ELF]`

Maksud langkah:

```text
[Memastikan kernel dapat dikompilasi sebagai ELF64 freestanding dan menghasilkan artefak audit yang diperlukan untuk validasi M5.]
```

Perintah:

```bash
[make clean
make grade]
```

Output ringkas:

```text
[M5] grade artifacts generated
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `[mcsos-m5.elf]` | `[build/]` | `[Kernel ELF hasil build]` |
| `[symbols.txt]` | `[build/]` | `[Audit symbol kernel]` |
| `[undefined.txt]` | `[build/]` | `[Audit undefined symbol]` |
| `[disassembly.txt]` | `[build/]` | `[Audit instruksi mesin]` |

Indikator berhasil:

```text
[Build selesai tanpa error dan seluruh artefak audit berhasil dibuat.]
```

### Langkah 3 — `[Audit Struktur ELF dan Symbol]`

Maksud langkah:

```text
[Memverifikasi bahwa kernel memiliki format ELF yang benar dan symbol penting M5 tersedia.]
```

Perintah:

```bash
[readelf -h build/mcsos-m5.elf
readelf -S build/mcsos-m5.elf
readelf -l build/mcsos-m5.elf

nm -n build/mcsos-m5.elf | grep -E \
"isr_stub_32|pic_remap|pit_configure_hz|timer_on_irq0|x86_64_trap_dispatch"

nm -u build/mcsos-m5.elf.]
```

Output ringkas:

```text
[ELF64 x86-64
pic_remap
pit_configure_hz
timer_on_irq0
x86_64_trap_dispatch
isr_stub_32.]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `[readelf-header.txt]` | `[build/]` | `[Audit ELF header]` |
| `[readelf-sections.txt]` | `[build/]` | `[Audit section layout]` |
| `[symbols.txt]` | `[build/]` | `[Audit symbol penting M5]` |

Indikator berhasil:

```text
Kernel berformat ELF64 dan seluruh symbol PIC, PIT, IRQ, serta dispatcher tersedia.
```

### Langkah 4 — `[Pengujian Runtime Menggunakan QEMU]`

Maksud langkah:

```text
[Memastikan PIC dan PIT menghasilkan interrupt periodik yang dapat diproses kernel.]
```

Perintah:

```bash
[qemu-system-x86_64 \
  -M q35 \
  -m 512M \
  -cdrom build/mcsos.iso \
  -serial stdio \
  -no-reboot \
  -no-shutdown \
  2>&1 | tee evidence/m5-qemu.log.]
```

Output ringkas:

```text
[M5] remapping PIC
[M5] configuring PIT
[M5] enabling interrupts
[M5] timer IRQ online

[MCSOS:TIMER] ticks=count=0x64
[MCSOS:TIMER] ticks=count=0xc8
[MCSOS:TIMER] ticks=count=0x12c
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `[m5-qemu.log]` | `[evidence/]` | `[Bukti runtime timer dan interrupt]` |

Indikator berhasil:

```text
[Tick periodik muncul pada serial log dan terus bertambah selama kernel berjalan.]
```

### Langkah 5 — `[Validasi Menggunakan GDB]`

Maksud langkah:

```text
[Memastikan fungsi-fungsi utama M5 benar-benar dieksekusi selama boot dan runtime.]
```

Perintah:

```bash
[gdb build/mcsos-m5.elf

target remote :1234

break kmain
break pic_remap
break pit_configure_hz
break x86_64_trap_dispatch

continue]
```

Output ringkas:

```text
[Breakpoint 1 at kmain
Breakpoint 2 at pic_remap
Breakpoint 3 at pit_configure_hz
Breakpoint 4 at x86_64_trap_dispatch

Breakpoint 1, kmain()]
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `[Screenshot GDB]` | `[Lampiran laporan]` | `[Bukti validasi runtime]` |

Indikator berhasil:

```text
[Breakpoint berhasil dipasang dan eksekusi berhenti pada fungsi yang diharapkan.]
```

## 11. Checkpoint Buildable

Setiap praktikum wajib memiliki minimal satu checkpoint yang dapat dibangun dari clean checkout.

| Checkpoint | Perintah | Expected result | Status |
|---|---|---|---|
| Clean build | `make clean && make grade` | `[Kernel ELF dan artefak audit berhasil dibangun]` | `[PASS]` |
| Metadata toolchain | `make meta` | `[build/meta/toolchain-versions.txt ada]` | `[NA]` |
| Image generation | `make image` | `[build/mcsos.iso tersedia]` | `[PASS]` |
| QEMU smoke test | `qemu-system-x86_64 -M q35 -m 512M -cdrom build/mcsos.iso -serial stdio -no-reboot -no-shutdown` | `[Serial log M5 dan tick timer periodik muncul]` | `[PASS]` |
| Test suite | `make test` | `[Seluruh test relevan lulus]` | `[NA]` |

Catatan checkpoint:

```text
[Checkpoint clean build dinyatakan lulus karena make clean && make grade berhasil menghasilkan kernel ELF, symbol table, disassembly, dan artefak audit lainnya tanpa error. Image generation dinyatakan lulus karena build/mcsos.iso berhasil dibuat dan dapat digunakan untuk boot QEMU.
QEMU smoke test dinyatakan lulus karena log runtime menunjukkan PIC berhasil diremap, PIT berhasil dikonfigurasi, interrupt diaktifkan, dan tick timer periodik muncul pada serial log.
Checkpoint make meta dan make test ditandai NA karena tidak digunakan sebagai bagian dari alur validasi M5 yang dijalankan pada repository ini. Validasi utama dilakukan melalui build audit, audit ELF, audit symbol, audit disassembly, QEMU runtime test, dan GDB breakpoint verification.]
```
---

# 12. Perintah Uji dan Validasi

### 12.1 Build Test

Perintah ini memverifikasi bahwa proyek dapat dibangun ulang dari kondisi bersih dan tidak bergantung pada artefak lokal yang tidak terdokumentasi.

```bash
make clean
make grade
```

Hasil:

```text
[rm -rf build
...
ld.lld -nostdlib -static -z max-page-size=0x1000 -T linker.ld -Map=build/kernel.map -o build/kernel.elf
...
[M5] grade artifacts generated]
```

Status: `[PASS]`

### 12.2 Static Inspection

Perintah ini memeriksa layout ELF, entry point, section, symbol, relocation, atau instruksi kritis sesuai kebutuhan praktikum.

```bash
readelf -h build/mcsos-m5.elf
readelf -l build/mcsos-m5.elf
readelf -S build/mcsos-m5.elf

nm -n build/mcsos-m5.elf | grep -E \
"isr_stub_32|pic_remap|pit_configure_hz|timer_on_irq0|x86_64_trap_dispatch"

nm -u build/mcsos-m5.elf

objdump -d build/mcsos-m5.elf | grep -E "lidt|iretq|outb|sti|hlt"
```

Hasil penting:

```text
[ELF Header:
Class: ELF64
Machine: Advanced Micro Devices X86-64
Type: EXEC (Executable file)
Entry point address: 0xffffffff800006a0

Program Headers:
LOAD R E  (.text)
LOAD R    (.rodata)
LOAD RW   (.data .requests .bss)

Symbol verification:
ffffffff80000360 T pic_remap
ffffffff800005b0 T pit_configure_hz
ffffffff80000650 T timer_on_irq0
ffffffff80000f90 T x86_64_trap_dispatch
ffffffff800015a8 T isr_stub_32

Undefined symbols:
(none)

Critical instructions:
lidt
iretq
outb
sti
hlt]
```

Status: `[PASS]`

### 12.3 QEMU Smoke Test

Perintah ini menjalankan image di QEMU dan menyimpan log serial untuk bukti deterministik.

```bash
qemu-system-x86_64 \
  -M q35 \
  -m 512M \
  -cdrom build/mcsos.iso \
  -serial stdio \
  -no-reboot \
  -no-shutdown
```

Hasil:

```text
[limine: Loading executable `boot():/boot/kernel.elf`...

MCSOS 260502 M4 kernel entered
kernel_start=0xffffffff80000000
kernel_end=0xffffffff80005838

[M4] IDT loaded
[M4] selftest: IDT invariants passed

[M5] remapping PIC
[M5] configuring PIT

pic_master_mask=0x00000000000000fe

[M5] enabling interrupts
[M5] timer IRQ online
[M5] entering idle halt loop

[MCSOS:TIMER] ticks=count=0x0000000000000064
[MCSOS:TIMER] ticks=count=0x00000000000000c8
[MCSOS:TIMER] ticks=count=0x000000000000012c
[MCSOS:TIMER] ticks=count=0x0000000000000190
[MCSOS:TIMER] ticks=count=0x00000000000001f4]
```

Status: `[PASS]`

### 12.4 GDB Debug Evidence

Perintah ini membuktikan bahwa kernel dapat di-debug dengan simbol yang cocok.

```bash
qemu-system-x86_64 \
  -M q35 \
  -m 512M \
  -cdrom build/mcsos.iso \
  -serial stdio \
  -no-reboot \
  -no-shutdown \
  -s -S
```

Di terminal lain:

```bash
gdb build/mcsos-m5.elf

target remote :1234

break kmain
break pic_remap
break pit_configure_hz
break x86_64_trap_dispatch

continue
```

Hasil:

```text
[Remote debugging using :1234
0x000000000000fff0 in ?? ()
(gdb) break kmain
Breakpoint 1 at 0xffffffff800006a0
(gdb) break pic_remap
Breakpoint 2 at 0xffffffff80000360
(gdb) break pit_configure_hz
Breakpoint 3 at 0xffffffff800005b0
(gdb) break x86_64_trap_dispatch
Breakpoint 4 at 0xffffffff80000f90
(gdb) continue
Continuing.
Breakpoint 1, 0xffffffff800006a0 in kmain () ]
```

Status: `[PASS]`

### 12.5 Unit Test

```bash
make test
```

Hasil:

```text
[Tidak dijalankan pada praktikum M5. Validasi dilakukan melalui build audit, audit ELF, audit symbol, audit disassembly, QEMU runtime test, dan GDB debugging.]
```

Status: `[NA]`

### 12.6 Stress/Fuzz/Fault Injection Test

Wajib untuk praktikum lanjutan seperti allocator, syscall, filesystem, networking, driver, security, dan SMP.

```bash
[Tidak diterapkan pada M5]
```

Hasil:

```text
[Praktikum M5 berfokus pada implementasi PIC, PIT, IRQ0, dan timer interrupt dasar. Validasi dilakukan melalui build audit, audit ELF, audit symbol, audit disassembly, runtime test QEMU, dan debugging menggunakan GDB. Tidak dilakukan stress test, fuzzing, maupun fault injection terstruktur pada tahap ini.]
```

Status: `[NA]`

### 12.7 Visual Evidence

Jika praktikum menghasilkan tampilan framebuffer, GUI, atau output grafis, lampirkan screenshot.

| Screenshot | Lokasi file | Keterangan |
|---|---|---|
| `[build-success.png]` | `[evidence/screenshots/build-success.png]` | `[Menunjukkan make clean && make grade berhasil dan artefak M5 berhasil dibuat.]` |
| `[qemu-timer-log.png]` | `[evidence/screenshots/qemu-timer-log.png]` | `[Menunjukkan kernel berhasil boot, PIC dan PIT aktif, serta tick timer periodik muncul pada serial log.]` |
| `[gdb-breakpoint-kmain.png]` | `[evidence/screenshots/gdb-breakpoint-kmain.png]` | `[Menunjukkan GDB berhasil terhubung ke QEMU dan breakpoint pada kmain() berhasil dicapai.]` |


---

## 13. Hasil Uji

### 13.1 Tabel Ringkasan Hasil

| No. | Uji | Expected result | Actual result | Status | Evidence |
|---|---|---|---|---|---|
| 1 | `[Build Test]` | `[Kernel dan artefak audit berhasil dibangun dari kondisi bersih.]` | `[make clean && make grade berhasil dan menghasilkan mcsos-m5.elf, symbol table, disassembly, serta artefak audit lainnya.]` | `[PASS]` | `[evidence/m5-build.log]` |
| 2 | `[ELF Header Verification]` | `[Kernel berformat ELF64 x86_64 dengan entry point valid.]` | `[readelf menunjukkan ELF64, x86_64, dan entry point 0xffffffff800006a0.]` | `[PASS]` | `[build/readelf-header.txt]` |
| 3 | `[Section dan Program Header Verification]` | `[Section .text, .rodata, .data, dan .bss tersusun dengan benar.]` | `[Layout section dan segment berhasil diverifikasi melalui readelf.]` | `[PASS]` | `[build/readelf-sections.txt, build/readelf-program-headers.txt]` |
| 4 | `[Symbol Verification]` | `[Symbol PIC, PIT, IRQ, dispatcher, dan ISR tersedia.]` | `[pic_remap, pit_configure_hz, timer_on_irq0, x86_64_trap_dispatch, dan isr_stub_32 ditemukan.]` | `[PASS]` | `[build/symbols.txt]` |
| 5 | `[Undefined Symbol Audit]` | `[Tidak terdapat unresolved external symbol.]` | `[nm -u tidak menghasilkan output.]` | `[PASS]` | `[build/undefined.txt]` |
| 6 | `[Disassembly Verification]` | `[Instruksi kritis interrupt ditemukan.]` | `[Instruksi lidt, iretq, outb, sti, dan hlt ditemukan pada disassembly.]` | `[PASS]` | `[build/disassembly.txt]` |
| 7 | `[QEMU Runtime Test]` | `[Kernel boot dan timer interrupt berjalan.]` | `[Kernel berhasil boot, PIC diremap, PIT aktif, dan tick periodik muncul.]` | `[PASS]` | `[evidence/m5-qemu.log]` |
| 8 | `[Timer Tick Validation]` | `[Counter tick meningkat secara periodik.]` | `[ticks=count=0x64, 0xc8, 0x12c, dan seterusnya muncul pada serial log.]` | `[PASS]` | `[evidence/m5-qemu.log]` |
| 9 | `[GDB Breakpoint Verification]` | `[Breakpoint dapat dipasang dan dicapai.]` | `[GDB berhasil berhenti pada fungsi kmain().]` | `[PASS]` | `[Screenshot GDB / Lampiran]` |


### 13.2 Log Penting

```text
[limine: Loading executable `boot():/boot/kernel.elf`...

MCSOS 260502 M4 kernel entered
kernel_start=0xffffffff80000000
kernel_end=0xffffffff80005838

rflags_before_idt=0x0000000000000082
idt_base=0xffffffff80004000
idt_limit=0x0000000000000fff

[M4] IDT loaded
[M4] selftest: IDT invariants passed

[M5] remapping PIC
[M5] configuring PIT

pic_master_mask=0x00000000000000fe

[M5] enabling interrupts
[M5] timer IRQ online
[M5] entering idle halt loop

[MCSOS:TIMER] ticks=count=0x0000000000000064
[MCSOS:TIMER] ticks=count=0x00000000000000c8
[MCSOS:TIMER] ticks=count=0x000000000000012c
[MCSOS:TIMER] ticks=count=0x0000000000000190
[MCSOS:TIMER] ticks=count=0x00000000000001f4
```

### 13.3 Artefak Bukti

| Artefak | Path | SHA-256 / hash | Fungsi |
|---|---|---|---|
| `kernel.elf` | `[build/kernel.elf]` | `[bbd4c81800bc40417c81423ce1dbfce21a65ef0971b9fa5241fef4b69713bb81]` | `[Kernel binary utama hasil build.]` |
| `mcsos.iso` | `[build/mcsos.iso]` | `[a1b48fa9725026324eac58df3ac8b3ca569ca5ae59366072cb83beedb1cdac27]` | `[Boot image yang digunakan oleh QEMU.]` |
| `m5-qemu.log` | `[evidence/m5-qemu.log]` | `[c27bd79c47916132e4de1d7eefd2001f31d95a903e16e539da11e421e6ca9044]` | `[Log runtime yang membuktikan PIC, PIT, dan timer interrupt berjalan.]` |
| `kernel.map` | `[build/kernel.map]` | `[8763b4d68e3803db8c31f7a324ba285d5b0b9e7bc4b2fec187e76bd05cc410b3]` | `[Peta simbol dan layout hasil linking.]` |
| `disassembly.txt` | `[build/disassembly.txt]` | `[7b7a24c3460fd7c642d94f838261a09fa28a458c5a18c1e24619972628859a73]` | `[Bukti keberadaan instruksi kritis seperti lidt, iretq, outb, sti, dan hlt.]` |
| `symbols.txt` | `[build/symbols.txt]` | `[e5c061ba0b58b80b76922c1582809c33fef09337d3aa56775338fa71bc636ca6]` | `[Bukti symbol PIC, PIT, IRQ, dispatcher, dan ISR.]` |

Perintah hash:

```bash
sha256sum build/kernel.elf
sha256sum build/mcsos.iso
sha256sum evidence/m5-qemu.log
sha256sum build/kernel.map
sha256sum build/disassembly.txt
sha256sum build/symbols.txt
```

---

## 14. Analisis Teknis

### 14.1 Analisis Keberhasilan

```text
[Hasil praktikum M5 dinyatakan berhasil karena seluruh tujuan utama implementasi external interrupt dan timer dasar dapat dicapai serta dibuktikan melalui build audit, audit ELF, audit symbol, audit disassembly, runtime QEMU, dan debugging menggunakan GDB.
Dari sisi desain, PIC berhasil diremap ke rentang vector 0x20–0x2F sehingga tidak lagi bertabrakan dengan exception CPU 0–31. Kebijakan hanya membuka IRQ0 dan tetap mem-mask IRQ lain juga terbukti efektif untuk mengurangi kompleksitas debugging dan menjaga sistem tetap fail-closed selama tahap awal pengembangan.
Dari sisi implementasi, PIT channel 0 berhasil dikonfigurasi pada frekuensi 100 Hz dan menghasilkan interrupt periodik yang diterima kernel melalui vector 32 (IRQ0). Trap dispatcher berhasil membedakan exception dan external interrupt sehingga timer interrupt tidak diperlakukan sebagai fatal exception. Hal ini dibuktikan oleh log runtime yang menunjukkan status "timer IRQ online" dan kemunculan tick periodik yang terus bertambah.
Invariant utama praktikum juga tetap terjaga. IDT berhasil dimuat dan diverifikasi melalui selftest, vector interrupt tetap berada pada rentang yang benar, dan setiap interrupt timer dapat diproses tanpa menyebabkan panic maupun triple fault. Selain itu, keberadaan instruksi kritis seperti lidt, iretq, outb, sti, dan hlt pada hasil disassembly menunjukkan bahwa jalur interrupt telah terpasang dengan benar.
Keberhasilan implementasi juga didukung oleh hasil audit statis. Kernel berhasil dibangun sebagai ELF64 x86_64 freestanding, symbol PIC, PIT, IRQ, dispatcher, dan ISR ditemukan pada symbol table, serta audit undefined symbol menunjukkan tidak adanya unresolved external symbol. Hasil ini mengindikasikan bahwa seluruh dependensi kernel telah terhubung dengan benar pada saat linking.
Pada pengujian runtime, serial log QEMU menunjukkan urutan inisialisasi yang sesuai desain, yaitu IDT dimuat terlebih dahulu, kemudian PIC diremap, PIT dikonfigurasi, interrupt diaktifkan, dan akhirnya tick timer periodik muncul secara konsisten. Selain itu, pengujian menggunakan GDB berhasil mencapai breakpoint pada fungsi kmain(), membuktikan bahwa simbol debug sesuai dengan kernel yang dijalankan dan proses debugging dapat dilakukan dengan benar.
Secara keseluruhan, seluruh kriteria lulus M5 berhasil dipenuhi, termasuk build reproducibility, validasi IDT, remap PIC, konfigurasi PIT, aktivasi IRQ0, audit ELF, audit symbol, audit disassembly, runtime timer tick, dan debugging menggunakan GDB. Oleh karena itu, implementasi dapat dinyatakan berhasil dan mencapai status readiness "siap uji QEMU" untuk jalur external interrupt dan timer dasar.]
```

### 14.2 Analisis Kegagalan atau Perbedaan Hasil

```text
[Selama pelaksanaan praktikum M5 terdapat beberapa kendala dan perbedaan hasil yang muncul pada tahap implementasi maupun pengujian. Kendala pertama terjadi saat pengujian QEMU ketika image boot belum tersedia pada lokasi yang diharapkan. Gejalanya berupa pesan "Could not open 'build/mcsos.iso': No such file or directory". Akar masalahnya adalah image ISO belum dibuat atau path yang digunakan tidak sesuai dengan lokasi artefak hasil build. Bukti pendukung diperoleh dari pesan error QEMU. Tindakan perbaikan dilakukan dengan memastikan proses build image berhasil dan memverifikasi keberadaan file build/mcsos.iso sebelum menjalankan QEMU.
Kendala kedua terjadi saat penggunaan GDB. Pada percobaan awal, beberapa breakpoint dimasukkan dalam satu perintah sehingga GDB menganggap seluruh teks sebagai nama fungsi tunggal dan menampilkan pesan bahwa fungsi tersebut tidak ditemukan. Gejalanya adalah kegagalan pemasangan breakpoint. Akar masalahnya berasal dari kesalahan penggunaan sintaks GDB, bukan kesalahan implementasi kernel. Perbaikan dilakukan dengan memasukkan setiap perintah break secara terpisah sehingga breakpoint pada kmain, pic_remap, pit_configure_hz, dan x86_64_trap_dispatch berhasil dipasang.
Kendala ketiga berkaitan dengan validasi runtime interrupt. Pada tahap awal pengembangan terdapat risiko bahwa timer interrupt tidak muncul meskipun kernel berhasil boot. Gejala yang mungkin muncul adalah tidak adanya perubahan nilai tick atau tidak munculnya log timer periodik. Dugaan akar masalah mencakup konfigurasi PIC yang salah, IRQ0 masih termask, kesalahan konfigurasi PIT, tidak terkirimnya End Of Interrupt (EOI), atau kesalahan pemasangan handler pada IDT. Untuk mengurangi risiko tersebut dilakukan audit symbol, audit disassembly, verifikasi breakpoint GDB, dan pengamatan serial log QEMU hingga diperoleh bukti bahwa timer interrupt berjalan normal.
Setelah seluruh perbaikan dilakukan, hasil akhir sesuai dengan desain yang direncanakan. Kernel berhasil melakukan remap PIC, mengonfigurasi PIT, mengaktifkan IRQ0, menerima interrupt timer periodik, dan menampilkan tick yang terus bertambah pada serial log. Tidak ditemukan panic, hang, maupun triple fault selama pengujian yang didokumentasikan pada laporan ini.]
```

### 14.3 Perbandingan dengan Teori

| Konsep teori | Implementasi praktikum | Sesuai/tidak sesuai | Penjelasan |
|---|---|---|---|
| `[Interrupt Descriptor Table (IDT) digunakan untuk memetakan vector interrupt ke handler.]` | `[Kernel memuat IDT menggunakan instruksi lidt dan mengisi vector 0–47 dengan handler yang sesuai.]` | `[Sesuai]` | `[Log runtime menunjukkan IDT berhasil dimuat dan audit disassembly menemukan instruksi lidt.]` |
| `[Exception CPU dan hardware interrupt sebaiknya menggunakan vector yang tidak saling bertabrakan.]` | `[PIC diremap ke rentang 0x20–0x2F sehingga IRQ0 menggunakan vector 32.]` | `[Sesuai]` | `[Remap PIC menghindari konflik dengan exception CPU 0–31 sebagaimana direkomendasikan pada arsitektur x86.]` |
| `[PIT dapat digunakan sebagai sumber interrupt periodik untuk sistem operasi.]` | `[PIT channel 0 dikonfigurasi pada frekuensi 100 Hz dan menghasilkan tick periodik.]` | `[Sesuai]` | `[Serial log menunjukkan nilai tick terus bertambah selama kernel berjalan.]` |
| `[Interrupt handler harus mengembalikan kontrol menggunakan mekanisme return interrupt.]` | `[ISR stub menggunakan iretq untuk mengembalikan konteks CPU setelah interrupt selesai diproses.]` | `[Sesuai]` | `[Audit disassembly menunjukkan keberadaan instruksi iretq.]` |
| `[Programmable Interrupt Controller (PIC) memerlukan End Of Interrupt (EOI) setelah interrupt selesai diproses.]` | `[Kernel mengirim EOI setelah IRQ timer diproses.]` | `[Sesuai]` | `[Timer dapat terus menghasilkan interrupt secara periodik tanpa terhenti.]` |
| `[Kernel freestanding tidak boleh bergantung pada runtime atau libc host.]` | `[Kernel dibangun menggunakan -ffreestanding, -nostdlib, dan target ELF freestanding.]` | `[Sesuai]` | `[Build berhasil menghasilkan kernel ELF64 tanpa ketergantungan pada library host.]` |
| `[Interrupt handler sebaiknya singkat dan tidak melakukan operasi blocking.]` | `[Handler IRQ0 hanya memperbarui counter tick, melakukan logging periodik, dan mengirim EOI.]` | `[Sesuai]` | `[Desain ini menjaga latency interrupt tetap rendah dan mempermudah debugging.]` |

### 14.4 Kompleksitas dan Kinerja

| Aspek | Estimasi/hasil | Bukti | Catatan |
|---|---|---|---|
| Kompleksitas algoritma | `[O(1) per interrupt timer]` | `[Analisis source code timer_on_irq0() dan dispatcher.]` | `[Handler IRQ0 hanya melakukan pemeriksaan vector, increment counter, logging periodik, dan EOI.]` |
| Waktu build | `[< 10 detik pada lingkungan praktikum]` | `[evidence/m5-build.log]` | `[Build berhasil menyelesaikan kompilasi, linking, dan audit artefak tanpa error.]` |
| Waktu boot QEMU | `[Beberapa detik hingga muncul marker "timer IRQ online"]` | `[evidence/m5-qemu.log]` | `[Kernel berhasil boot, memuat IDT, mengonfigurasi PIC dan PIT, lalu mengaktifkan interrupt.]` |
| Penggunaan memori | `[Tidak diukur secara kuantitatif pada M5]` | `[Tidak tersedia metrik runtime.]` | `[Praktikum berfokus pada jalur interrupt dan timer, bukan profiling memori.]` |
| Latensi/throughput | `[Timer periodik 100 Hz]` | `[evidence/m5-qemu.log menunjukkan tick bertambah secara konsisten.]` | `[M5 hanya memvalidasi keberadaan interrupt periodik, bukan benchmark performa.]` |

---

## 15. Debugging dan Failure Modes

### 15.1 Failure Modes yang Ditemukan

| Failure mode | Gejala | Penyebab sementara | Bukti | Perbaikan |
|---|---|---|---|---|
| `[QEMU gagal boot karena image tidak ditemukan]` | `[QEMU menampilkan pesan "Could not open 'build/mcsos.iso': No such file or directory".]` | `[File ISO belum dibuat atau path yang digunakan tidak sesuai.]` | `[Output terminal QEMU saat pengujian awal.]` | `[Memastikan build image berhasil dan memverifikasi keberadaan build/mcsos.iso sebelum menjalankan QEMU.]` |
| `[Breakpoint GDB gagal dipasang]` | `[GDB melaporkan fungsi tidak ditemukan saat beberapa breakpoint ditulis dalam satu perintah.]` | `[Kesalahan sintaks penggunaan GDB sehingga seluruh teks dianggap sebagai satu nama fungsi.]` | `[Output GDB yang menampilkan pesan Function not defined.]` | `[Memasukkan setiap perintah break secara terpisah sehingga breakpoint berhasil dipasang.]` |
| `[Timer interrupt tidak muncul]` | `[Kernel berhasil boot tetapi tidak ada log tick periodik.]` | `[Kemungkinan IRQ0 masih termask, PIC belum diremap dengan benar, PIT belum aktif, atau EOI tidak dikirim.]` | `[Tidak munculnya log ticks=count pada serial log.]` | `[Memverifikasi konfigurasi PIC, PIT, IDT, dispatcher, dan jalur EOI melalui audit serta debugging.]` |
| `[Interrupt storm]` | `[Log timer muncul sangat cepat atau sistem menjadi tidak responsif.]` | `[EOI tidak dikirim dengan benar atau konfigurasi PIC/PIT tidak sesuai.]` | `[Dugaan berdasarkan analisis desain interrupt dan dokumentasi PIC.]` | `[Memastikan EOI dikirim setelah IRQ diproses dan hanya IRQ0 yang diaktifkan.]` |
| `[Triple fault akibat konfigurasi IDT atau ISR salah]` | `[QEMU reset, hang, atau berhenti tanpa log diagnostik.]` | `[Vector IDT tidak valid, handler salah, atau format trap frame tidak konsisten.]` | `[Failure mode yang dianalisis selama pengembangan jalur interrupt.]` | `[Memverifikasi IDT, ISR stub, dispatcher, dan instruksi iretq melalui audit dan GDB.]` |


### 15.2 Failure Modes yang Diantisipasi

| Failure mode | Deteksi | Dampak | Mitigasi |
|---|---|---|---|
| `[IRQ0 tetap termask setelah inisialisasi PIC]` | `[Tidak muncul log timer periodik pada serial console.]` | `[Kernel tidak menerima interrupt timer sehingga tick tidak bertambah.]` | `[Memverifikasi mask register PIC dan memastikan IRQ0 di-unmask selama inisialisasi.]` |
| `[PIC tidak diremap dengan benar]` | `[Interrupt masuk ke vector yang tidak sesuai atau memicu panic.]` | `[Konflik antara IRQ dan exception CPU sehingga diagnosis menjadi sulit.]` | `[Memverifikasi urutan ICW1–ICW4 dan memastikan vector berada pada rentang 0x20–0x2F.]` |
| `[EOI tidak dikirim setelah IRQ diproses]` | `[Tick berhenti bertambah setelah beberapa interrupt pertama.]` | `[PIC tidak mengirim interrupt berikutnya sehingga timer berhenti.]` | `[Memastikan pic_send_eoi() dipanggil pada jalur IRQ yang valid.]` |
| `[Trap frame tidak konsisten dengan ISR stub]` | `[Kernel panic, hang, atau triple fault saat interrupt terjadi.]` | `[Dispatcher membaca konteks CPU yang salah dan menghasilkan perilaku tidak terdefinisi.]` | `[Menjaga kesesuaian layout trap frame antara assembly dan kode C serta melakukan audit disassembly.]` |
| `[PIT dikonfigurasi dengan divisor yang salah]` | `[Frekuensi tick terlalu cepat, terlalu lambat, atau tidak muncul.]` | `[Sumber waktu kernel menjadi tidak akurat.]` | `[Memverifikasi perhitungan divisor dan menguji hasil melalui serial log QEMU.]` |
| `[Vector IDT tidak menunjuk ke handler yang valid]` | `[Interrupt menyebabkan fault atau reset sistem.]` | `[Kernel dapat mengalami hang atau triple fault.]` | `[Melakukan selftest IDT, audit symbol, dan verifikasi handler melalui GDB.]` |

### 15.3 Triage yang Dilakukan

```text
[Proses diagnosis dilakukan secara bertahap dengan pendekatan dari gejala paling terlihat hingga analisis tingkat rendah.

1. Serial log QEMU digunakan sebagai alat diagnosis utama untuk memverifikasi urutan boot, pemuatan IDT, konfigurasi PIC, konfigurasi PIT, aktivasi interrupt, dan kemunculan tick timer periodik.
2. Ketika terjadi masalah saat pengujian runtime, keberadaan artefak build seperti kernel.elf, mcsos.iso, symbol table, dan disassembly diperiksa untuk memastikan build berhasil dan seluruh komponen yang diperlukan tersedia.
3. Audit ELF dilakukan menggunakan readelf untuk memverifikasi format ELF64, entry point, section layout, dan program header kernel.
4. Audit symbol dilakukan menggunakan nm untuk memastikan symbol penting seperti pic_remap, pit_configure_hz, timer_on_irq0, isr_stub_32, dan x86_64_trap_dispatch tersedia serta tidak terdapat undefined symbol.
5. Audit disassembly dilakukan menggunakan objdump untuk memastikan instruksi kritis seperti lidt, iretq, outb, sti, dan hlt benar-benar terdapat pada binary hasil build.
6. Debugging runtime dilakukan menggunakan GDB yang terhubung ke QEMU melalui target remote :1234. Breakpoint dipasang pada kmain, pic_remap, pit_configure_hz, dan x86_64_trap_dispatch untuk memverifikasi jalur eksekusi.
7. Hasil diagnosis dibandingkan dengan source code, linker map, symbol table, dan log runtime untuk memastikan bahwa masalah berasal dari konfigurasi atau penggunaan alat, bukan dari implementasi kernel yang telah diverifikasi.]
```

### 15.4 Panic Path

```text
[Selama pengujian yang didokumentasikan pada praktikum M5 tidak terjadi panic runtime, kernel hang, maupun triple fault. Oleh karena itu tidak terdapat panic log yang dapat ditempelkan pada laporan.
Meskipun demikian, panic path tetap dipertahankan dari implementasi M4 dan tidak dihapus selama pengembangan M5. Keberadaan panic path diverifikasi secara tidak langsung melalui audit source code, symbol table, dan integrasi trap dispatcher yang masih membedakan exception fatal dan external interrupt.
Selain itu, salah satu tujuan desain M5 adalah memastikan bahwa IRQ0 timer tidak diperlakukan sebagai fatal exception. Keberhasilan pemisahan jalur tersebut dibuktikan oleh munculnya tick timer periodik pada serial log tanpa memicu panic. Dengan demikian, panic path tetap tersedia sebagai mekanisme fail-safe untuk exception fatal, meskipun tidak dipicu selama rangkaian pengujian yang dilakukan.]
```

---

## 16. Prosedur Rollback

Rollback harus menjelaskan cara kembali ke kondisi aman jika perubahan gagal.

| Skenario rollback | Perintah | Data yang harus diselamatkan | Status |
|---|---|---|---|
| Kembali ke commit awal | `git checkout 82ccae6` | `[evidence/m5-build.log, evidence/m5-qemu.log, screenshot, dan laporan praktikum.]` | `[Belum diuji]` |
| Revert commit praktikum | `git revert 1f955e7` | `[evidence/m5-build.log, evidence/m5-qemu.log, screenshot, dan laporan praktikum.]` | `[Belum diuji]` |
| Bersihkan artefak build | `make clean` | `[Tidak ada; source code tetap aman di repository Git.]` | `[Teruji]` |
| Regenerasi image | `make grade` | `[Image lama jika masih diperlukan untuk perbandingan atau reproduksi hasil.]` | `[Teruji]` |

Catatan rollback:

```text
Rollback penuh ke commit awal maupun revert commit praktikum tidak diuji secara eksplisit selama pelaksanaan M5 karena fokus praktikum berada pada implementasi dan validasi jalur interrupt serta timer. Namun seluruh perubahan telah dicommit ke Git dan dipush ke repository sehingga rollback dapat dilakukan kapan saja menggunakan mekanisme version control.
Prosedur yang benar-benar diuji adalah make clean dan build ulang menggunakan make grade. Hal ini dibuktikan oleh keberhasilan regenerasi kernel ELF, image boot, symbol table, disassembly, dan artefak audit lainnya setelah direktori build dihapus.
Risiko utama rollback adalah hilangnya artefak evidence yang belum dicadangkan. Oleh karena itu log build, log QEMU, screenshot, laporan, serta commit hash penting harus disimpan sebelum melakukan checkout atau revert ke revisi sebelumnya.
```
---

## 17. Keamanan dan Reliability

### 17.1 Risiko Keamanan

| Risiko | Boundary | Dampak | Mitigasi | Evidence |
|---|---|---|---|---|
| `[IRQ tidak di-remap dengan benar]` | `[PIC → Kernel Interrupt Dispatcher]` | `[Konflik antara IRQ dan exception CPU sehingga diagnosis menjadi tidak akurat.]` | `[PIC diremap ke rentang 0x20–0x2F.]` | `[evidence/m5-qemu.log, build/symbols.txt]` |
| `[IRQ tidak sah atau tidak diharapkan masuk ke dispatcher]` | `[Interrupt Vector → Trap Dispatcher]` | `[Kernel dapat menjalankan handler yang salah atau memicu panic.]` | `[Dispatcher memeriksa vector dan hanya menangani IRQ yang valid.]` | `[Review source code trap.c dan audit symbol.]` |
| `[Akses port I/O yang salah]` | `[Kernel → PIC/PIT]` | `[Perangkat tidak berfungsi atau interrupt tidak berjalan.]` | `[Penggunaan wrapper inb/outb yang terkontrol.]` | `[Audit disassembly dan runtime log.]` |
| `[Exception fatal tidak terdeteksi]` | `[CPU Exception → Kernel]` | `[Kernel dapat melanjutkan eksekusi pada kondisi tidak aman.]` | `[Panic path dari M4 tetap dipertahankan.]` | `[Review source code dan log runtime.]` |
| `[Membuka IRQ terlalu banyak pada tahap awal]` | `[PIC → Kernel]` | `[Interrupt storm atau perilaku tidak terduga.]` | `[Hanya IRQ0 yang di-unmask, IRQ lain tetap dimask.]` | `[pic_master_mask pada serial log.]` |

### 17.2 Reliability dan Data Integrity

| Risiko reliability | Dampak | Deteksi | Mitigasi |
|---|---|---|---|
| `[Timer interrupt tidak berjalan]` | `[Tick kernel tidak bertambah dan subsistem waktu tidak tersedia.]` | `[Tidak muncul log ticks=count.]` | `[Verifikasi PIC, PIT, IDT, dan EOI.]` |
| `[Hang akibat konfigurasi interrupt salah]` | `[Kernel berhenti merespons.]` | `[Serial log berhenti atau tidak berkembang.]` | `[Audit IDT, ISR, dispatcher, dan debugging GDB.]` |
| `[Triple fault]` | `[Kernel reset atau berhenti tanpa diagnosis.]` | `[QEMU reset atau berhenti mendadak.]` | `[Validasi IDT, trap frame, dan instruksi iretq.]` |
| `[Interrupt berhenti setelah beberapa tick]` | `[Sumber waktu kernel terhenti.]` | `[Nilai tick tidak bertambah lagi.]` | `[Memastikan EOI selalu dikirim setelah IRQ selesai diproses.]` |
| `[Build tidak reproducible]` | `[Artefak berbeda antar build.]` | `[Build gagal atau hasil audit berubah.]` | `[Menggunakan toolchain dan build procedure yang terdokumentasi.]` |

### 17.3 Negative Test

| Negative test | Input buruk | Expected result | Actual result | Status |
|---|---|---|---|---|
| `[QEMU dijalankan tanpa image ISO yang valid]` | `[build/mcsos.iso tidak ada.]` | `[QEMU menampilkan error dan tidak melakukan boot.]` | `[QEMU menampilkan "Could not open 'build/mcsos.iso'.]` | `[PASS]` |
| `[Breakpoint GDB ditulis dengan sintaks yang salah]` | `[Beberapa breakpoint dimasukkan dalam satu perintah.]` | `[GDB menolak perintah dan menampilkan pesan error.]` | `[GDB menampilkan Function not defined.]` | `[PASS]` |
| `[IRQ selain IRQ0 tetap dimask]` | `[Interrupt eksternal selain IRQ0.]` | `[Interrupt tidak diproses oleh kernel.]` | `[Sesuai desain baseline M5, hanya IRQ0 yang aktif.]` | `[PASS]` |
| `[Panic runtime]` | `[Exception fatal.]` | `[Kernel masuk panic path dengan log yang dapat dibaca.]` | `[Tidak terjadi selama pengujian M5.]` | `[NA]` |

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
| Proyek dapat dibangun dari clean checkout | `[PASS]` | `[evidence/m5-build.log]` |
| Perintah build terdokumentasi | `[PASS]` | `[Bagian 10 dan 12 laporan]` |
| QEMU boot atau test target berjalan deterministik | `[PASS]` | `[evidence/m5-qemu.log]` |
| Semua unit test/praktikum test relevan lulus | `[NA]` | `[Tidak ada unit test khusus yang dijalankan pada M5]` |
| Log serial disimpan | `[PASS]` | `[evidence/m5-qemu.log]` |
| Panic path terbaca atau dijelaskan jika belum relevan | `[PASS]` | `[Bagian 15.4 Panic Path]` |
| Tidak ada warning kritis pada build | `[PASS]` | `[evidence/m5-build.log]` |
| Perubahan Git terkomit | `[PASS]` | `[1f955e7, 4a76ef9, 9604535, 5d467ac]` |
| Desain dan failure mode dijelaskan | `[PASS]` | `[Bab 9 dan Bab 15]` |
| Laporan berisi screenshot/log yang cukup | `[PASS]` | `[Lampiran screenshot, m5-build.log, m5-qemu.log, GDB evidence]` |

Kriteria tambahan untuk praktikum lanjutan:

| Kriteria lanjutan | Status | Evidence |
|---|---|---|
| Static analysis dijalankan | `[NA]` | `[Tidak menggunakan cppcheck atau clang-tidy pada M5]` |
| Stress test dijalankan | `[NA]` | `[Tidak dilakukan pada ruang lingkup M5]` |
| Fuzzing atau malformed-input test dijalankan | `[NA]` | `[Tidak dilakukan pada ruang lingkup M5]` |
| Fault injection dijalankan | `[NA]` | `[Tidak dilakukan pada ruang lingkup M5]` |
| Disassembly/readelf evidence tersedia | `[PASS]` | `[build/disassembly.txt, build/readelf-header.txt, build/readelf-sections.txt, build/readelf-program-headers.txt]` |
| Review keamanan dilakukan | `[PASS]` | `[Bab 17 Keamanan dan Reliability]` |
| Rollback diuji | `[NA]` | `[Rollback dianalisis tetapi tidak diuji secara eksplisit]` |

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
[Status "Siap uji QEMU" dipilih karena seluruh bukti minimum praktikum M5 telah tersedia dan berhasil diverifikasi. Build dapat direproduksi dari kondisi bersih menggunakan make clean && make grade, kernel ELF berhasil dikompilasi dan dilink tanpa error, serta artefak audit seperti readelf, symbol table, undefined symbol audit, dan disassembly berhasil dihasilkan.
Pengujian runtime menggunakan QEMU menunjukkan bahwa kernel berhasil boot, IDT berhasil dimuat, PIC berhasil diremap ke rentang 0x20–0x2F, PIT berhasil dikonfigurasi, interrupt berhasil diaktifkan, dan timer interrupt menghasilkan tick periodik yang dapat diamati melalui serial log. Selain itu, debugging menggunakan GDB berhasil mencapai breakpoint pada fungsi kmain(), membuktikan bahwa simbol debug sesuai dengan kernel yang dijalankan.
Meskipun demikian, hasil ini belum layak disebut "Siap demonstrasi praktikum" maupun "Kandidat siap pakai terbatas" karena belum dilakukan stress test, fault injection terstruktur, validasi pada perangkat keras fisik, pengujian APIC/IOAPIC, pengujian SMP, maupun integrasi dengan scheduler preemptive. Oleh karena itu status yang paling tepat dan sesuai bukti adalah "Siap uji QEMU".]
```

Known issues:

| No. | Issue | Dampak | Workaround | Target perbaikan |
|---|---|---|---|---|
| 1 | `[Hanya IRQ0 yang aktif.]` | `[Perangkat lain belum dapat menghasilkan interrupt.]` | `[Tetap menggunakan baseline M5 dan fokus pada timer interrupt.]` | `[Milestone interrupt lanjutan.]` |
| 2 | `[Masih menggunakan legacy PIC dan PIT.]` | `[Belum mendukung mekanisme interrupt modern.]` | `[Menggunakan konfigurasi QEMU yang kompatibel.]` | `[Implementasi APIC/IOAPIC/LAPIC timer.]` |
| 3 | `[Belum ada scheduler preemptive.]` | `[Tick timer belum digunakan untuk penjadwalan proses.]` | `[Menggunakan idle loop sebagai workload.]` | `[Milestone scheduler.]` |
| 4 | `[Belum mendukung SMP.]` | `[Hanya berjalan pada konfigurasi single-core.]` | `[Menjalankan kernel pada lingkungan single-core.]` | `[Milestone SMP dan CPU startup.]` |
| 5 | `[Belum diuji pada hardware fisik.]` | `[Kompatibilitas perangkat keras belum diketahui.]` | `[Pengujian dilakukan pada QEMU.]` | `[Tahap validasi hardware di masa depan.]` |

Keputusan akhir:

```text
[Berdasarkan bukti build reproducibility, audit ELF, audit symbol, audit disassembly, serial log QEMU, dan debugging menggunakan GDB, hasil praktikum M5 layak disebut siap uji QEMU untuk jalur external interrupt dan PIT timer dasar. Hasil ini belum layak disebut siap demonstrasi praktikum tingkat lanjut maupun kandidat siap pakai terbatas karena belum dilakukan stress test, fault injection, validasi hardware fisik, integrasi scheduler, maupun pengujian SMP.]
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
[Praktikum M5 berhasil mengimplementasikan jalur external interrupt dasar pada kernel MCSOS x86_64. Kernel berhasil memuat IDT, melakukan remap legacy PIC ke rentang vector 0x20–0x2F, mengonfigurasi PIT channel 0 pada frekuensi 100 Hz, dan membuka IRQ0 sebagai sumber interrupt timer pertama.
Hasil implementasi berhasil diverifikasi melalui build reproducibility, audit ELF, audit symbol, audit disassembly, pengujian runtime menggunakan QEMU, dan debugging menggunakan GDB. Audit menunjukkan bahwa symbol penting seperti pic_remap, pit_configure_hz, timer_on_irq0, isr_stub_32, dan x86_64_trap_dispatch tersedia serta tidak terdapat unresolved external symbol.
Pada pengujian runtime, kernel berhasil boot di QEMU, menampilkan log inisialisasi PIC dan PIT, mengaktifkan interrupt, serta menghasilkan tick timer periodik yang dapat diamati melalui serial log. Selain itu, debugging menggunakan GDB berhasil mencapai breakpoint pada fungsi kmain(), membuktikan bahwa simbol debug sesuai dengan binary yang dijalankan.
Berdasarkan seluruh evidence yang tersedia, kriteria minimum praktikum M5 berhasil dipenuhi dan hasil implementasi dapat dinyatakan siap uji QEMU untuk jalur external interrupt dan timer dasar.]
```

### 22.2 Yang Belum Berhasil

```text
[Implementasi M5 masih memiliki beberapa keterbatasan yang memang berada di luar ruang lingkup praktikum. Sistem masih menggunakan legacy PIC dan PIT sehingga belum memanfaatkan APIC, IOAPIC, maupun LAPIC timer yang umum digunakan pada sistem modern.
Selain itu, hanya IRQ0 yang dibuka pada baseline M5, sementara interrupt perangkat lain masih tetap dimask. Tick timer yang dihasilkan juga belum digunakan oleh scheduler preemptive karena subsistem penjadwalan belum diimplementasikan.
Pengujian masih terbatas pada lingkungan QEMU dan belum mencakup perangkat keras fisik, SMP, stress test, fault injection terstruktur, maupun validasi performa yang lebih mendalam. Oleh karena itu hasil praktikum belum dapat diklaim siap produksi ataupun siap digunakan pada berbagai konfigurasi hardware.]
```

### 22.3 Rencana Perbaikan

```text
[Tahap pengembangan berikutnya adalah memanfaatkan tick timer sebagai dasar implementasi scheduler sehingga interrupt periodik dapat digunakan untuk mendukung mekanisme penjadwalan proses dan preemption.
Selain itu, sistem perlu diperluas dengan dukungan interrupt tambahan di luar IRQ0, pengujian failure mode yang lebih komprehensif, serta validasi menggunakan stress test dan fault injection. Pada tahap lanjutan, penggunaan legacy PIC dan PIT dapat digantikan oleh APIC, IOAPIC, dan LAPIC timer agar lebih sesuai dengan arsitektur sistem modern.
Pengembangan berikutnya juga perlu mencakup dukungan SMP, pengujian pada perangkat keras fisik, peningkatan keamanan jalur interrupt, serta dokumentasi dan evidence yang lebih lengkap untuk memastikan kualitas dan keandalan kernel terus meningkat.]
```

---

## 23. Lampiran

### Lampiran A — Commit Log

```text
1f955e7 (HEAD -> praktikum/m5-timer-irq, tag: m5-enrichment-stable, origin/praktikum/m5-timer-irq) M5: add unexpected IRQ diagnostics
4a76ef9 M5: add common types header
9604535 (tag: m5-stable) M5: implement PIC, PIT, and IRQ0 timer interrupts
5d467ac M5: add IRQ 32-47 ISR stubs
82ccae6 (origin/m4-idt-exception-path, m4-idt-exception-path) M4 add enhanced trap diagnostics and fault-int3 target
```

### Lampiran B — Diff Ringkas

```diff
+ kernel/arch/x86_64/pic.c
+ kernel/arch/x86_64/pit.c
+ kernel/include/pic.h
+ kernel/include/pit.h
+ kernel/include/types.h

* kernel/arch/x86_64/isr.S
  + Menambahkan ISR stub untuk vector IRQ 32–47

* kernel/core/trap.c
  + Menambahkan penanganan IRQ0 timer
  + Menambahkan pengiriman EOI
  + Memisahkan exception dan external interrupt

* kernel/core/kmain.c
  + Inisialisasi PIC
  + Inisialisasi PIT
  + Aktivasi interrupt menggunakan sti
```

### Lampiran C — Log Build Lengkap

```text
Path log build lengkap:

evidence/m5-build.log

Artefak build terkait:

build/kernel.elf
build/mcsos-m5.elf
build/kernel.map
build/disassembly.txt
build/symbols.txt
build/undefined.txt
build/readelf-header.txt
build/readelf-sections.txt
build/readelf-program-headers.txt
```

### Lampiran D — Log QEMU Lengkap

```text
Path log runtime lengkap:

evidence/m5-qemu.log

Cuplikan penting:

[M4] IDT loaded
[M4] selftest: IDT invariants passed

[M5] remapping PIC
[M5] configuring PIT

[M5] enabling interrupts
[M5] timer IRQ online

[MCSOS:TIMER] ticks=count=0x64
[MCSOS:TIMER] ticks=count=0xc8
[MCSOS:TIMER] ticks=count=0x12c
```

### Lampiran E — Output Readelf/Objdump

```text
ELF Header

Class: ELF64
Machine: Advanced Micro Devices X86-64
Type: EXEC (Executable file)
Entry point address: 0xffffffff800006a0

Symbol Verification

pic_remap
pit_configure_hz
timer_on_irq0
x86_64_trap_dispatch
isr_stub_32

Undefined Symbol Audit

(no undefined symbol)

Critical Instruction Verification

lidt
iretq
outb
sti
hlt

Artefak lengkap tersedia pada:

build/readelf-header.txt
build/readelf-sections.txt
build/readelf-program-headers.txt
build/disassembly.txt
build/symbols.txt
build/undefined.txt
```

### Lampiran F — Screenshot

| No. | File | Keterangan |
|---|---|---|
| 1 | `evidence/screenshots/build-success.png` | `Build kernel M5 berhasil menggunakan make clean dan make grade.` |
| 2 | `evidence/screenshots/qemu-timer-log.png` | `Kernel berhasil melakukan remap PIC, konfigurasi PIT 100 Hz, mengaktifkan IRQ0, dan menghasilkan timer tick periodik.` |
| 3 | `evidence/screenshots/gdb-kernel-elf.png` | `Verifikasi simbol kernel menggunakan nm menunjukkan fungsi kmain tersedia pada kernel ELF.` |


### Lampiran G — Bukti Tambahan

```text
Build Evidence
--------------
evidence/m5-build.log

Runtime Evidence
----------------
evidence/m5-qemu.log

ELF Audit Evidence
------------------
build/readelf-header.txt
build/readelf-sections.txt
build/readelf-program-headers.txt

Symbol Audit Evidence
---------------------
build/symbols.txt
build/undefined.txt

Disassembly Evidence
--------------------
build/disassembly.txt

Git Evidence
------------
Branch:
praktikum/m5-timer-irq

Tag:
m5-stable
m5-enrichment-stable

Commit:
1f955e7  M5: add unexpected IRQ diagnostics
4a76ef9  M5: add common types header
9604535  M5: implement PIC, PIT, and IRQ0 timer interrupts

Debug Evidence
--------------
Breakpoint berhasil dipasang pada:
- kmain
- pic_remap
- pit_configure_hz
- x86_64_trap_dispatch

Readiness Evidence
------------------
- IDT loaded
- PIC remapped to 0x20–0x2F
- IRQ0 unmasked
- PIT configured to 100 Hz
- Periodic timer tick observed
- Undefined symbol audit passed
- Critical instruction audit passed (lidt, iretq, outb, sti, hlt)
```

---

## 24. Daftar Referensi

Gunakan format IEEE. Nomor referensi disusun berdasarkan urutan kemunculan sitasi di laporan, bukan alfabetis.

```text
[1] R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces. Madison, WI, USA: Arpaci-Dusseau Books, 2018. [Online]. Available: https://pages.cs.wisc.edu/~remzi/OSTEP/. Accessed: 30-May-2026.
[2] Intel Corporation, Intel 64 and IA-32 Architectures Software Developer’s Manual, Combined Volumes 1–4. [Online]. Available: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html. Accessed: 30-May-2026.
[3] Advanced Micro Devices, AMD64 Architecture Programmer’s Manual, Volumes 1–5. [Online]. Available: https://www.amd.com/en/support/tech-docs/amd64-architecture-programmers-manual-volumes-1-5. Accessed: 30-May-2026.
[4] Limine Bootloader Project, “Limine Boot Protocol and Bootloader Documentation.” [Online]. Available: https://github.com/limine-bootloader/limine. Accessed: 30-May-2026.
[5] R. Cox, F. Kaashoek, and R. Morris, “xv6: a simple, Unix-like teaching operating system,” MIT PDOS. [Online]. Available: https://pdos.csail.mit.edu/6.828/2023/xv6.html. Accessed: 30-May-2026.
[6] OSDev Community, “Interrupt Descriptor Table (IDT).” [Online]. Available: https://wiki.osdev.org/Interrupt_Descriptor_Table. Accessed: 30-May-2026.
[7] OSDev Community, “8259 PIC.” [Online]. Available: https://wiki.osdev.org/8259_PIC. Accessed: 30-May-2026.
[8] OSDev Community, “Programmable Interval Timer.” [Online]. Available: https://wiki.osdev.org/Programmable_Interval_Timer. Accessed: 30-May-2026.
[9] GNU Project, “GNU Debugger (GDB) Documentation.” [Online]. Available: https://www.gnu.org/software/gdb/documentation/. Accessed: 30-May-2026.
[10] QEMU Project, “QEMU System Emulator Documentation.” [Online]. Available: https://www.qemu.org/docs/master/. Accessed: 30-May-2026.
```

---

## 25. Checklist Final Sebelum Pengumpulan

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
[1f955e7]
```

Status akhir yang diklaim:

```text
[Siap uji QEMU]
```

Ringkasan satu paragraf:

```text
[Praktikum M5 berhasil mengimplementasikan jalur external interrupt dasar pada kernel MCSOS x86_64 melalui pemasangan IDT, remap legacy PIC ke rentang vector 0x20–0x2F, konfigurasi PIT channel 0 pada frekuensi 100 Hz, serta aktivasi IRQ0 sebagai sumber timer interrupt pertama. Keberhasilan implementasi dibuktikan melalui build reproducibility, audit ELF, audit symbol, audit disassembly, pengujian runtime menggunakan QEMU, dan debugging menggunakan GDB. Serial log menunjukkan bahwa kernel berhasil boot, PIC dan PIT berhasil diinisialisasi, interrupt berhasil diaktifkan, serta tick timer periodik muncul secara konsisten. Meskipun demikian, implementasi masih terbatas pada lingkungan QEMU, belum mendukung APIC, IOAPIC, SMP, scheduler preemptive, maupun pengujian pada hardware fisik. Oleh karena itu hasil praktikum ini dinyatakan memenuhi kriteria minimum M5 dan layak diklaim berada pada status "Siap uji QEMU" sebagai fondasi untuk pengembangan subsistem interrupt dan scheduler pada milestone berikutnya.]
```

## 27. Pertanyaan Analisis

### 1. Mengapa IRQ legacy tidak boleh tetap berada pada vector historis yang dapat bertabrakan dengan exception CPU?

```text
[Pada arsitektur x86, exception CPU menggunakan vector 0–31. Sementara itu, PIC legacy secara historis mengirim IRQ pada vector yang dapat bertabrakan dengan rentang tersebut. Jika tidak dilakukan remap, kernel akan kesulitan membedakan apakah sebuah interrupt berasal dari perangkat keras atau dari exception CPU.
Sebagai contoh, sebuah IRQ timer dapat terlihat seperti exception tertentu sehingga diagnosis menjadi salah. Oleh karena itu PIC diremap ke rentang 0x20–0x2F agar seluruh IRQ berada di luar rentang exception CPU dan dapat ditangani secara konsisten oleh dispatcher.]
```

### 2. Mengapa sti harus dipanggil setelah IDT, PIC, dan PIT siap?

```text
[Instruksi sti mengaktifkan interrupt maskable pada CPU. Jika sti dipanggil terlalu awal sebelum IDT, PIC, atau PIT siap, interrupt dapat masuk ketika handler belum tersedia atau konfigurasi perangkat belum selesai.
Akibatnya kernel dapat mengalami panic, hang, atau bahkan triple fault. Oleh karena itu urutan yang benar adalah memuat IDT terlebih dahulu, mengonfigurasi PIC dan PIT, kemudian baru mengaktifkan interrupt menggunakan sti.]
```

### 3. Apa konsekuensi jika handler IRQ0 lupa mengirim EOI?

```text
[EOI (End Of Interrupt) memberi tahu PIC bahwa interrupt saat ini telah selesai diproses. Jika EOI tidak dikirim, PIC akan menganggap interrupt masih aktif dan tidak mengirim interrupt berikutnya.
Akibatnya timer hanya menghasilkan satu atau beberapa tick awal lalu berhenti. Sistem kehilangan sumber waktu periodik sehingga fungsi yang bergantung pada timer tidak dapat berjalan dengan benar.]
```

### 4. Apa perbedaan exception dengan error code dan exception tanpa error code pada layout stack?

```text
[Beberapa exception CPU secara otomatis mendorong error code ke stack, sedangkan exception lain tidak. Akibatnya layout stack yang diterima handler menjadi berbeda.
Untuk menyederhanakan penanganan, ISR biasanya membuat format trap frame yang seragam dengan menambahkan dummy error code pada exception yang tidak memilikinya. Dengan cara ini dispatcher dapat memproses seluruh exception menggunakan struktur data yang konsisten.]
```

### 5. Mengapa volatile dipakai untuk g_ticks, tetapi tidak cukup untuk sinkronisasi SMP jangka panjang?

```text
[volatile memberi tahu compiler agar tidak menghapus atau mengoptimalkan akses terhadap variabel karena nilainya dapat berubah di luar alur program normal.
Namun volatile tidak menjamin atomicity, ordering, maupun sinkronisasi antar CPU. Pada sistem SMP, beberapa CPU dapat mengakses variabel yang sama secara bersamaan sehingga tetap diperlukan atomic operation, memory barrier, atau mekanisme sinkronisasi lain. Karena itu volatile cukup untuk M5 yang masih sederhana, tetapi tidak memadai untuk kernel SMP modern.]
```

### 6. Mengapa PIT/PIC masih dipakai pada M5 meskipun APIC/HPET/LAPIC timer lebih relevan untuk sistem modern?

```text
[PIT dan PIC dipilih karena lebih sederhana, terdokumentasi dengan baik, dan tersedia hampir di seluruh emulator seperti QEMU. Tujuan M5 adalah membangun jalur interrupt dasar terlebih dahulu sebelum masuk ke perangkat keras yang lebih kompleks.
Dengan menggunakan PIT dan PIC, mahasiswa dapat memahami konsep fundamental interrupt, remap vector, timer periodik, dan EOI sebelum beralih ke APIC, IOAPIC, HPET, atau LAPIC timer pada tahap berikutnya.]
```

### 7. Bagaimana cara membuktikan bahwa kernel tidak menarik dependency libc host?

```text
[Bukti dapat diperoleh melalui konfigurasi build dan audit binary. Kernel dibangun menggunakan opsi seperti -ffreestanding dan -nostdlib sehingga tidak bergantung pada runtime host.
Selain itu, audit symbol menggunakan nm -u dapat menunjukkan bahwa tidak ada symbol eksternal dari libc host yang belum terdefinisi. Jika kernel berhasil dilink dan undefined symbol audit bersih, maka dapat disimpulkan bahwa kernel tidak menarik dependency libc host.]
```

### 8. Bagaimana QEMU gdbstub membantu membedakan hang sebelum sti dan hang setelah interrupt aktif?

```text
[QEMU gdbstub memungkinkan GDB berhenti pada breakpoint tertentu dan memeriksa posisi eksekusi kernel. Jika kernel hang sebelum mencapai instruksi sti, penyebabnya kemungkinan berada pada tahap inisialisasi awal seperti IDT, PIC, atau PIT.
Sebaliknya, jika kernel berhasil melewati sti dan hang setelah interrupt aktif, penyebabnya lebih mungkin berada pada handler interrupt, trap dispatcher, konfigurasi PIC, EOI, atau jalur timer. Dengan demikian GDB membantu mempersempit lokasi masalah secara sistematis.]
```

### 9. Risiko keamanan apa yang muncul jika semua IRQ dibuka sebelum driver tersedia?

```text
[Membuka seluruh IRQ sebelum driver dan handler siap dapat menyebabkan interrupt tak terduga masuk ke kernel. Interrupt tersebut mungkin tidak memiliki handler yang valid atau belum memiliki mekanisme validasi yang memadai.
Akibatnya sistem dapat mengalami interrupt storm, panic, hang, atau perilaku yang tidak terprediksi. Karena itu M5 hanya membuka IRQ0 dan tetap mem-mask IRQ lain sampai subsistem yang sesuai benar-benar siap digunakan.]
```

### 10. Bagaimana M5 menjadi fondasi untuk scheduler tick atau clocksource pada praktikum berikutnya?

```text
[M5 memperkenalkan sumber waktu periodik melalui IRQ0 timer. Setiap interrupt timer meningkatkan nilai tick sehingga kernel memiliki referensi waktu dasar yang terus bertambah.
Pada praktikum berikutnya, tick ini dapat digunakan untuk mengukur waktu tunggu, menghitung quantum proses, melakukan preemption, membangunkan task yang sedang sleep, dan membangun scheduler berbasis waktu. Oleh karena itu M5 menyediakan fondasi penting bagi subsistem scheduler dan manajemen waktu kernel.]
```

## 28. Readiness Review M5

| Area | Status yang Diharapkan | Bukti |
|---|---|---|
| Build reproducibility | `[Siap uji statis]` | `[make clean && make grade lulus]` |
| Interrupt entry | `[Siap uji QEMU]` | `[Instruksi lidt, iretq, dan symbol isr_stub_32 terbukti melalui audit disassembly dan symbol table]` |
| PIC/PIT | `[Siap uji QEMU]` | `[Symbol pic_remap dan pit_configure_hz tersedia serta log konfigurasi muncul pada serial log]` |
| Runtime tick | `[Siap demonstrasi praktikum jika QEMU log menunjukkan tick]` | `[Log menunjukkan ticks=count=0x64, ticks=count=0xc8, dan seterusnya]` |
| Security baseline | `[Kandidat terbatas untuk tahap awal]` | `[IRQ selain IRQ0 tetap masked sesuai baseline M5]` |
| Hardware fisik | `[Belum siap]` | `[Belum ada APIC, IOAPIC, HPET, maupun hardware compatibility matrix]` |
| Scheduler integration | `[Belum siap]` | `[Tick timer belum digunakan untuk preemption atau scheduler]` |

Kesimpulan readiness:

```text
[Seluruh kriteria utama M5 berhasil dipenuhi. Build dapat direproduksi dari kondisi bersih, audit ELF dan disassembly berhasil membuktikan keberadaan jalur interrupt, PIC berhasil diremap ke rentang 0x20–0x2F, PIT berhasil dikonfigurasi, dan timer interrupt menghasilkan tick periodik yang dapat diamati pada serial log QEMU.
Berdasarkan bukti tersebut, hasil praktikum ini layak diklaim berada pada status "Siap uji QEMU" untuk jalur external interrupt dan PIT timer dasar. Namun hasil ini belum dapat dianggap sebagai timer production-grade karena masih menggunakan legacy PIC/PIT, belum mendukung APIC/IOAPIC/HPET, belum mendukung SMP, belum diuji pada hardware fisik umum, dan belum terintegrasi dengan scheduler preemptive. Oleh karena itu readiness tertinggi yang dapat diklaim secara akademik adalah "Siap uji QEMU".]
```

















