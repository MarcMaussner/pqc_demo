# M4 Release Notes — Operational Manual & RSA-2048 Full Baseline

**Release date:** 2026-02-19
**Tag:** `v1.1.0`
**Status:** ✅ All success criteria met

---

## Scope

Milestone 4 completes the classical RSA-2048 baseline by adding **Key Generation** and **Private Key** (decryption/signing) operation benchmarks. It also introduces an operational user manual (`docs/RUN_DEMO_MANUAL.md`), making the demo accessible without requiring source code inspection.

---

## Deliverables

| Deliverable | Path | Description |
|---|---|---|
| Operational Manual | `docs/RUN_DEMO_MANUAL.md` | Step-by-step guide for flash, run, interpret |
| Updated RSA Harness | `Core/Src/crypto_harness.c` | RSA KeyGen + Private Op via `mbedtls_rsa_gen_key` / `mbedtls_rsa_private` |
| LCG RNG | `Core/Src/crypto_harness.c` | `fake_rng()` — deterministic LCG for reproducible prime generation |
| Updated mbedTLS Config | `Core/Inc/mbedtls/mbedtls_config.h` | Added `MBEDTLS_GENPRIME` |
| Updated Final Report | `docs/FINAL_REPORT.md` | Includes RSA KeyGen and Private Op data |
| Updated Plots | `docs/assets/` | Regenerated `benchmark_performance.png`, `benchmark_stack.png` |

---

## Key Engineering Decisions

| Decision | Rationale |
|---|---|
| LCG `fake_rng` | Deterministic prime search ensures comparable, reproducible KeyGen timing |
| `mbedtls_rsa_gen_key` | Real 2048-bit key required for valid Public/Private Op benchmarks |
| CRT available via `gen_key` | `mbedtls_rsa_private` uses CRT (P, Q, DP, DQ, QP) — realistic performance |
| `MBEDTLS_GENPRIME` flag | Required mbedTLS configuration flag to enable prime number generation |

---

## RSA-2048 Full Benchmark Results

**Target:** STM32F769I-DISCO @ 216 MHz | **Key:** 2048-bit (generated with LCG seed)

| Operation | Cycles | Time (ms) | Stack (B) |
|---|---|---|---|
| **KeyGen** | 2,309,007,365 | ~10,690 | 2,876 |
| **Public Op (Encrypt)** | 10,932,816 | ~50.6 | 2,876 |
| **Private Op (Decrypt)** | 580,734,206 | ~2,688 | 2,876 |

---

## Complete M4 Benchmark Suite

| Algorithm | Operation | Cycles | Time (ms) | Stack (B) |
|---|---|---|---|---|
| **RSA-2048** | KeyGen | 2,309,007,365 | 10,690 | 2,876 |
| **RSA-2048** | Public Op | 10,932,816 | 50.6 | 2,876 |
| **RSA-2048** | Private Op | 580,734,206 | 2,688 | 2,876 |
| **ML-KEM-512** | KeyGen | 5,152,020 | 23.9 | 9,268 |
| **ML-KEM-512** | Encaps | 6,264,054 | 29.0 | 11,900 |
| **ML-DSA-44** | KeyGen | 15,542,697 | 72.0 | 41,524 |
| **ML-DSA-44** | Sign | 57,625,468 | 266.8 | 55,012 |
| **Falcon-512** | KeyGen | 3,146,006,056 | 14,565 | 20,900 |
| **Falcon-512** | Sign | 488,925,219 | 2,264 | 45,636 |
| **SPHINCS+** | KeyGen | 341,958,663 | 1,583 | 3,580 |

---

## Key Finding

> **RSA-2048 Private Key (~2.7 s) is ~10× slower than ML-DSA-44 Sign (~267 ms)** on the STM32F769I Cortex-M7 @ 216 MHz, using reference (non-hardware-accelerated) implementations.
>
> Post-quantum signatures are not only quantum-resistant — they are **already faster** than classical RSA on embedded ARM targets.
