# M3 Release Notes — Professional Grade Implementation & Reporting

**Release date:** 2026-02-19
**Tag:** `v1.0.0`
**Status:** ✅ All success criteria met

---

## Scope

Milestone 3 transitions the project from a research proof-of-concept to a professional-grade embedded benchmarking platform. It replaces the custom schoolbook RSA with **mbedTLS v3.5.0** and introduces automated resource footprint analysis, advanced visualization, and a comprehensive final report.

---

## Deliverables

| Deliverable | Path | Description |
|---|---|---|
| mbedTLS Integration | `deps/mbedtls/` | Full mbedTLS v3.5.0 source tree |
| mbedTLS Config | `Core/Inc/mbedtls/mbedtls_config.h` | Minimal bare-metal configuration |
| Refactored RSA Harness | `Core/Src/crypto_harness.c` | RSA Public Key Op via `mbedtls_rsa_public` |
| Plot Generator | `scripts/generate_plots.py` | Cycle count + stack usage bar charts |
| Resource Analyzer | `scripts/analyze_resources.py` | GCC `.map` file parser for Flash/RAM |
| Demo Controller | `scripts/run_demo.ps1` | Automated UART capture script |
| Final Report | `docs/FINAL_REPORT.md` | Synthesis of all benchmark and analysis data |
| Performance Plots | `docs/assets/` | `benchmark_performance.png`, `benchmark_stack.png` |

---

## Key Engineering Decisions

| Decision | Rationale |
|---|---|
| mbedTLS v3.5.0 | Official, standards-compliant RSA to replace schoolbook baseline |
| `MBEDTLS_PKCS_V15` padding | PKCS#1 v1.5 is sufficient for performance benchmarking purposes |
| Minimal `mbedtls_config.h` | Only RSA, Bignum, SHA-256 enabled to minimize Flash footprint |
| Static LCG entropy | Deterministic `fake_rng` ensures reproducible benchmark cycles |
| Python plot generation | Automated, reproducible visualizations from raw UART log |
| GCC `.map` file parser | Gives per-module ROM/RAM breakdown without extra tooling |

---

## Resource Footprint

| Resource | Size | STM32F769 Capacity | Utilization |
|---|---|---|---|
| **Flash (ROM)** | 700,938 bytes (685 KB) | 2,048 KB | ~33% |
| **RAM (Static)** | 54,477 bytes (53 KB) | 512 KB | ~10% |

---

## Benchmark Summary (M3 Baseline)

| Algorithm | Operation | Cycles | Time (ms) | Stack (B) |
|---|---|---|---|---|
| **RSA-2048** | Public Op | 1,414 | < 0.01 | 2,868 |
| **ML-KEM-512** | KeyGen | 5,197,355 | 24.1 | 9,260 |
| **ML-KEM-512** | Encaps | 6,293,328 | 29.1 | 11,900 |
| **ML-DSA-44** | KeyGen | 15,636,768 | 72.4 | 41,524 |
| **ML-DSA-44** | Sign | 58,180,881 | 269.4 | 55,012 |
| **Falcon-512** | KeyGen | 3,167,249,380 | ~14,663 | 20,900 |
| **Falcon-512** | Sign | 492,481,582 | 2,280 | 45,592 |

> **Note:** RSA Public Op used a hard-coded dummy key in M3. Real key generation added in M4.

---

## Open Items for M4

- [ ] RSA Private Key Operation benchmark (CRT-accelerated)
- [ ] RSA Key Generation timing
- [ ] Operational manual for `run_demo.ps1`
