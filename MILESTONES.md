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

## [M1] Baseline Cryptography & Integration ✅ RELEASED 2026-02-19

**Goal:** Integrate standard RSA and PQC candidates (ML-DSA-44/ML-KEM-512) and establish benchmarking primitives.

- [x] **Cryptographic Libraries**:
    - [x] Integrate custom 2048-bit BigInt for classical RSA baseline
    - [x] Integrate ML-DSA-44 and ML-KEM-512 from PQClean
- [x] **Benchmarking Harness**:
    - [x] Implement DWT cycle counting in `cycles.c`
    - [x] Create a unified `benchmark_pqc` and `benchmark_rsa` interface
- [x] **First Comparison**:
    - [x] Benchmark RSA-2048 Public Key Operation
    - [x] Benchmark ML-DSA-44 (Sign/Verify) and ML-KEM-512 (Encaps/Decaps)
    - [x] Output cycle counts to UART in CSV-ready format

**Success Criteria:** ✅ MET
- Comparative performance data captured for classical vs. PQC algorithms.
- RSA-2048 (~152M cycles) vs ML-DSA-44 Keygen (~15M cycles) baseline established.
- Stable execution on hardware with 32KB stack allocation.

---

## [M2] Optimization & Extended Analysis ✅ RELEASED 2026-02-19
**Goal:** Optimize implementations for the Cortex-M7 and produce publication-ready data.

- [ ] **Memory Profiling**:
    - [x] **Milestone 2: Optimization & Extended Analysis** — Released 2026-02-19
    - [x] Implement memory profiling (Stack watermarking)
    - [x] Integrate Falcon-512 & SPHINCS+
    - [x] Finalize automated benchmarking report
- [ ] Add Kyber for KEM comparison (vs. RSA/ECC)
- [ ] **Documentation Generator**:
    - [ ] Python script to parse UART CSV and generate LaTeX tables/plots
    - [ ] Compile draft report PDF

**Success Criteria:** ✅ MET
- A single command runs the full benchmark suite, downloads results, and compiles a PDF comparing Cycle Count, Stack Usage, and ROM Size.