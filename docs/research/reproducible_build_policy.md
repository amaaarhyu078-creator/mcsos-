# Policy Reproducible Build Kernel ELF

## Tujuan

Reproducible build bertujuan memastikan hasil build kernel selalu identik walaupun dilakukan pada waktu atau mesin yang berbeda.

---

## Risiko

Tanpa reproducible build:

* hash binary dapat berubah
* debugging menjadi sulit
* validasi integritas kernel tidak konsisten
* supply-chain attack lebih sulit dideteksi

---

## Policy yang Digunakan

### 1. Kontrol Timestamp

Build tidak boleh menyimpan timestamp dinamis.

Gunakan:

```text id="v1d7xr"
SOURCE_DATE_EPOCH
```

agar timestamp build tetap konsisten.

---

### 2. Kontrol Debug Path

Path absolut host tidak boleh masuk ke binary debug.

Gunakan:

```text id="z8n4pl"
-fdebug-prefix-map
```

untuk mengganti path lokal menjadi path virtual yang konsisten.

---

### 3. Toolchain Tetap

Versi compiler, linker, assembler, dan utility harus dicatat.

Contoh:

* Clang 21
* Binutils 2.45
* GCC cross compiler 16.1.0

---

### 4. Build Environment Konsisten

WSL, distro Linux, dan package utama harus terdokumentasi.

---

### 5. Build Flags Tetap

Semua build kernel harus memakai flag yang sama agar hasil object konsisten.

Contoh:

```text id="w6f3cy"
-ffreestanding
-mno-red-zone
-fno-stack-protector
```

---

## Kesimpulan

Reproducible build penting untuk keamanan, debugging, validasi integritas, dan stabilitas pengembangan kernel sistem operasi.
