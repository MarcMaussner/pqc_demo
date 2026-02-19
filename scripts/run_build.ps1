# Run Build in Container
$ErrorActionPreference = "Stop"

$ProjectRoot = "C:\Daten\Projekte\AUTOMO~1\2026_V~1\SAFE~1.TEC\demos\pqc"

# Resolve full path to be safe for mounting
# Use Convert-Path to resolve tilde
if (Test-Path $ProjectRoot) {
    $ProjectRootResolved = Convert-Path $ProjectRoot
}
else {
    Write-Error "Project Root not found: $ProjectRoot"
    exit 1
}

Write-Host "Project Root Resolved: $ProjectRootResolved"
Write-Host "Running Build inside Container..."

# Define command properly to avoid quoting issues
# We use toolchain_container.cmake relative to build dir (which is inside firmware)
# So ../toolchain_container.cmake is correct.
# And source dir is ..
$BuildCmd = "mkdir -p build && cd build && cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=../toolchain_container.cmake .. && ninja"

# Volume mount: Host Path : Container Path
# Workdir: where we run the command
podman run --rm -v "${ProjectRootResolved}:/workspace" -w /workspace/pqc_demo/firmware pqc-build-env bash -c $BuildCmd

if ($LASTEXITCODE -eq 0) {
    Write-Host "Container Build Successful!"
}
else {
    Write-Error "Container Build Failed!"
    exit 1
}
