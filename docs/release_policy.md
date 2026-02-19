# Release Policy

This document defines the requirements for releasing a milestone in the `pqc_demo` project.

## Mandatory Artifacts
Each release tag MUST include the following artifacts:
1.  **Source Code**: Full source tree at the time of the release.
2.  **Binaries**: Compiled firmware (`.elf`, `.bin`, `.hex`) for the STM32F769 board.
3.  **Benchmark Report**: A generated `benchmark_results.md` (and PDF) containing latest performance data.
4.  **Quality Audit**: A summary report confirming 100% test coverage and zero linting issues.

## Quality Gates
A release is only permitted if the following checks pass:
- `scripts/quality_gate.ps1` returns success.
- Cross-compilation for STM32F769 succeeds without warnings.
- All unit tests in `tests/` pass.
- Linker map analysis confirms memory usage is within hardware limits.

## Versioning
Milestones follow semantic versioning `vM.m.p-milestone`, where `milestone` is a descriptive name (e.g., `v1.0.0-kyber-base`).
 
