#!/usr/bin/env bash
set -euo pipefail

echo "[CI] checking source tree"
make check-src

echo "[CI] checking scripts"
make check-scripts

echo "[CI] building kernel"
make build

echo "[CI] inspecting ELF"
make inspect

echo "[CI] building ISO"
make image

echo "[CI] running local grade"
make grade

echo "[CI] all checks passed"
