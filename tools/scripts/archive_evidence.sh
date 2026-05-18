#!/usr/bin/env bash
set -euo pipefail

mkdir -p release

tar -czf release/m1-evidence.tar.gz \
    build/meta \
    build/proof \
    build/repro \
    docs/readiness \
    docs/architecture \
    docs/security

echo "OK: evidence archived to release/m1-evidence.tar.gz"
