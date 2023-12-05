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