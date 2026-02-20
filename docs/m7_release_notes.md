# M7 Release Notes — Full Automation & Presentation Ready

**Release date:** 2026-02-20
**Tag:** `v1.3.0`
**Status:** ✅ Project Finalized

---

## Scope

Milestone 7 marks the finalization of the PQC Demo project for Safe.Tech 2026. This release focuses on developer productivity and presentation reliability by unifying all scripts into a single automated workflow.

---

## Deliverables

| Deliverable | Path | Description |
|---|---|---|
| Master Orchestrator | `scripts/run_benchmarking_session.ps1` | One script to Build, Flash, Capture, Plot, and Report |
| Automated Reporter | `scripts/update_report.py` | Dynamically updates Markdown tables with live hardware data |
| Demo Playbook | `docs/SAFE_TECH_DEMO_PLAYBOOK.md` | Presentation-focused manual with scripts and talking points |
| Static Resource Map | `docs/FINAL_REPORT.md` | Automated per-module Flash and RAM footprint analysis |

---

## Key Engineering Decisions

| Decision | Rationale |
|---|---|
| HTML Comment Markers | Used in `FINAL_REPORT.md` to allow reliable automated regex-based table replacement |
| Unified Asset Management | Script automatically moves plots to `docs/assets/` to ensure documentation is always current |
| Performance Baselines | Hardcoded M4 "Clean C" values into the reporter to provide instant speedup comparisons |

---

## Resource Utilization (M7 Final Build)

**Target:** STM32F769I-DISCO (2MB Flash / 512KB RAM)

| Resource | Usage (Bytes) | Utilization |
|---|---|---|
| **Flash (ROM)** | 760,517 | **36.3%** |
| **RAM (Static)** | 55,509 | **10.6%** |

---

## Key Finding

> **The entire hardware-to-report pipeline now executes in < 3 minutes.**
>
> By automating the path from C code to a finalized Markdown report (complete with plots and resource analysis), we have ensured that the Safe.Tech demo is reproducible, verifiable, and presentation-ready.
