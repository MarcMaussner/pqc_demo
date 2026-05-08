param (
    [switch]$SkipBuild,
    [int]$DurationSeconds = 300,
    [string]$Command
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Get-Location

Write-Host "--- PQC Benchmarking Master Script ---" -ForegroundColor Cyan

# 1. Build
if (-not $SkipBuild) {
    Write-Host "[1/6] Building firmware..." -ForegroundColor Yellow
    powershell -ExecutionPolicy Bypass -File scripts/run_build.ps1
}
else {
    Write-Host "[1/6] Skipping build." -ForegroundColor Gray
}

# 2. Flash
Write-Host "[2/6] Flashing target..." -ForegroundColor Yellow
& cmd /c scripts\flash.bat

# 2b. Select Suite
if (-not $Command) {
    Write-Host "`n--- Select Benchmark Suite ---" -ForegroundColor Cyan
    Write-Host "1) RSA Only (Fastest)"
    Write-Host "2) PQC Only"
    Write-Host "3) All (RSA + PQC)"
    $choice = Read-Host "Select option [1-3] (Default: 3)"
    if ([string]::IsNullOrWhiteSpace($choice)) { $choice = "3" }
    $Command = $choice
}

# 3. Capture UART
Write-Host "`n[3/6] Running benchmarks and capturing UART ($DurationSeconds s)..." -ForegroundColor Yellow
powershell -ExecutionPolicy Bypass -File scripts/run_demo.ps1 -DurationSeconds $DurationSeconds -OutputFile "benchmarks.log" -Command $Command

# 4. Generate Plots
Write-Host "[4/6] Generating performance/stack plots..." -ForegroundColor Yellow
python scripts/generate_plots.py benchmarks.log

# 5. Move Assets
Write-Host "[5/6] Updating documentation assets..." -ForegroundColor Yellow
if (Test-Path "benchmark_performance.png") { Move-Item -Force "benchmark_performance.png" "docs/assets/benchmark_performance.png" }
if (Test-Path "benchmark_stack.png") { Move-Item -Force "benchmark_stack.png" "docs/assets/benchmark_stack.png" }

# 6. Update Report
Write-Host "[6/6] Updating FINAL_REPORT.md..." -ForegroundColor Yellow
$MapFile = "firmware/build/pqc_demo.map"
python scripts/update_report.py benchmarks.log $MapFile

Write-Host "`n--- Session Complete! ---" -ForegroundColor Green
Write-Host "Results available in docs/FINAL_REPORT.md and docs/assets/"
