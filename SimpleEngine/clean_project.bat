@echo off


del *.sln

rmdir /s /q .vs

for /r %%i in (*.vcxproj) do (
    del "%%i" /q
)

for /r %%i in (*.vcxproj.filters) do (
    del "%%i" /q
)

for /r %%i in (*.vcxproj.user) do (
    del "%%i" /q
)

set "folderPaths=Temp"

for %%d in (%folderPaths%) do (
    rd /s /q "%%d"
)

set "folderPaths=Local"

for %%d in (%folderPaths%) do (
    rd /s /q "%%d"
)

setlocal enabledelayedexpansion

SET "sourcedir=Bin"
SET "keepdir=Assets"
SET "keepfile="

FOR /d %%a IN ("%sourcedir%\*") DO (
    set "folderName=%%~nxa"
    set "keepFolder="
    for %%k in (%keepdir%) do (
        if /i "!folderName!"=="%%~k" set "keepFolder=true"
    )
    if not defined keepFolder RD /S /Q "%%a"
)

FOR %%a IN ("%sourcedir%\*") DO (
    set "fileName=%%~nxa"
    set "keepFile="
    for %%k in (%keepfile%) do (
        if /i "!fileName!"=="%%~k" set "keepFile=true"
    )
    if not defined keepFile DEL "%%a"
)


SET "sourcedir=Dependencies"
SET "keepdir=SimpleLib"
SET "keepfile="

for /f %%a in ('dir /b /ad "%sourcedir%"') do (
    if /i "%%a" equ "%keepdir%" (
        RD /S /Q "%sourcedir%\%%a"
    )
)

del /q "%sourcedir%\%keepfile%"

endlocal

cd ..

set /p DUMMY=Project cleaned, press any key to exit.