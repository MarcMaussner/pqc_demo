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

- [x] **Memory Profiling**:
    - [x] **Milestone 2: Optimization & Extended Analysis** — Released 2026-02-19
    - [x] Implement memory profiling (Stack watermarking)
    - [x] Integrate Falcon-512 & SPHINCS+
    - [x] Finalize automated benchmarking report
- [x] Add Kyber for KEM comparison (vs. RSA/ECC)
- [x] **Documentation Generator**:
    - [x] Python script to parse UART CSV and generate LaTeX tables/plots
    - [x] Compile draft report PDF

**Success Criteria:** ✅ MET
- A single command runs the full benchmark suite, downloads results, and compiles a PDF comparing Cycle Count, Stack Usage, and ROM Size.

---

## [M3] Final Reporting & Deployment ✅ RELEASED 2026-02-19
**Goal:** Transition from data collection to high-quality presentation and final project solidification.
- [x] **Improve Implementation**:
    - [x] Add Kyber for KEM comparison (vs. RSA/ECC)
    - [x] Ensure to take official RSA algorithm (mbedtls, openssl)
    - [x] Standalone script to run all benchmarks and generate report (with output of run sections)
- [x] **Advanced Visualization**:
    - [x] Generate comparative plots for Cycles and Stack usage.
    - [x] Create security-level vs. performance trade-off charts.
- [x] **Resource Analysis**:
    - [x] Automate Flash/SRAM footprint measurement per algorithm.
    - [x] Analyze overhead of PQClean vs. hardware-optimized versions (if applicable).
- [x] **Final Deliverables**:
    - [x] `docs/FINAL_REPORT.md` including all visualizations and analysis.
    - [x] "Demo Mode" in firmware for interactiveSuite selection.
    - [x] Final project tagging (`v1.0.0`).

**Success Criteria:** ✅ MET
- Professional-grade PDF/Markdown report generated automatically from hardware logs.
- Full footprint analysis (ROM/SRAM) documented for all 5 candidates.

---

## [M4] Operational Manual & RSA-2048 Benchmark ✅ RELEASED 2026-02-19
**Goal:** Improve usability with a dedicated manual and complete the classical baseline with RSA Private Key benchmarking.
- [x] **Usability**:
    - [x] Create `docs/RUN_DEMO_MANUAL.md` explaining how to use `run_demo.ps1` and interpret results.
- [x] **Benchmarking**:
    - [x] Benchmark RSA-2048 Public Key Operation
    - [x] Benchmark RSA-2048 Private Key Operation (~580M cycles with CRT, ~2.7 s)
    - [x] Benchmark RSA-2048 Key Generation (~2.3B cycles, ~10.7 s)
- [x] **Final Deliverables**:
    - [x] Updated `FINAL_REPORT.md` with RSA keygen/private key data.
    - [x] `v1.1.0` release.

**Success Criteria:** ✅ MET
- Users can run the demo without looking at source code scripts using the manual.
- RSA Private Key cycles are captured: Private Op is ~10x slower than ML-DSA-44 Sign.

---

## [M5] Hardware Optimization & Configuration Analysis
**Goal:** Integrate `pqm4` for hardware-optimized performance (simulating production-grade assembly) and detail the exact configuration of all algorithms.

- [x] **Hardware Optimization**:
    - [x] Integrate `pqm4` library (Cortex-M4/M7 optimized assembly) - *Kyber/Dilithium*.
    - [x] Benchmark `pqm4` variants of Kyber/Dilithium/Falcon/Sphincs+.
    - [x] Compare "Clean C" vs "Assembly" performance.
- [x] **Configuration Reporting**:
    - [x] Extend `FINAL_REPORT.md` with a "Configuration Parameters" section.
    - [x] Document Key Sizes (pk, sk, sig/ct) for all algorithms.
    - [x] Document NIST Security Levels and parameters (e.g., Kyber-512 vs 768).
- [x] **Final Deliverables**:
    - [x] Updated `FINAL_REPORT.md` with new `pqm4` data and config tables.
    - [x] `v1.2.0` release.

**Success Criteria:**
- Cycle counts for Kyber/Dilithium show >2x improvement over M4 baseline.
- Report clearly lists all cryptographic parameters (n, k, q, sizes).

- **Status:** Completed ✅ RELEASED 2026-02-20
- **Notes:**
    - `pqm4` integrated successfully.
    - Achieved **2.25x speedup** for Dilithium Signing (Assembly vs C).
    - Verified on STM32 hardware.
    - **Release v1.2.0** tagged.

---

## [M6] RSA Scalability Analysis (2048/3072/4096)
**Goal:** Benchmark classical RSA at higher security levels (3072-bit and 4096-bit) to demonstrate the cubic scaling cost compared to PQC.
**Status:** Completed ✅ RELEASED 2026-02-20
**Deliverables:**
*   [x] Updated harness causing cubic scaling for RSA (2048, 3072, 4096).
*   [x] Verification of RSA-3072 on hardware (Heap increased to 128KB).
*   [x] Analysis of RSA-4096 feasibility (Time/Resource constrained).
**Notes:** 
*   RSA-3072 confirmed functional.
*   RSA-4096 proved prohibitive for interactive demo due to execution time (>80s).
*   Demonstrated cubic scaling vs PQC's linear/log-linear trends.

---

## [M7] Full Automation & Presentation Ready ✅ RELEASED 2026-02-20
**Goal:** Consolidate all independent scripts into a single "One-Click" workflow and prepare the final presentation playbook for safe.tech 2026.
**Status:** Completed ✅
**Deliverables:**
*   [x] **Master Orchestrator**: `run_benchmarking_session.ps1` (Build -> Flash -> Capture -> Plot -> Report).
*   [x] **Automated Reporting**: `update_report.py` dynamically updates `FINAL_REPORT.md` from UART logs and Map files.
*   [x] **Presentation Playbook**: `docs/SAFE_TECH_DEMO_PLAYBOOK.md` with targeted talking points and demo scenarios.
*   [x] **Final Verification**: Successfully captured and integrated RSA-4096 hardware metrics into the final report.
**Notes:**
*   Project is now fully automated; a single command generates the entire technical report from hardware.
*   Ready for live demonstration at safe.tech 2026.
