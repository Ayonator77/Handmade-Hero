# Run all recipes using real Windows cmd.exe (not sh)
SHELL := C:/Windows/System32/cmd.exe
.SHELLFLAGS := /V:ON /C
.ONESHELL:
# Optional: stop printing each command line
.SILENT:

VSVARS_BAT := d:\Visual Studio\VC\Auxiliary\Build\vcvarsall.bat

SRC := code\win32_handmade.cpp
OUT := build\win32_handmade_msvc.exe

CLFLAGS := /nologo /Zi /W4 /EHsc
LIBS := user32.lib gdi32.lib

# Default target: "make" == "make build"
.DEFAULT_GOAL := build

.PHONY: build run clean rebuild

# If you want "bat-like" behavior (always compile), keep build PHONY:
build:
	if not exist build mkdir build
	call "$(VSVARS_BAT)" x64
	cl $(CLFLAGS) "$(SRC)" /Fe:"$(OUT)" /Fo:"build\\" /Fd:"build\\vc140.pdb" $(LIBS)

run: build
	.\$(OUT)

clean:
	if exist build\*.exe del /q build\*.exe
	if exist build\*.pdb del /q build\*.pdb
	if exist build\*.ilk del /q build\*.ilk
	rem (optional) remove build dir entirely:
	rem if exist build rmdir /s /q build

rebuild: clean build
