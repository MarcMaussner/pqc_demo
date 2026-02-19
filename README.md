# PQC Demo on STM32F769

Benchmarking Post-Quantum Cryptography (PQC) algorithms against classical RSA on the STM32F769I Discovery Kit, targeting automotive security contexts.

## Hardware

| Item | Detail |
|---|---|
| Board | STM32F769I-DISCO |
| MCU | ARM Cortex-M7 @ 216 MHz |
| Flash | 2 MB |
| SRAM | 512 KB |
| Debugger | On-board ST-Link V2 |
| UART | ST-Link VCP → COM5 @ 115200 |

## Quick Start

> **Prerequisites:** Podman installed, board connected via USB, `tools/` populated.

```powershell
# 1. Build the Docker build environment (once)
.\scripts\build_container.ps1

# 2. Compile firmware
.\scripts\run_build.ps1
# → generates firmware/build/pqc_demo.{elf,bin}

# 3. Flash to board
.\scripts\flash.bat
# → ** Verified OK ** → ** Resetting Target **

# 4. Read UART output
.\scripts\read_uart.ps1
# → UART >> PQC Demo: Environment Setup Successful
# → UART >> PQC Demo: Running...
```

## Project Structure

```
pqc_demo/
├── docker/                    # Container build environment
│   └── Dockerfile             #   Ubuntu 22.04 + ARM GCC 12.2 + CMake + Ninja
├── firmware/
│   ├── CMakeLists.txt         # Build configuration
│   ├── STM32F769NIHx_FLASH.ld # Linker script (2MB Flash / 512KB RAM)
│   ├── toolchain_container.cmake
│   ├── Core/
│   │   ├── Inc/               # Headers (stm32f7xx_hal_conf.h, etc.)
│   │   └── Src/               # main.c, syscalls.c, startup_stm32f769xx.s
│   ├── deps/
│   │   ├── CMSIS/             # ARM CMSIS core + ST device headers
│   │   └── STM32F7xx_HAL_Driver/
│   ├── PQC/                   # (M1) PQC algorithm implementations
│   └── RSA/                   # (M1) RSA baseline
├── scripts/
│   ├── build_container.ps1    # Build the pqc-build-env Docker image
│   ├── run_build.ps1          # Compile firmware in container
│   ├── flash.bat              # Flash via OpenOCD + ST-Link
│   └── read_uart.ps1          # Read UART output from COM5
├── docs/                      # Release notes, reports
├── tests/                     # Unit tests for crypto primitives
├── MILESTONES.md
└── README.md
```

## Milestone Status

| Milestone | Status | Date |
|---|---|---|
| M0: Environment Setup & Automation | ✅ Released | 2026-02-19 |
| M1: Baseline Cryptography & Integration | 🔲 Planned | — |
| M2: Optimization & Extended Analysis | 🔲 Planned | — |

## Benchmark Metrics (M1+)

- **Execution Time**: ARM DWT cycle counter
- **Memory Footprint**: Flash + SRAM (static), Stack (dynamic via canary)
- **Algorithms**: RSA-2048, Dilithium2, Kyber512, Falcon (M2)

## Quality Policy

- Zero GCC warnings on release builds
- All releases tagged in git with changelog
- Every release includes a verified build log and UART capture proof

---
*Automotive Security PQC Demonstration — safe.tech 2026*
