
param(
    [string]$RootPath = "..\firmware\ThirdParty\pqm4"
)

$symlinks = Get-ChildItem -Path $RootPath -Recurse -File

foreach ($file in $symlinks) {
    if ($file.Length -lt 255) {
        try {
            # Read first line only
            $content = Get-Content -Path $file.FullName -TotalCount 1
            if ($null -ne $content -and $content -match "^(?:\.\./|/|\\)") {
                $targetRelative = $content.Trim()
                $targetFullPath = [System.IO.Path]::GetFullPath([System.IO.Path]::Combine($file.DirectoryName, $targetRelative))
                
                if (Test-Path $targetFullPath) {
                    Write-Host "Resolving symlink: $($file.Name) -> $targetRelative"
                    Copy-Item -Path $targetFullPath -Destination $file.FullName -Force
                }
                else {
                    # Write-Warning "Target not found for $($file.Name): $targetFullPath"
                }
            }
        }
        catch {
            Write-Warning "Error processing $($file.Name): $_"
        }
    }
}
