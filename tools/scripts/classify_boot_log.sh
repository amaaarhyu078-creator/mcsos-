#!/usr/bin/env bash
set -euo pipefail

LOG="${1:-build/qemu-serial.log}"

if [ ! -f "$LOG" ]; then
    echo "ERROR: log file not found"
    exit 1
fi

if grep -q "kernel reached controlled halt loop" "$LOG"; then
    echo "CLASSIFICATION: SUCCESSFUL KERNEL ENTRY"
    exit 0
fi

if grep -q "Loading executable" "$LOG"; then
    echo "CLASSIFICATION: KERNEL ENTRY FAILURE"
    exit 0
fi

if grep -qi "limine" "$LOG"; then
    echo "CLASSIFICATION: BOOTLOADER FAILURE"
    exit 0
fi

echo "CLASSIFICATION: FIRMWARE FAILURE"
