@echo off

setlocal enabledelayedexpansion

set argCount=0
for %%x in (%*) do (
   set /A argCount+=1
   set "argVec[!argCount!]=%%~x"
)

for /L %%i in (1,1,%argCount%) do "%~dp0ck-cmd.exe" exportfbx "!argVec[%%i]!" -e .