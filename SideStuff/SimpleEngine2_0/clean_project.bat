@echo off
setlocal

echo Cleaning Visual Studio generated files...
echo.

:: Delete solution files
for %%f in (*.sln) do (
    del "%%f" /q && echo Deleted: %%f
)

:: Delete .vs folder
if exist ".vs" (
    rmdir /s /q ".vs" && echo Deleted folder: .vs
)

:: Delete project-related files
set "extensions=vcxproj vcxproj.filters vcxproj.user"

for %%e in (%extensions%) do (
    for /r %%i in (*%%e) do (
        del "%%i" /q && echo Deleted: %%~nxi
    )
)

:: Delete folders like Temp and Local
set "folders=Temp Local"

for %%f in (%folders%) do (
    if exist "%%f" (
        rmdir /s /q "%%f" && echo Deleted: %%f
    )
)

echo.
echo Project cleaned successfully.
pause
endlocal
