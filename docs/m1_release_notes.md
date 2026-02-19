# M1 Release Notes — Baseline Cryptography & Integration

**Release date:** 2026-02-19  
**Tag:** `v0.2.0-m1`  
**Status:** ✅ All success criteria met

---

## Scope

Milestone 1 integrates the classical RSA-2048 baseline and the first set of Post-Quantum Cryptographic (PQC) algorithms: ML-DSA-44 (Dilithium2) and ML-KEM-512 (Kyber512). It also establishes a unified hardware benchmarking harness using the Cortex-M7 DWT cycle counter.

---

## Deliverables

| Deliverable | Path | Description |
|---|---|---|
| BigInt Library | `RSA/Src/bigint.c` | Custom 2048-bit BigInt arithmetic supporting modular exponentiation |
| RSA Implementation | `RSA/Src/rsa.c` | RSA public key operation wrapper for classical baseline |
| PQC Sources | `deps/PQClean/` | Verified standard-compliant C sources for ML-DSA and ML-KEM |
| Benchmark Harness| `Core/Src/crypto_harness.c`| Unified API for measuring RSA and PQC performance |
| Cycle Counter | `Core/Src/cycles.c` | ARM DWT-based high-resolution timing (216 MHz) |
| Random Source | `Core/Src/randombytes.c` | Deterministic platform-specific entropy for reproducible benchmarks |

---

## Build Verification

**Toolchain:** arm-none-eabi-gcc 12.2.1 (Ubuntu 22.04 container)  
**Memory Configuration:** Stack increased to 32KB to support PQC frame requirements.

### Binary Size (Milestone 1)

```
   text    data     bss     dec     hex filename
  56608     124   51284  108016   1a5f0 pqc_demo.elf
```

- **Flash usage:** 56,732 bytes of 2,097,152 (2.7%)
- **RAM usage:** 51,408 bytes of 524,288 (9.8%) — Increased significantly due to static PQC buffers and stack size.

---

## Hardware Benchmark Results

**Target:** STM32F769I-DISCO @ 216MHz  
**Measurement:** ARM DWT Cycle Counter (1 cycle = ~4.63 ns)

| Algorithm | Operation | Clock Cycles | Time (ms) |
|---|---|---|---|
| **RSA-2048** | Public Key Op | 473,378,265 | 2,191.56 |
| **ML-DSA-44** | Keypair Gen | 15,494,509 | 71.73 |
| **ML-DSA-44** | Signing | 57,169,463 | 264.67 |
| **ML-DSA-44** | Verification | 17,226,345 | 79.75 |
| **ML-KEM-512** | Keypair Gen | 5,155,302 | 23.87 |
| **ML-KEM-512** | Encapsulation | 6,173,518 | 28.58 |
| **ML-KEM-512** | Decapsulation | 7,840,532 | 36.30 |

---

## Key Engineering Decisions

| Decision | Rationale |
|---|---|
| Custom BigInt for RSA | MbedTLS 3.x required complex submodule handling; custom layer provided immediate functional baseline |
| 32KB Stack Allocation | PQC algorithms (especially Dilithium) utilize significant stack for local arrays |
| Static PQC Buffers | Moved large keys/signatures to BSS to prevent HardFaults on the limited 2KB default stack |
| Deterministic `randombytes`| Necessary for benchmarking reproducibility on the Cortex-M7 target |
| PQClean Common Exclusion| Excluded architecture-specific Keccak SIMD implementations (x86/AVX) from the build |

---

## Open Items for M2

- [ ] Implementation of memory profiling (watermarking) for SRAM usage
- [ ] Integration of Falcon-512 and SPHINCS+ algorithms
- [ ] Automation of LaTeX report generation from UART CSV data
- [ ] Investigation of hardware-accelerated RNG integration
