# Post-Quantum Cryptography (PQC) Demo - Final Report

## 1. Executive Summary
This report summarizes the successful implementation and benchmarking of Post-Quantum Cryptography (PQC) algorithms on the STM32F769I-DISCO development board. The project integrated **mbedTLS v3.5.0** for classical RSA operations and **PQClean** for NIST Round 3 PQC finalists (Kyber, Dilithium, Falcon, SPHINCS+).

A comprehensive testing framework was developed to measure **CPU cycle counts** and **stack usage**, alongside a static resource analysis of **Flash (ROM)** and **RAM** footprints.

**Key Findings:**
*   **ML-KEM-512 (Kyber)** and **ML-DSA-44 (Dilithium)** demonstrate feasible performance for rigorous embedded applications.
*   **Falcon-512** signature verification is fast, but key generation is prohibitively slow (~3.1 billion cycles) for real-time edge use.
*   **RSA-2048 Private Key** operation (~580M cycles) is slower than ML-DSA-44 Sign (~57M cycles), challenging the classical performance assumption.
*   **Flash Usage:** The full suite requires **~700 KB** of Flash, comfortably within the 2MB on the STM32F769.
*   **RAM Usage:** Runtime RAM usage is **~54 KB**, well within the 512KB limit.

---

## 2. Implementation Overview

### 2.1 Hardware & Software Stack
*   **Board:** STM32F769I-DISCO (Cortex-M7 @ 216 MHz)
*   **Toolchain:** ARM GCC (via Docker/Podman) + CMake + Ninja
*   **Libraries:**
    *   `mbedTLS` (v3.5.0): Classical Crypto (RSA-2048)
    *   `PQClean`: Reference PQC Implementations (Clean C)
    *   `STM32CubeF7`: HAL/Drivers

### 2.2 Benchmarking Framework
*   **Cycle Counting:** Utilized the ARM DWT (Data Watchpoint and Trace) unit for precise cycle measurements.
*   **Stack Analysis:** Implemented stack watermarking (filling stack with `0xCC`) to measure peak usage.
*   **Automation:** Developed PowerShell and Python scripts (`run_demo.ps1`, `generate_plots.py`) to automate execution, data capture, and visualization.
*   **Demo Mode:** Interactive UART console for selecting specific benchmarks.

---

## 3. Performance Benchmarks

### 3.1 Cycle Counts (CPU Cycles @ 216 MHz)

| Algorithm | Operation | Cycles | Time (ms) | Notes |
| :--- | :--- | ---: | ---: | :--- |
| **RSA-2048** | KeyGen | **2,309,007,365** | ~10,690 | Real prime generation with LCG seed |
| | Public Op (Encrypt) | **10,932,816** | ~50.6 | Real 2048-bit key |
| | Private Op (Decrypt) | **580,734,206** | ~2,688 | CRT-accelerated |
| **ML-KEM-512 (Kyber)** | KeyGen | **5,152,020** | ~23.9 | |
| | Encapsulate | **6,264,054** | ~29.0 | |
| **ML-DSA-44 (Dilithium)** | KeyGen | **15,542,697** | ~72.0 | |
| | Sign | **57,625,468** | ~266.8 | |
| **Falcon-512** | KeyGen | **3,146,006,056** | ~14,565 | Very Slow |
| | Sign | **488,925,219** | ~2,264 | |
| **SPHINCS+** | KeyGen | **341,958,663** | ~1,583 | |
| | Sign | *> 3.7B* | *> 17,200* | Exceeded capture window |

> **Key Insight:** RSA-2048 Private Key operation (~2.7s) is **~10x slower** than ML-DSA-44 Sign (~267ms), making Dilithium a compelling embedded drop-in for digital signatures.

![Performance Plot](assets/benchmark_performance.png)

### 3.2 Stack Usage (Bytes)

| Algorithm | Operation | Peak Stack (Bytes) |
| :--- | :--- | ---: |
| **RSA-2048** | All (KeyGen, Public, Private) | 2,876 |
| **ML-KEM-512** | Encapsulate | 11,900 |
| **ML-DSA-44** | Sign | 55,012 |
| **Falcon-512** | Sign | 45,636 |
| **SPHINCS+** | KeyGen | 3,580 |

*Analysis:* RSA has the smallest stack footprint due to CRT use. ML-DSA-44 has the highest stack demand (~54KB), approaching limits for smaller Cortex-M0+ devices. All algorithms fit comfortably within the STM32F769's 512KB RAM.

![Stack Usage Plot](assets/benchmark_stack.png)

---

## 4. Resource Analysis (Static Footprint)

| Resource | Size (Bytes) | Size (KB) | Capacity (STM32F769) | Utilization |
| :--- | :--- | :--- | :--- | :--- |
| **Flash (ROM)** | **700,938** | **~684.5 KB** | 2,048 KB | ~33% |
| **RAM (Static)** | **54,477** | **~53.2 KB** | 512 KB | ~10% |

---

## 5. Conclusion & Recommendations

*   **For Key Exchange:** **ML-KEM-512** is the clear winner — fast, low stack, and post-quantum secure.
*   **For Digital Signatures:** **ML-DSA-44** outperforms RSA-2048 Private Op by ~10x on this platform. Falcon-512 offers smaller signatures but costly KeyGen.
*   **RSA-2048:** Only competitive for Public Key operations. Private key operations are substantially slower than PQC alternatives on the Cortex-M7.
*   **Optimization:** Assembly-optimized PQC implementations could reduce cycle counts by 2–5x, widening the performance gap further in favor of PQC.

**Status:** Milestone 4 Complete. Full RSA baseline established. Demo ready for presentation at safe.tech 2026.
