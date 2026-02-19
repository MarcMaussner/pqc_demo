# Read UART output from STM32 via ST-Link VCP
$portName = "COM5"
$baudRate = 115200
$timeout = 5000

Write-Host "Opening $portName at $baudRate baud..."
$port = New-Object System.IO.Ports.SerialPort($portName, $baudRate, [System.IO.Ports.Parity]::None, 8, [System.IO.Ports.StopBits]::One)
$port.ReadTimeout = $timeout
$port.Open()

Write-Host "Listening for data (5 seconds)..."
try {
    $deadline = [DateTime]::Now.AddSeconds(5)
    while ([DateTime]::Now -lt $deadline) {
        try {
            $line = $port.ReadLine()
            Write-Host "UART >> $line"
        }
        catch [System.TimeoutException] {
            break
        }
    }
}
finally {
    $port.Close()
    Write-Host "Port closed."
}
