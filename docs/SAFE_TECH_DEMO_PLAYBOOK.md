# Safe.Tech 2026: PQC Demo Playbook

This guide outlines the steps to perform a successful live demonstration of Post-Quantum Cryptography (PQC) using the STM32F769 Discovery Kit.

## Goal of the Demo
Showcase the performance gap between classical RSA (3072/4096-bit) and modern PQC algorithms (Kyber/Dilithium) in a constrained embedded environment.

---

## 1. Preparation Phase (5 min before)

### Hardware Setup
1.  **Connect Board**: Use a Micro-USB or Mini-USB cable to connect the ST-LINK port of the **STM32F769I-DISCO** to your laptop.
2.  **Verify LED**: Ensure the ST-LINK LED is solid (green/red) and the board is powered.

### Software Environment
1.  **Open PowerShell**: In the `pqc_demo` root directory.
2.  **Verify Tools**: Ensure you have Python and PowerShell execution permissions.
3.  **Find COM Port**: Check Device Manager to confirm the "STMicroelectronics STLink Virtual COM Port" is active (e.g., `COM5`).

---

## 2. Scenario A: The "One-Click" Demo (Automated)
*Best for a quick overview or during a presentation slot.*

1.  **Action**: Run the master session script:
    ```powershell
    powershell scripts/run_benchmarking_session.ps1
    ```
2.  **What Happens**: 
    - The firmware is built and flashed instantly.
    - The terminal displays live cycle counts for RSA-2048/3072/4096.
    - **Highlight to Audience**: Notice the ~10-15s delay during RSA-4096 Key Generation.
    - PQC algorithms (Kyber/Dilithium) will finish in milliseconds.
3.  **Result**: The script automatically opens/updates `docs/FINAL_REPORT.md` and generates new plots in `docs/assets/`.

---

## 3. Scenario B: Interactive Deep-Dive
*Best for 1-on-1 booth conversations.*

1.  **Start Terminal**: Use the standard `run_demo.ps1` script:
    ```powershell
    powershell scripts/run_demo.ps1
    ```
2.  **Explain the Options**:
    - **Option 1 (RSA)**: Run just RSA to show the "Legacy" struggle.
    - **Option 2 (PQC)**: Run the "Future" suite (Kyber/Dilithium). Point out the **pqm4 Assembly optimization** speedups.
    - **Option 3 (All)**: Full comparison.
3.  **Live Interaction**: Let the visitor type '2' to see the PQC speed.

---

## 4. Key Talking Points ("The Wow Factor")

### The RSA "Scaling Wall"
- **RSA-2048**: ~1.3s for Private Key Op. (Acceptable but slow).
- **RSA-4096**: ~12s for Private Key Op. (Too slow for real-time auth).
- **Point out**: Classical RSA scaling is **cubic**; as security needs go up, the performance drops off a cliff.

### The PQC "Efficiency"
- **ML-DSA-44 (Dilithium)**: Signing in **118ms**.
- **Comparison**: Dilithium is **>20x faster** than RSA-2048 and **>100x faster** than RSA-4096 on this hardware.
- **Assembly Gain**: Mention we use Cortex-M4/M7 assembly (`pqm4`) to gain another 2x speedup over standard C.

---

## 5. Handover
At the end of the demo, point the visitor to the `docs/FINAL_REPORT.md`:
> *"This report was just generated live from the board you see here. It includes the exact cycle counts and memory footprints for all candidates."*

---

**Emergency Reset**: If the board hangs or UART stays silent, press the **Black Reset Button** on the board and restart the script.
