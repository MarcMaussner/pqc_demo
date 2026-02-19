# M0 Release Notes — Environment Setup & Automation

**Release date:** 2026-02-19  
**Tag:** `v0.1.0-m0`  
**Status:** ✅ All success criteria met

---

## Scope

Milestone 0 establishes the complete development and automated testing environment for the PQC benchmarking project. No cryptographic algorithms are benchmarked yet — this milestone sets the foundation.

---

## Deliverables

| Deliverable | Path | Description |
|---|---|---|
| Dockerfile | `docker/Dockerfile` | Ubuntu 22.04 build environment with ARM GCC 12.2, CMake 3.x, Ninja |
| Build script | `scripts/run_build.ps1` | CMake + Ninja build inside container |
| Flash script | `scripts/flash.bat` | OpenOCD flash + verify via ST-Link (with TCL path fix) |
| UART reader | `scripts/read_uart.ps1` | PowerShell serial port reader, COM5 @ 115200 |
| Firmware | `firmware/` | HAL-based STM32F769 bare-metal firmware |
| Linker script | `firmware/STM32F769NIHx_FLASH.ld` | Memory layout: 2MB Flash / 512KB RAM |
| Startup | `firmware/Core/Src/startup_stm32f769xx.s` | Cortex-M7 vector table + Reset_Handler |

---

## Build Verification

**Toolchain:** arm-none-eabi-gcc 12.2.1 (Ubuntu 22.04 container)  
**Build system:** CMake 3.x + Ninja  
**Build command:** `podman run pqc-build-env cmake --build firmware/build`

### Binary Size

```
   text    data     bss     dec     hex filename
   9724      20    3244   12988    32bc pqc_demo.elf
```

- **Flash usage:** 9,744 bytes of 2,097,152 (0.46%)
- **RAM usage:** 3,264 bytes of 524,288 (0.62%)

---

## Flash Verification

**Tool:** OpenOCD 0.12.0 (xPack)  
**Interface:** ST-Link V2J40M27 (on-board, VID:PID 0483:374B)  
**Protocol:** SWD @ 1800 kHz  
**Target:** STM32F769NI (2048 KiB Flash, device ID 0x10016451)

```
** Programming Started **
Info : device id = 0x10016451
Info : flash size = 2048 KiB
** Programming Finished **
** Verify Started **
** Verified OK **
** Resetting Target **
```

---

## UART Verification

**Port:** COM5 (ST-Link Virtual COM Port)  
**Settings:** 115200-8N1  
**Observed output:**

```
UART >> PQC Demo: Environment Setup Successful
UART >> PQC Demo: Running...
```

---

## Key Engineering Decisions

| Decision | Rationale |
|---|---|
| Containerized build | Windows long-path + Umlaut in path caused CMake/GCC failures in native build |
| HAL conf from template | Minimal hand-crafted conf was missing `HSE_STARTUP_TIMEOUT`, `assert_param` etc. |
| `-lnosys` + custom `syscalls.c` | `-lnosys` provides `_exit` but NOT `_sbrk`/`_write` — both needed with `nano.specs` |
| `-DNDEBUG` flag | Prevents `assert_param` from calling external `assert_failed()` at link time |
| TCL curly-brace path | OpenOCD strips backslashes from program paths — use `program {/fwd/slash/path}` |
| USART1 @ PA9/PA10 | Standard ST-Link VCP mapping on STM32F769I-DISCO |

---

## Open Items for M1

- [ ] Integrate RSA (MbedTLS or custom bare-metal implementation)
- [ ] Integrate Dilithium2 / Kyber512 from PQClean
- [ ] Implement DWT cycle counter (`cycles.c`)
- [ ] UART CSV output format for benchmark data
