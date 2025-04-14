@echo off
setlocal

set PREMAKE_EXE=Premake\premake5.exe
set BUILD_SCRIPT=premake5.lua
set ACTION=vs2022

echo Generating Visual Studio 2022 project files...
echo.

if exist "%PREMAKE_EXE%" (
    call "%PREMAKE_EXE%" --file=%BUILD_SCRIPT% %ACTION%
) else (
    echo ERROR: Premake executable not found at %PREMAKE_EXE%
    pause
    exit /b 1
)

echo.
echo Project files generated successfully.
pause
endlocal