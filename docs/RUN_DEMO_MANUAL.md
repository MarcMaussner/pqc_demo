# PQC Demo: Operation Manual

## Overview
This manual guides you through the operation of the Post-Quantum Cryptography (PQC) Demo on the STM32F769I-DISCO board.

## Prerequisites
- **Hardware**: STM32F769I-DISCO Board connected via ST-LINK (USB Mini-B).
- **Software**: Windows with PowerShell and Python 3.x installed.
- **Drivers**: ST-LINK USB Drivers.

## 1. Flashing the Firmware
Before running the demo, ensure the board is flashed with the latest firmware.

1.  Open a PowerShell terminal in the `pqc_demo` directory.
2.  Run the flash script:
    ```powershell
    .\scripts\flash.bat
    ```
3.  Verify the output contains: `** Verified OK **` and `** Programming Finished **`.

## 2. Running the Demo
The `run_demo.ps1` script automates the interaction with the board via UART.

1.  Run the script:
    ```powershell
    powershell -ExecutionPolicy Bypass -File scripts\run_demo.ps1
    ```
2.  **Select a Benchmark**:
    The script will prompt you to select an option (default is '3' for "Run All"):
    - `1`: Run RSA Benchmark (Baseline)
    - `2`: Run PQC Benchmarks (Kyber, Dilithium, Falcon, SPHINCS+)
    - `3`: Run All (RSA + PQC)

3.  **Monitor Output**:
    The script will display the UART output in real-time. You will see lines like:
    ```
    UART >> RSA: Public Op took 1414 cycles...
    UART >> ML-KEM-512: Keygen took 5197355 cycles...
    ```

4.  **Completion**:
    Once finished, the script saves the log to `benchmarks.log`.

## 3. Generating Performance Plots
To visualize the results:

1.  Run the plotting script:
    ```powershell
    python scripts/generate_plots.py benchmarks.log
    ```
2.  This generates:
    - `benchmark_performance.png`: Cycle counts (log scale).
    - `benchmark_stack.png`: Peak stack usage.

## 4. Troubleshooting
- **No Output?** Ensure the ST-LINK COM port is correct. Edit `scripts/run_demo.ps1` if your COM port is not auto-detected correctly (defaults to COM5 in many cases).
- **Flash Failed?** Check the USB connection and ensure OpenOCD finds the board.
