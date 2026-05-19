#!/usr/bin/env bash

if command -v x86_64-elf-gcc >/dev/null 2>&1; then
    echo "OK: x86_64-elf-gcc $(command -v x86_64-elf-gcc)"
else
    echo "INFO: x86_64-elf-gcc not installed, using clang fallback"
fi
