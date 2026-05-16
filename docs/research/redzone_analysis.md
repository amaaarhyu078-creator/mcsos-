# Analisis Red Zone

## Percobaan

File source yang digunakan:

```c
unsigned long test(unsigned long a) {
    unsigned long x = a + 1;
    return x;
}
```

Dilakukan dua kali proses compile:

1. Tanpa `-mno-red-zone`
2. Dengan `-mno-red-zone`

---

## Perbandingan Assembly

### Tanpa `-mno-red-zone`

```asm
push %rbp
mov %rsp,%rbp
mov %rdi,-0x8(%rbp)
...
pop %rbp
ret
```

Pengamatan:

* Tidak ada instruksi alokasi stack tambahan.
* Compiler menggunakan area red zone di bawah register RSP.

---

### Dengan `-mno-red-zone`

```asm
push %rbp
mov %rsp,%rbp
sub $0x10,%rsp
...
add $0x10,%rsp
pop %rbp
ret
```

Pengamatan:

* Terdapat alokasi stack secara eksplisit.
* Compiler tidak menggunakan area red zone.

---

## Analisis

Pada arsitektur x86-64 terdapat area bernama red zone sebesar 128 byte di bawah stack pointer (RSP). Area ini dapat digunakan compiler untuk menyimpan data sementara tanpa perlu mengubah nilai RSP.

Penggunaan red zone aman pada program user-space biasa, tetapi tidak aman pada kernel atau sistem operasi karena interrupt dan exception dapat menimpa area tersebut.

Oleh karena itu kernel biasanya menggunakan:

```text
-mno-red-zone
```

agar compiler selalu membuat alokasi stack yang aman.

---

## Kesimpulan

Penggunaan `-mno-red-zone` membuat kode sedikit lebih panjang karena ada proses alokasi stack tambahan, tetapi lebih aman untuk pengembangan kernel dan sistem operasi karena menghindari kerusakan data akibat interrupt.

