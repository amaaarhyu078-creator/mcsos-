#!/usr/bin/env bash
set -euo pipefail

score=0

make clean >/dev/null
make audit >/dev/null

score=$((score + 60))

tools/scripts/m4_audit_elf.sh build/kernel.elf >/dev/null

score=$((score + 20))

cp build/kernel.elf iso_root/boot/kernel.elf >/dev/null 2>&1 || true

xorriso -as mkisofs \
  -b boot/limine/limine-bios-cd.bin \
  -no-emul-boot \
  -boot-load-size 4 \
  -boot-info-table \
  --efi-boot boot/limine/limine-uefi-cd.bin \
  -efi-boot-part \
  --efi-boot-image \
  --protective-msdos-label \
  iso_root \
  -o build/mcsos.iso >/dev/null 2>&1

tools/scripts/m4_qemu_run.sh >/dev/null 2>&1 || true

if [[ -f build/m4-qemu-serial.log ]]; then
    grep -q '\[M4\]' build/m4-qemu-serial.log \
        && score=$((score + 10))
fi

[[ -f evidence/M4/manifest.txt ]] \
    && score=$((score + 10))

echo "M4_LOCAL_SCORE=$score/100"
