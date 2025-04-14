@echo off


del *.sln
del /q Source\Launcher\Resources\*.aps

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

cd ..

set /p DUMMY=Project cleaned, press any key to exit.