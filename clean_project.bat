@echo off

rem delete all .sln files
del *.sln

rem delete all files and folder in Bin except "Assets" folder
set "folderPath=Bin"
cd %folderPath%
for /d %%d in (*) do (
    if /i not "%%d"=="Assets" (
        rmdir /s /q "%%d"
    )
)
cd ..

rem delete Temp folder
set "folderPaths=Temp Lib"
for %%d in (%folderPaths%) do (
    rd /s /q "%%d"
)

rem delete hidden ".vs" folder
rmdir /s /q .vs

set /p DUMMY=Success cleaning project, press any key to exit