#!/usr/bin/env bash
set -euo pipefail

printf '[M8] checking repository baseline...\n'

required=(
  kernel/include/mcsos/kernel/kmem.h
  kernel/core/kmem.c
  tests/test_kmem_host.c
  Makefile
)

for f in "${required[@]}"; do
  if [[ ! -f "$f" ]]; then
    printf '[FAIL] missing %s\n' "$f" >&2
    exit 1
  fi
done

printf '[M8] checking toolchain...\n'

command -v clang >/dev/null
command -v nm >/dev/null
command -v objdump >/dev/null
command -v readelf >/dev/null
command -v make >/dev/null

printf '[M8] tool versions...\n'

clang --version | head -n 1
ld.lld --version 2>/dev/null | head -n 1 || true
make --version | head -n 1

printf '[M8] running make check-m8...\n'
make check-m8

printf '[M8] running make m8-audit...\n'
make m8-audit

test -f build/m8/kmem.freestanding.o
test -f build/m8/kmem.objdump.txt
test -f build/m8/readelf_h.txt
test -f build/m8/test_kmem.log
test -f build/m8/nm_u.txt

if [[ -s build/m8/nm_u.txt ]]; then
    printf '[FAIL] unresolved symbols found\n' >&2
    exit 1
fi

grep -q 'PASS' build/m8/test_kmem.log

printf '[PASS] M8 preflight completed.\n'
