# Threat Model Supply-Chain Toolchain dan Bootloader

## Tujuan

Dokumen ini menjelaskan risiko supply-chain pada toolchain dan bootloader dalam pengembangan kernel sistem operasi.

---

## Ancaman Utama

### 1. Compiler Berbahaya

Compiler yang telah dimodifikasi dapat menyisipkan backdoor ke binary kernel tanpa terlihat pada source code.

Risiko:

* kernel terinfeksi malware
* privilege escalation
* sabotase build

Mitigasi:

* gunakan source resmi GNU/LLVM
* verifikasi checksum SHA256
* catat versi compiler

---

### 2. Source Archive Palsu

File source GCC, Binutils, atau bootloader dapat diganti oleh pihak ketiga.

Risiko:

* trojan pada toolchain
* binary hasil build tidak aman

Mitigasi:

* download dari website resmi
* verifikasi checksum
* gunakan HTTPS

---

### 3. Dependency Berbahaya

Dependency eksternal dapat mengandung kode berbahaya.

Contoh:

* library build
* package manager
* plugin compiler

Mitigasi:

* minimalkan dependency
* gunakan package terpercaya
* dokumentasikan dependency

---

### 4. Bootloader Compromise

Bootloader yang dimodifikasi dapat memuat kernel palsu atau memodifikasi memory sebelum kernel berjalan.

Mitigasi:

* audit source bootloader
* gunakan reproducible build
* verifikasi binary bootloader

---

### 5. Build Environment Tidak Konsisten

Perbedaan versi toolchain dan package dapat menghasilkan binary berbeda.

Risiko:

* bug sulit direproduksi
* validasi integritas gagal

Mitigasi:

* dokumentasikan environment
* gunakan reproducible build policy
* catat metadata toolchain

---

## Kesimpulan

Supply-chain security sangat penting dalam pengembangan kernel dan sistem operasi karena compiler, linker, bootloader, dan dependency memiliki akses penuh terhadap binary akhir yang dihasilkan.
