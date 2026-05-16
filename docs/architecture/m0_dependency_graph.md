# M0 Dependency Graph — MCSOS 260502

```mermaid
graph TD

    A[Windows 11 Host]
    B[WSL2 Linux Environment]
    C[Repository ~/src/mcsos]

    D[tools/check_env.sh]
    E[Makefile]
    F[smoke/freestanding.c]

    G[Clang LLVM]
    H[Binutils]
    I[QEMU]
    J[GDB]

    K[build/meta/toolchain-versions.txt]
    L[build/smoke/freestanding.o]
    M[build/evidence/M0]

    N[Requirements]
    O[ADR]
    P[Threat Model]
    Q[Risk Register]
    R[Verification Matrix]
    S[M0 Report]

    A --> B
    B --> C

    C --> D
    C --> E
    C --> F

    D --> K

    E --> G
    E --> H
    E --> F

    G --> L
    H --> L

    E --> M
    D --> M
    L --> M

    C --> N
    C --> O
    C --> P
    C --> Q
    C --> R
    C --> S

    I --> M
    J --> M
