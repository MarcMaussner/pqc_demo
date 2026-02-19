# build_container.ps1 - Build the pqc-build-env Docker image
# Run from: pqc_demo/scripts/ or pqc_demo/
$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$PqcDemo = Split-Path -Parent $ScriptDir
$Dockerfile = Join-Path $PqcDemo "docker\Dockerfile"

if (-not (Test-Path $Dockerfile)) {
    Write-Error "Dockerfile not found at: $Dockerfile"
    exit 1
}

Write-Host "Building pqc-build-env container image..."
podman build -t pqc-build-env -f $Dockerfile $PqcDemo

if ($LASTEXITCODE -ne 0) {
    Write-Error "Container build failed!"
    exit 1
}

Write-Host "Container image 'pqc-build-env' built successfully."
