# Project Milestones

## [M0] Environment Setup & Automation ✅ RELEASED 2026-02-19

**Goal:** Establish a reproducible development environment and automated testing pipeline.

- [x] **Toolchain Setup**:
    - ARM GCC 12.2, CMake, Ninja, OpenOCD installed in `tools/`
    - MSYS2 Bash environment configured for Windows path compatibility
- [x] **Containerized Build**:
    - Ubuntu 22.04 container (`pqc-build-env`) with full toolchain
    - Solves Windows long-path / Umlaut encoding issues
    - `docker/Dockerfile` + `scripts/build_container.ps1`
- [x] **Firmware Base**:
    - STM32F769I-DISCO project structure (Core, Drivers, Startup)
    - HAL configuration with full `stm32f7xx_hal_conf.h` template
    - STM32F769NIHx linker script (2MB Flash / 512KB RAM)
    - Startup assembly with Cortex-M7 vector table
    - USART1 @ 115200 baud (PA9/PA10 → ST-Link VCP)
    - 216 MHz PLL clock configuration
- [x] **Automation**:
    - `scripts/run_build.ps1` — compiles in container, generates `.elf` + `.bin`
    - `scripts/flash.bat` — flashes via OpenOCD + ST-Link (TCL forward-slash fix)
    - `scripts/read_uart.ps1` — reads UART output from COM5

**Success Criteria:** ✅ MET
- `scripts/run_build.ps1` builds firmware (exit 0)
- `scripts/flash.bat` flashes and verifies (`** Verified OK **`)
- `scripts/read_uart.ps1` receives `PQC Demo: Environment Setup Successful` on COM5

---

## [M1] Baseline Cryptography & Integration
**Goal:** Integrate standard RSA and a PQC candidate (Dilithium/Kyber) and establish benchmarking primitives.

- [ ] **Cryptographic Libraries**:
    - [ ] Integrate a lightweight RSA implementation (e.g., MbedTLS or custom small footprint)
    - [ ] Integrate Dilithium2/Kyber512 from PQClean
- [ ] **Benchmarking Harness**:
    - [ ] Implement `cycles.c` using the ARM DWT cycle counter
    - [ ] Create a unified interface `crypto_sign(message, len, signature, &sig_len)`
- [ ] **First Comparison**:
    - [ ] Benchmark RSA-2048 Sign/Verify
    - [ ] Benchmark Dilithium2 Sign/Verify
    - [ ] Output raw cycle counts to UART in CSV format

**Success Criteria:** The firmware outputs a CSV block with cycle counts for both algorithms, verifiable by a host script.

---

## [M2] Optimization & Extended Analysis
**Goal:** Optimize implementations for the Cortex-M7 and produce publication-ready data.

- [ ] **Memory Profiling**:
    - [ ] Stack painting/canary checks for max stack usage
    - [ ] Analyze `.map` files for flash/SRAM footprint
- [ ] **Extended Algorithms**:
    - [ ] Add Falcon or SPHINCS+ for broader comparison
    - [ ] Add Kyber for KEM comparison (vs. RSA/ECC)
- [ ] **Documentation Generator**:
    - [ ] Python script to parse UART CSV and generate LaTeX tables/plots
    - [ ] Compile draft report PDF

**Success Criteria:** A single command runs the full benchmark suite, downloads results, and compiles a PDF comparing Cycle Count, Stack Usage, and ROM Size.