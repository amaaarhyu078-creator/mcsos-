# Readiness Review M1 - Toolchain Reproducible

## Identitas

- Nama mahasiswa/kelompok: Salma Rahayu
- NIM anggota: 2583207073007
- Kelas: PTI 1A
- Dosen: Muhaemin Sidiq, S.Pd., M.Pd.
- Program Studi: Pendidikan Teknologi Informasi, Institut Pendidikan Indonesia
- Tanggal: 18 Mei 2026
- Commit hash:

## Ringkasan hasil

M1 berhasil dijalankan dengan seluruh acceptance criteria terpenuhi. Toolchain, QEMU, OVMF, dan proof freestanding ELF berhasil diverifikasi. Build reproducibility juga berhasil dibuktikan menggunakan SHA256 hash yang identik antar build. Lingkungan dinyatakan siap untuk M2.

## Evidence checklist

| Evidence | Path | Status | Catatan |
|---|---|---|---|
| Toolchain versions | `build/meta/toolchain-versions.txt` | OK | Toolchain terdeteksi lengkap |
| Host readiness | `build/meta/host-readiness.txt` | OK | Repository berada di filesystem Linux WSL |
| QEMU capabilities | `build/meta/qemu-capabilities.txt` | OK | q35 dan OVMF tersedia |
| Freestanding object | `build/proof/freestanding_probe.o` | OK | ELF64 relocatable x86_64 |
| Freestanding ELF | `build/proof/freestanding_probe.elf` | OK | ELF64 executable x86_64 |
| ELF header | `build/proof/readelf-header.txt` | OK | Machine: AMD X86-64 |
| ELF sections | `build/proof/readelf-sections.txt` | OK | Section valid |
| Disassembly | `build/proof/objdump-disassembly.txt` | OK | Disassembly berhasil |
| Undefined symbol report | `build/proof/nm-undefined.txt` | OK | Tidak ada undefined symbol |
| Reproducibility hash | `build/repro/sha256-run1.txt`, `build/repro/sha256-run2.txt` | OK | Hash identik |

## Acceptance criteria M1

| Kriteria | Lulus/Gagal | Bukti |
|---|---|---|
| Repository berada di filesystem Linux WSL | Lulus | `/home/salma_rahayu/src/mcsos` |
| Semua tool wajib tersedia | Lulus | `check_toolchain.sh` |
| `make meta` berhasil | Lulus | Metadata berhasil dibuat |
| `make check` berhasil | Lulus | Semua tool tervalidasi |
| `make proof` berhasil | Lulus | Proof ELF berhasil dibuat |
| `make qemu-probe` berhasil | Lulus | q35 dan OVMF terdeteksi |
| `make repro` berhasil | Lulus | Hash reproducible identik |
| `make test` berhasil dari clean checkout | Lulus | `OK: M1 test suite passed` |
| `nm-undefined.txt` kosong | Lulus | Tidak ada undefined symbol |
| Hasil `readelf` menunjukkan ELF64 x86_64 | Lulus | Machine AMD X86-64 |

## Known limitations

1. Belum menggunakan cross compiler khusus seperti `x86_64-elf-gcc`.
2. Belum memiliki CI/CD pipeline otomatis.
3. Belum ada boot image kernel aktual.
4. Belum ada pengujian pada hardware fisik.
5. Build masih terbatas pada proof freestanding sederhana.

## Risiko dan mitigasi

1. Risiko ketidaksesuaian toolchain host dengan target kernel.  
   Mitigasi: menggunakan `readelf`, `objdump`, dan validasi target ELF.

2. Risiko dependency terhadap hosted libc atau runtime host.  
   Mitigasi: menggunakan mode freestanding dan `-nostdlib`.

3. Risiko build tidak reproducible akibat perubahan lingkungan.  
   Mitigasi: pencatatan versi toolchain dan reproducibility hash.

## Readiness decision

- [ ] Belum siap lanjut M2.
- [ ] Siap lanjut M2 dengan catatan.
- [x] Siap lanjut M2.

Alasan keputusan:

Seluruh acceptance criteria M1 telah terpenuhi, seluruh script valid berjalan tanpa error, reproducibility proof berhasil diverifikasi, dan lingkungan build siap digunakan untuk milestone M2.

