# Release Notes - Milestone 2: Optimization & Extended Analysis (v0.2.0-m2)

## Summary
Milestone 2 successfully extended the PQC benchmarking suite, implemented memory profiling, and verified the project's stability under the high memory demands of modern post-quantum algorithms on the STM32F769 board.

## New Features
- **Integrated Extended PQC Suite**: Added Falcon-512 and SPHINCS+-SHA2-128s-simple to the benchmark harness.
- **Stack Watermarking**: Added peak stack usage tracking using magic pattern filling.
- **Automated Reporting**: Added a Python-based reporting script to parse hardware logs and generate markdown tables.

## Performance & Memory Analysis
- **Stack Driver**: Dilithium2 (ML-DSA-44) was identified as the primary memory driver, requiring up to **55KB** of stack for signing.
- **Execution Profile**: Falcon-512 Keygen (~14.7s) and SPHINCS+ Sign (~18.4s) represent the most compute-intensive operations in the current suite.
- **Linker Configuration**: The minimum stack size has been increased to **128KB** to ensure reliable operation for all candidates.

## Benchmark Table

| Algorithm | Operation | Clock Cycles | Time (ms) | Peak Stack (B) |
|---|---|---|---|---|
| RSA-2048 | Public Op | 3.97 G | 18,382 | 3,392 |
| ML-DSA-44 | Keygen | 15.8 M | 73 | 41,516 |
| ML-DSA-44 | Sign | 58.1 M | 269 | 55,004 |
| ML-KEM-512| Keygen | 5.2 M | 24 | 9,260 |
| ML-KEM-512| Encaps | 6.3 M | 29 | 11,892 |
| Falcon-512 | Keygen | 3.19 G | 14,773 | 20,892 |
| Falcon-512 | Sign | 496.7 M | 2,300 | 45,628 |
| SPHINCS+ | Keygen | 370.1 M | 1,714 | 3,392 |
| SPHINCS+ | Sign | 3.97 G | 18,383 | 5,300 |

## Repository Status
- **Tag**: `v0.2.0-m2`
- **Linker Script**: `STM32F769NIHx_FLASH.ld` updated to 128KB stack.
- **Scripts**: `read_uart.ps1` fixed for long-duration captures.
