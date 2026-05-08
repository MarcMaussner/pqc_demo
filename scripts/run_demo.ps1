param (
    [int]$DurationSeconds = 60,
    [string]$OutputFile = "benchmarks.log",
    [string]$Command = "3"
)

$portName = "COM5"
$baudRate = 115200
$timeout = 5000

Write-Host "Opening $portName at $baudRate baud..."
$port = New-Object System.IO.Ports.SerialPort($portName, $baudRate, [System.IO.Ports.Parity]::None, 8, [System.IO.Ports.StopBits]::One)
$port.ReadTimeout = $timeout

try {
    $port.Open()
    
    # Wait for board to boot/menu
    Start-Sleep -Seconds 2
    
    # Send the selected command
    Write-Host "Sending command '$Command'..."
    $port.Write($Command)
    
    Write-Host "Capturing output to $OutputFile ($DurationSeconds s)..."
    $deadline = [DateTime]::Now.AddSeconds($DurationSeconds)
    
    # Clear file content
    Set-Content -Path $OutputFile -Value ""
    
    while ([DateTime]::Now -lt $deadline) {
        try {
            $line = $port.ReadLine()
            Write-Host "Rx: $line"
            Add-Content -Path $OutputFile -Value $line
        }
        catch [System.TimeoutException] {
            continue
        }
    }
}
catch {
    Write-Host "Error: $_"
}
finally {
    if ($port.IsOpen) { $port.Close() }
    Write-Host "Port closed."
}
