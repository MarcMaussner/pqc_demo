# M6 Release Notes — RSA Scalability Analysis

**Release date:** 2026-02-20
**Status:** ✅ Completed

---

## Scope

Milestone 6 expands the classical RSA baseline beyond 2048 bits to demonstrate the computational scalability limits of RSA-3072 and RSA-4096. This release establishes the "cubic scaling" proof-of-concept required for the Safe.Tech presentation.

---

## Deliverables

| Deliverable | Path | Description |
|---|---|---|
| Scalability Harness | `Core/Src/crypto_harness.c` | Loop-based benchmarking for 2048, 3072, and 4096-bit keys |
| Expanded Heap | `STM32F769NIHx_FLASH.ld` | Heap increased to **128 KB** to support multi-precision math for RSA-4096 |
| Scalability Data | `docs/FINAL_REPORT.md` | Comparative table of RSA sizes vs cycle counts |
| Scalability Plot | `docs/assets/` | Visual representation of RSA performance degradation with key size |

---

## Key Engineering Decisions

| Decision | Rationale |
|---|---|
| 128 KB Heap | MbedTLS requires substantial heap memory for 3072 and 4096-bit prime generation and modular exponentiation |
| Timeout Increase | RSA-4096 KeyGen can take >15 seconds; capture scripts were updated to accommodate longer runtimes |

---

## RSA Scalability Benchmark Results

**Target:** STM32F769I-DISCO @ 216 MHz

| Key Size | KeyGen (Cycles) | Public Op (Cycles) | Private Op (Cycles) |
|---|---|---|---|
| **RSA-2048** | 2,195,917,567 | 11,276,473 | 593,035,188 |
| **RSA-3072** | 1,922,051,975* | 21,016,683 | 1,388,724,897 |
| **RSA-4096** | 3,561,821,795* | 37,344,305 | 2,639,596,869 |

*\*KeyGen timing varies significantly due to the probabilistic nature of prime number search.*

---

## Key Finding

> **RSA-4096 Private Key operations take >12 seconds per execution.**
>
> Moving from 2048 to 4096 bits roughly **quadruples** the private key operation time, highlighting why classical RSA is becoming impractical for modern high-security embedded requirements compared to PQC.
