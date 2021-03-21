@echo off
setlocal enabledelayedexpansion
set numiter=%1
for /f "tokens=1,* delims= " %%a in ("%*") do set args=%%b
for /L %%i in (1,1,%numiter%) do _builds\rtiow\x64\Release\bin\rtiow.exe -o perf_%%i %args%
