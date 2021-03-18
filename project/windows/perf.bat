@echo off
setlocal
set numiter=%1
shift
for /L %%i in (1,1,%numiter%) do _builds\rtiow\x64\Release\bin\rtiow.exe %*
