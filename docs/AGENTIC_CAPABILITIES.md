# Agentic Team Capabilities & Workflows

This document outlines the specialized roles and automated workflows governing the **Titanium Grade** development and research process for the PQC (Post-Quantum Cryptography) demonstration project.

## 🛡️ Agentic Roles

The team is composed of specialized agents, each acting as a gatekeeper for specific quality metrics or domain expertise.

### 🏗️ Architecture & Planning
*   **Architect**: Translates high-level research goals into technical subgoals and system designs.
*   **Research Lead**: Responsible for chapter planning, mathematical accuracy, and algorithm validation.
*   **Product Manager**: Liaison for the User; maintains the requirement backlog in `docs/requirements.md`.

### 🧪 Construction (The TDD Trio)
*   **Test Engineer (Challenger)**: Defines constraints by writing strict, failing atomic unit tests (Red Phase).
*   **Software Developer (Solver)**: Implements minimal logic to satisfy failing tests (Green Phase).
*   **TDD Watchguard (Referee)**: Strictly enforces the Red-Green-Refactor cycle, blocking any implementation not preceded by a test.

### 💎 Quality Assurance & Auditing
*   **Refactoring Specialist**: Restructures functional code to meet complexity standards (Cyclomatic Complexity < 5).
*   **Documentation Artisan**: Ensures the codebase is strictly typed and documented with Google-style docstrings.
*   **Titanium Auditor**: The final gatekeeper. Verifies that Coverage is 100% and Lint Errors are 0.
*   **Environment Auditor**: Verifies the stability of the toolchain and container environment.

### 🔐 Domain Experts
*   **Cryptographic Expert / Cipher-X**: Conducts deep security analysis and cryptanalysis of primitives (RSA vs. PQC).
*   **Quantum Specialist / Dr. Entangle**: Designs quantum circuits and analyzes post-quantum robustness.
*   **Embedded Firmware Engineer**: Optimizes PQC implementations for the STM32F769 (ARM Cortex-M7).
*   **Hardware Benchmarking Specialist**: Captures cycle-accurate performance data and memory footprints on target.

### ✍️ Scholarly Output
*   **Technical Writer**: Authors the LaTeX documents, ensuring pedagogical flow and correct notation.
*   **The Proof-Master**: Mathematically verifies all proofs, complexity bounds, and numeric stability.
*   **Der Übersetzer & Dr. Germanicus**: Coordinate the high-fidelity translation and auditing of technical content from English to German.

---

## 🔄 Core Workflows

Workflows are codified processes found in `.agent/workflows/` that guide the agents through complex tasks.

| Workflow | Description |
| :--- | :--- |
| **Titanium Construction (TDD)** | A strict "Ping-Pong" cycle between Test Engineer and Developer to ensure 100% test-driven logic. |
| **PQC Benchmarking** | Standardized process for flashing firmware, acquiring serial metrics, and generating comparison reports. |
| **Computation Pipeline** | Automated generation of figures and tables from Python simulations for direct TeX integration. |
| **Scientific Script Genesis** | Multi-agent process for drafting and deepening research chapters from initial outlines. |
| **Toolchain Provisioning** | Automated setup of the embedded development environment (ARM GCC, OpenOCD, etc.). |
| **Knowledge Base Sync** | The process of distilling "Lessons Learned" from the activity log back into the agent memory. |

---

> [!IMPORTANT]
> **Enforcement Policy**: No code enters the master branch without passing the **Titanium Auditor** gate, which requires 100% test coverage and zero linting violations.
