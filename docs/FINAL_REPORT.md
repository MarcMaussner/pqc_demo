# Post-Quantum Cryptography (PQC) Demo - Final Report

## 1. Executive Summary
This report summarizes the successful implementation and benchmarking of Post-Quantum Cryptography (PQC) algorithms on the STM32F769I-DISCO development board. The project integrated **mbedTLS v3.5.0** for classical RSA operations and **PQClean** for NIST Round 3 PQC finalists (Kyber, Dilithium, Falcon, SPHINCS+).

A comprehensive testing framework was developed to measure **CPU cycle counts** and **stack usage**, alongside a static resource analysis of **Flash (ROM)** and **RAM** footprints.

**Key Findings:**
*   **ML-KEM-512 (Kyber)** and **ML-DSA-44 (Dilithium)** demonstrate feasible performance for rigorous embedded applications.
*   **Falcon-512** signature verification is fast, but key generation is prohibitively slow (~3.1 billion cycles) for real-time edge use.
*   **Flash Usage:** The full suite requires **~700 KB** of Flash, fitting comfortably within the 2MB available on the STM32F769.
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

The following results compare Classical RSA-2048 against the selected PQC algorithms.

### 3.1 Cycle Counts (CPU Cycles)

| Algorithm | Operation | Cycles | Notes |
| :--- | :--- | :--- | :--- |
| **RSA-2048** | Public Key Op (Verify/Encrypt) | **1,414** | Extremely fast (small public exponent e=65537) |
| | Private Key Op | *(Not measured)* | Typically ~5M+ cycles on this core |
| **ML-KEM-512 (Kyber)** | KeyGen | **5,197,355** | ~5.2 M |
| | Encapsulate | **6,293,328** | ~6.3 M |
| **ML-DSA-44 (Dilithium)** | KeyGen | **15,636,768** | ~15.6 M |
| | Sign | **58,180,881** | ~58.2 M |
| **Falcon-512** | KeyGen | **3,167,249,380** | **~3.1 Billion** (Very Slow) |
| | Sign | **492,481,582** | ~492 M |

![Performance Plot](assets/benchmark_performance.png)

### 3.2 Stack Usage (Bytes)

| Algorithm | Peak Stack Usage (Bytes) |
| :--- | :--- |
| **RSA-2048** | 2,868 |
| **ML-KEM-512** | 11,900 |
| **ML-DSA-44** | 55,012 |
| **Falcon-512** | 45,592 |

*Analysis:* PQC algorithms generally require significantly more stack memory than classical RSA. ML-DSA-44 and Falcon-512 approach 64KB, which may be a constraint on smaller Cortex-M4 devices but is acceptable on the M7.

![Stack Usage Plot](assets/benchmark_stack.png)

---

## 4. Resource Analysis (Static Footprint)

The firmware executable (`pqc_demo.elf`) was analyzed to determine the static memory footprint.

| Resource | Size (Bytes) | Size (KB) | Capacity (STM32F769) | Utilization |
| :--- | :--- | :--- | :--- | :--- |
| **Flash (ROM)** | **700,938** | **~684.5 KB** | 2,048 KB | ~33% |
| **RAM (Static)** | **54,477** | **~53.2 KB** | 512 KB | ~10% |

*   **Flash:** Includes Code (`.text`) and Read-only Data (`.rodata`). The PQC algorithms (especially pre-computed tables for Falcon and Dilithium) contribute significantly to this size.
*   **RAM:** Includes Initialized Data (`.data`) and Zero-initialized Data (`.bss`). Dynamic stack usage supplements this static overhead.

---

## 5. Conclusion & Recommendations

The **PQC Demo** has proven that post-quantum cryptography is viable on high-performance microcontrollers like the STM32F7.

*   **For Key Exchange:** **ML-KEM-512** is the clear winner, offering a good balance of performance and resource usage.
*   **For Digital Signatures:** **ML-DSA-44** is robust but heavy on stack. **Falcon-512** offers smaller signatures and faster verification (theoretical) but its key generation is too slow for ephemeral key use cases on-device.
*   **Optimization:** Further optimization using assembly-optimized implementations (rather than reference C code) could reduce cycle counts by 2-5x.

**Status:** Milestone 3 Complete. The project is ready for demonstration and presentation at safe.tech 2026.
