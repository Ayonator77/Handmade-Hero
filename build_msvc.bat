@echo off
setlocal

:: Load MSVC compiler environment (x64 toolchain)
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64

set SRC=code\win32_handmade.cpp
set OUT=build\win32_handmade_msvc.exe

if not exist build mkdir build

cl /nologo /Zi /W4 /EHsc %SRC% /Fe:%OUT% user32.lib gdi32.lib

endlocal
