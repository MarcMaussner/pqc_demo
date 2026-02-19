@echo off
SET PROJ=C:\Daten\Projekte\AUTOMO~1\2026_V~1\SAFE~1.TEC\demos\pqc
SET OPENOCD=%PROJ%\tools\openocd\bin\openocd.exe
SET SCRIPTS=%PROJ%\tools\openocd\scripts
SET ELF=C:/Daten/Projekte/AUTOMO~1/2026_V~1/SAFE~1.TEC/demos/pqc/pqc_demo/firmware/build/pqc_demo.elf

echo Project:  %PROJ%
echo OpenOCD:  %OPENOCD%
echo ELF:      %ELF%

"%OPENOCD%" ^
  -s "%SCRIPTS%" ^
  -f board/stm32f769i-disco.cfg ^
  -c "program {%ELF%} verify reset exit" ^
  2>&1
