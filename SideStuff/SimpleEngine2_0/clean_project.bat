@echo off
setlocal

echo Cleaning Visual Studio generated files...
echo.

for %%f in (*.sln) do (
    del "%%f" /q && echo Deleted: %%f
)

if exist ".vs" (
    rmdir /s /q ".vs" && echo Deleted: .vs
)

set "extensions=vcxproj vcxproj.filters vcxproj.user"

for %%e in (%extensions%) do (
    for /r %%i in (*%%e) do (
        del "%%i" /q && echo Deleted: %%~nxi
    )
)

set "folders=Temp Local"

for %%f in (%folders%) do (
    if exist "%%f" (
        rmdir /s /q "%%f" && echo Deleted: %%f
    )
)

set "libPaths=Dependencies\Lib\Debug Dependencies\Lib\Release Dependencies\Lib\Retail"

for %%p in (%libPaths%) do (
    if exist "%%p" (
        for %%f in ("%%p\*") do (
            if exist "%%f" (
                echo Deleted: %%~nxf
                del /q "%%f"
            )
        )
    )
)

set "binPaths=Bin\Debug Bin\Release Bin\Retail"

for %%p in (%binPaths%) do (
    if exist "%%p" (
        for %%f in ("%%p\*") do (
            if exist "%%f" (
                echo Deleted: %%~nxf
                del /q "%%f"
            )
        )
	echo Deleted: %%p
	rd /s /q "%%p"
    )
)

echo.
echo Project cleaned successfully.
pause
endlocal
