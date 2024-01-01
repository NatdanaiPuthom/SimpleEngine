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

setlocal enabledelayedexpansion

SET "sourcedir=Bin"
SET "keepdir=Assets Shaders"
SET "keepfile=imgui.ini levels.json settings.json SimpleEngine_Simple.exe SimpleEngine_Debug.exe SimpleEngine_Release.exe easy_profiler.dll fmod.dll fmodL.dll fmodstudio.dll fmodstudioL.dll"

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

endlocal

cd ..

setlocal enabledelayedexpansion

SET "sourcedir=Lib"
SET "keepdir=Debug"
SET "keepfile=easy_profiler.dll SoundEngine-FMod.lib SoundEngine-FMod.pdb"

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

endlocal

set /p DUMMY=Project cleaned, press any key to exit.