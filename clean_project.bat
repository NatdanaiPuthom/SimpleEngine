@echo off

rem delete all .sln files

del *.sln

rem delete all .vcxproj files

for /r %%i in (*.vcxproj) do (
    del "%%i" /q
)

for /r %%i in (*.vcxproj.filters) do (
    del "%%i" /q
)

for /r %%i in (*.vcxproj.user) do (
    del "%%i" /q
)

rem delete all folder and it's content in Bin except "Assets" and "Shaders" folder

set "folderPath=Bin"
cd %folderPath%
for /d %%d in (*) do (
    if /i not "%%d"=="Assets" if /i not "%%d"=="Shaders" (
        rmdir /s /q "%%d"
    )
)

rem Remove files with .pdb extension
for %%f in (*.pdb) do (
    del /q "%%f"
)

cd ..




rem delete Temp folder

set "folderPaths=Temp"
for %%d in (%folderPaths%) do (
    rd /s /q "%%d"
)




set "folderPath=Lib"
set "excludeFile=easy_profiler.lib"
for %%f in ("%folderPath%\*") do (
    echo "%%~nxf" | findstr /i /c:"%excludeFile%" >nul && (
        echo Keeping "%%~nxf"
    ) || (
        del /f /q "%%f"
    )
)




rem delete hidden ".vs" folder

rmdir /s /q .vs

set /p DUMMY=Success cleaning project, press any key to exit