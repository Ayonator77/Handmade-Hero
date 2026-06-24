@echo off

if "%1" == "build" goto :msvc
if "%1" == "run" goto :run
if "%1" == "clean" goto :clean
if "%1" == "vs" goto :vs

echo Commands:
echo   handmade build    - build with MSVC
echo   handmade run     - build + run
echo   handmade clean   - delete build
echo   handmade vs      - open Visual Studio project
exit /b

:msvc
call build_msvc.bat
exit /b
  
:run
call build_msvc.bat
build\win32_handmade_msvc.exe
exit /b

:clean
del /q build\*.exe *.pdb
exit /b

:vs
call devenv .\build\win32_handmade_msvc.exe ^ code\win32_handmade.cpp
exit /b
